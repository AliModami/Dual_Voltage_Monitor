/******************************************************************************
 * @file    lcd_i2c.c
 * @brief   Character LCD I2C driver implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file implements the driver layer for HD44780 compatible
 *      character LCD displays connected through PCF8574 I2C expanders.
 *
 *      Supported displays:
 *
 *          - LCD 20x4
 *          - LCD 16x2
 *
 *      Communication:
 *
 *          Application
 *               |
 *               v
 *          LCD Driver
 *               |
 *               v
 *          HAL I2C
 *               |
 *               v
 *          PCF8574
 *               |
 *               v
 *          LCD Controller
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      1. Application modules never access HAL_I2C directly.
 *
 *      2. All LCD low level commands remain private.
 *
 *      3. Timing values are controlled through configuration file.
 *
 *      4. No blocking delays are used except LCD controller required delays.
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 * Created :
 *      2026-07-08
 *
 ******************************************************************************/



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "lcd_i2c.h"

#include "lcd_config.h"

#include "main.h"

#include <string.h>

#include <stdio.h>



/******************************************************************************
 *                         Private Constants
 ******************************************************************************/

/*
 * PCF8574 pin mapping.
 *
 * Most LCD backpacks use this standard mapping:
 *
 *      P0 -> RS
 *      P1 -> RW
 *      P2 -> EN
 *      P3 -> Backlight
 *      P4-P7 -> Data D4-D7
 *
 */


/*
 * Register Select pin.
 */

#define LCD_RS_BIT                 (0x01U)



/*
 * Enable pin.
 */

#define LCD_EN_BIT                 (0x04U)



/*
 * Backlight control bit.
 */

#define LCD_BACKLIGHT_BIT          (0x08U)



/*
 * LCD command values.
 */

#define LCD_COMMAND_CLEAR          (0x01U)

#define LCD_COMMAND_HOME           (0x02U)

#define LCD_COMMAND_FUNCTION       (0x28U)

#define LCD_COMMAND_DISPLAY        (0x0CU)

#define LCD_COMMAND_ENTRY          (0x06U)



/*
 * Data mode identifiers.
 */

#define LCD_COMMAND_MODE           (0U)

#define LCD_DATA_MODE              (1U)



/******************************************************************************
 *                         Private Variables
 ******************************************************************************/


/*
 * Current PCF8574 output state.
 *
 * Keeping this value allows modifying individual bits
 * without affecting other outputs.
 */

static uint8_t lcd_output_state = LCD_BACKLIGHT_BIT;



/*
 * Current cursor position.
 */

static uint8_t lcd_current_row = 0U;

static uint8_t lcd_current_column = 0U;



/******************************************************************************
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Write one byte to PCF8574.
 *
 * @param data
 *      Output data sent to expander.
 *
 * @return
 *      HAL status.
 */
static HAL_StatusTypeDef LCD_I2C_Write(uint8_t data)
{
    return HAL_I2C_Master_Transmit(&LCD_I2C_HANDLE,
                                   LCD_I2C_ADDRESS,
                                   &data,
                                   1U,
                                   100U);
}



/**
 * @brief
 *      Generate LCD enable pulse.
 *
 * @details
 *      HD44780 requires a high-to-low transition
 *      on EN line to latch data.
 */
static void LCD_EnablePulse(void)
{
    lcd_output_state |= LCD_EN_BIT;

    LCD_I2C_Write(lcd_output_state);

    HAL_Delay(LCD_ENABLE_PULSE_DELAY_MS);


    lcd_output_state &= ~LCD_EN_BIT;

    LCD_I2C_Write(lcd_output_state);

    HAL_Delay(LCD_ENABLE_PULSE_DELAY_MS);
}



/**
 * @brief
 *      Send 4-bit data to LCD.
 *
 * @param data
 *      Upper nibble data.
 *
 * @param mode
 *      Command or data mode.
 */
static void LCD_SendNibble(uint8_t data,
                           uint8_t mode)
{
    lcd_output_state &= 0x0FU;


    if(mode == LCD_DATA_MODE)
    {
        lcd_output_state |= LCD_RS_BIT;
    }
    else
    {
        lcd_output_state &= ~LCD_RS_BIT;
    }


    lcd_output_state |= (data & 0xF0U);


    LCD_I2C_Write(lcd_output_state);

    LCD_EnablePulse();
}
/******************************************************************************
 *                         Private Functions (continued)
 ******************************************************************************/



/**
 * @brief
 *      Send byte to LCD controller.
 *
 * @param value
 *      Data or command byte.
 *
 * @param mode
 *      Command or data mode.
 *
 * @details
 *      HD44780 works in 4-bit mode.
 *      Therefore every byte is divided into two nibbles.
 */
static void LCD_SendByte(uint8_t value,
                         uint8_t mode)
{
    LCD_SendNibble(value & 0xF0U,
                   mode);


    LCD_SendNibble((value << 4U) & 0xF0U,
                   mode);
}



/**
 * @brief
 *      Send LCD command.
 *
 * @param command
 *      LCD controller command.
 */
static void LCD_SendCommand(uint8_t command)
{
    LCD_SendByte(command,
                 LCD_COMMAND_MODE);


    HAL_Delay(LCD_COMMAND_DELAY_MS);
}



/**
 * @brief
 *      Send LCD data.
 *
 * @param data
 *      Character data.
 */
static void LCD_SendData(uint8_t data)
{
    LCD_SendByte(data,
                 LCD_DATA_MODE);
}



/**
 * @brief
 *      Initialize LCD into 4-bit mode.
 *
 * @details
 *      The HD44780 controller starts in unknown state
 *      after power-up.
 *
 *      This sequence forces the controller into 4-bit mode.
 */
static void LCD_InitializeSequence(void)
{
    HAL_Delay(50U);



    /*
     * Initialization sequence recommended by HD44780 datasheet.
     */

    LCD_SendNibble(0x30U,
                   LCD_COMMAND_MODE);

    HAL_Delay(5U);



    LCD_SendNibble(0x30U,
                   LCD_COMMAND_MODE);

    HAL_Delay(1U);



    LCD_SendNibble(0x30U,
                   LCD_COMMAND_MODE);

    HAL_Delay(1U);



    /*
     * Switch to 4-bit communication mode.
     */

    LCD_SendNibble(0x20U,
                   LCD_COMMAND_MODE);

    HAL_Delay(1U);
}



/**
 * @brief
 *      Calculate LCD DDRAM address.
 *
 * @param row
 *      LCD row.
 *
 * @param column
 *      LCD column.
 *
 * @return
 *      DDRAM address.
 *
 * @details
 *      HD44780 memory layout is not linear.
 *
 *      LCD 20x4:
 *
 *          Row 0 -> 0x00
 *          Row 1 -> 0x40
 *          Row 2 -> 0x14
 *          Row 3 -> 0x54
 *
 *
 *      LCD 16x2:
 *
 *          Row 0 -> 0x00
 *          Row 1 -> 0x40
 */
static uint8_t LCD_GetAddress(uint8_t row,
                              uint8_t column)
{
    uint8_t address = 0U;



    if(row == 0U)
    {
        address = 0x00U;
    }

    else if(row == 1U)
    {
        address = 0x40U;
    }

    else if(row == 2U)
    {
        address = 0x14U;
    }

    else if(row == 3U)
    {
        address = 0x54U;
    }

    else
    {
        address = 0x00U;
    }



    return (address + column);
}
/******************************************************************************
 *                         Public Functions
 ******************************************************************************/



/**
 * @brief
 *      Initialize LCD module.
 */
void LCD_Init(void)
{
    lcd_output_state = LCD_BACKLIGHT_BIT;


    LCD_I2C_Write(lcd_output_state);



    LCD_InitializeSequence();



    LCD_SendCommand(LCD_COMMAND_FUNCTION);

    LCD_SendCommand(LCD_COMMAND_DISPLAY);

    LCD_SendCommand(LCD_COMMAND_CLEAR);

    LCD_SendCommand(LCD_COMMAND_ENTRY);



    LCD_BacklightOn();



    lcd_current_row = 0U;

    lcd_current_column = 0U;
}



/**
 * @brief
 *      Clear LCD screen.
 */
void LCD_Clear(void)
{
    LCD_SendCommand(LCD_COMMAND_CLEAR);


    HAL_Delay(LCD_CLEAR_DELAY_MS);


    lcd_current_row = 0U;

    lcd_current_column = 0U;
}



/**
 * @brief
 *      Set LCD cursor position.
 */
void LCD_SetCursor(uint8_t row,
                   uint8_t column)
{
    uint8_t address;



    /*
     * Protect LCD memory from invalid positions.
     */

    if(row >= LCD_ROWS)
    {
        row = 0U;
    }


    if(column >= LCD_COLUMNS)
    {
        column = 0U;
    }



    address = LCD_GetAddress(row,
                             column);



    LCD_SendCommand(0x80U | address);



    lcd_current_row = row;

    lcd_current_column = column;
}



/**
 * @brief
 *      Print string on LCD.
 */
void LCD_Print(const char *text)
{
    if(text == NULL)
    {
        return;
    }



    while(*text != '\0')
    {
        LCD_PrintChar(*text);

        text++;
    }
}



/**
 * @brief
 *      Print single character.
 */
void LCD_PrintChar(char character)
{
    LCD_SendData((uint8_t)character);



    lcd_current_column++;



    if(lcd_current_column >= LCD_COLUMNS)
    {
        lcd_current_column = 0U;
    }
}



/**
 * @brief
 *      Control LCD display.
 */
void LCD_DisplayControl(LCD_DisplayState_t state)
{
    if(state == LCD_DISPLAY_ON)
    {
        LCD_SendCommand(0x0CU);
    }
    else
    {
        LCD_SendCommand(0x08U);
    }
}



/**
 * @brief
 *      Control cursor.
 */
void LCD_CursorControl(LCD_CursorMode_t mode)
{
    switch(mode)
    {
        case LCD_CURSOR_OFF:

            LCD_SendCommand(0x0CU);

            break;



        case LCD_CURSOR_ON:

            LCD_SendCommand(0x0EU);

            break;



        case LCD_CURSOR_BLINK:

            LCD_SendCommand(0x0FU);

            break;



        default:

            LCD_SendCommand(0x0CU);

            break;
    }
}



/**
 * @brief
 *      Enable LCD backlight.
 */
void LCD_BacklightOn(void)
{
    lcd_output_state |= LCD_BACKLIGHT_BIT;


    LCD_I2C_Write(lcd_output_state);
}



/**
 * @brief
 *      Disable LCD backlight.
 */
void LCD_BacklightOff(void)
{
    lcd_output_state &= ~LCD_BACKLIGHT_BIT;


    LCD_I2C_Write(lcd_output_state);
}



/**
 * @brief
 *      Print integer value.
 */
void LCD_PrintInt(int32_t value)
{
    char buffer[16];



    snprintf(buffer,
             sizeof(buffer),
             "%ld",
             value);



    LCD_Print(buffer);
}



/******************************************************************************
 *                              End Of File
 ******************************************************************************/
