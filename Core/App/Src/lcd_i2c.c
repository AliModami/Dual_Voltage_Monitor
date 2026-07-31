/******************************************************************************
 *
 * @file    lcd_i2c.c
 *
 * @brief   HD44780 Character LCD Driver over I2C PCF8574
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      3.1.0
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      Low level hardware driver for HD44780 compatible character LCD
 *      modules connected through PCF8574 I2C GPIO expander.
 *
 *
 *      Supported LCD:
 *
 *          - 16x2 Character LCD
 *          - 20x4 Character LCD
 *
 *
 *      Hardware:
 *
 *          STM32F103C8T6
 *
 *              |
 *              |
 *              | I2C1
 *              |
 *              v
 *
 *          PCF8574 I2C Backpack
 *
 *              |
 *              |
 *              | 4 bit parallel interface
 *              |
 *              v
 *
 *          HD44780 LCD Controller
 *
 *
 *------------------------------------------------------------------------------
 *
 * Responsibilities:
 *
 *      - PCF8574 communication
 *      - HD44780 initialization
 *      - LCD command transmission
 *      - LCD data transmission
 *      - Cursor positioning
 *      - Display control
 *      - Backlight control
 *
 *
 * This module does NOT handle:
 *
 *      - Menu logic
 *      - Button processing
 *      - Application screens
 *      - Voltage measurement
 *
 *
 *------------------------------------------------------------------------------
 *
 * Architecture:
 *
 *
 *      Application Layer
 *
 *              |
 *              v
 *
 *      lcd_display.c
 *
 *              |
 *              v
 *
 *      lcd_i2c.c
 *
 *              |
 *              v
 *
 *      STM32 HAL I2C
 *
 *
 ******************************************************************************/




/******************************************************************************
 *                              Includes
 ******************************************************************************/

#include "lcd_i2c.h"

#include <stdio.h>
#include <string.h>





/******************************************************************************
 *                         Private Definitions
 ******************************************************************************/


/*
 * HD44780 operating mode.
 *
 * LCD is always configured in:
 *
 *      4 bit mode
 *
 */
#define LCD_INTERFACE_4BIT              0x00U



/*
 * Character font:
 *
 *      5x8 dots
 *
 */
#define LCD_FONT_5X8                    0x00U




/*
 * HD44780 RS control.
 *
 * RS = 0:
 *
 *      Command register
 *
 *
 * RS = 1:
 *
 *      Data register
 *
 */
#define LCD_MODE_COMMAND               0U
#define LCD_MODE_DATA                  1U





/*
 * Display control flags.
 */
#define LCD_DISPLAY_ENABLE             0x04U
#define LCD_CURSOR_ENABLE              0x02U
#define LCD_CURSOR_BLINK               0x01U





/*
 * Entry mode.
 *
 * Cursor moves right after writing data.
 */
#define LCD_ENTRY_INCREMENT            0x02U





/*
 * Timing parameters.
 *
 * These values are conservative and
 * suitable for STM32F103 + PCF8574.
 */
#define LCD_POWER_DELAY_MS              50U
#define LCD_ENABLE_DELAY_US   			 1U
#define LCD_COMMAND_DELAY_MS  			 2U





/*
 * Default backlight state.
 */
#define LCD_BACKLIGHT_DEFAULT           LCD_PCF8574_BACKLIGHT_BIT


#define LCD_FUNCTION_2LINE        0x08U

#define LCD_CMD_SHIFT_LEFT        0x18U
#define LCD_CMD_SHIFT_RIGHT       0x1CU


/******************************************************************************
 *                         Private Driver Object
 ******************************************************************************/


/*
 * Single LCD instance.
 *
 * Current project uses one LCD module.
 */
static LCD_HandleTypeDef lcd;

static LCD_HandleTypeDef lcd;

static uint8_t lcd_last_expander_state = 0xFFU;


 /*
  * Cached cursor position.
  *
  * Prevents sending duplicate
  * SetCursor commands.
  */



/******************************************************************************
 *                         Private Prototypes
 ******************************************************************************/


static void LCD_ExpanderWrite(
        uint8_t data);



static void LCD_PulseEnable(
        uint8_t data);



static void LCD_Write4Bits(
        uint8_t data);



static void LCD_SendByte(
        uint8_t value,
        uint8_t mode);



static void LCD_InitializeSequence(void);



static void LCD_Configure(void);





/******************************************************************************
 *                    PCF8574 Low Level Interface
 ******************************************************************************/


/**
 * @brief
 * Write one byte to PCF8574.
 *
 * @param data
 *      PCF8574 output state.
 *
 */
static void LCD_ExpanderWrite(uint8_t data)
{
    if(lcd.hi2c == NULL)
    {
        return;
    }

    if(data == lcd_last_expander_state)
    {
        return;
    }

    lcd_last_expander_state = data;

    (void)HAL_I2C_Master_Transmit(
            lcd.hi2c,
            (uint16_t)(lcd.address << 1U),
            &data,
            1U,
            HAL_MAX_DELAY);
}





/**
 * @brief
 * Generate HD44780 Enable pulse.
 *
 */

static void LCD_PulseEnable(
        uint8_t data)
{
    LCD_ExpanderWrite(
            data | LCD_PCF8574_ENABLE_BIT);

    __NOP();
    __NOP();
    __NOP();
    __NOP();

    LCD_ExpanderWrite(
            data &
            (uint8_t)(~LCD_PCF8574_ENABLE_BIT));

    __NOP();
    __NOP();
}




/**
 * @brief
 * Send four bits to LCD.
 *
 */
static void LCD_Write4Bits(
        uint8_t data)
{

    uint8_t output;


    output =
            data |
            lcd.backlight_state;



    LCD_ExpanderWrite(output);



    LCD_PulseEnable(output);

}

/******************************************************************************
 *                         LCD Byte Transmission
 ******************************************************************************/


/**
 * @brief
 * Send complete byte to HD44780.
 *
 * @details
 *
 * HD44780 works in 4-bit mode.
 *
 * Every byte is divided into:
 *
 *      High nibble
 *
 *      Low nibble
 *
 */
static void LCD_SendByte(
        uint8_t value,
        uint8_t mode)
{

    uint8_t high_nibble;

    uint8_t low_nibble;



    /*
     * Extract high nibble.
     *
     * Example:
     *
     *      0xAB
     *
     *      High = 0xA0
     */
    high_nibble =
            value & 0xF0U;



    /*
     * Extract low nibble
     * and shift to D4-D7 position.
     */
    low_nibble =
            (uint8_t)((value << 4U) & 0xF0U);




    /*
     * Select command or data register.
     */
    if(mode == LCD_MODE_DATA)
    {

        high_nibble |=
                LCD_PCF8574_RS_BIT;


        low_nibble |=
                LCD_PCF8574_RS_BIT;

    }




    /*
     * Send high nibble first.
     */
    LCD_Write4Bits(
            high_nibble);



    /*
     * Send low nibble.
     */
    LCD_Write4Bits(
            low_nibble);

}





/******************************************************************************
 *                         Command / Data API
 ******************************************************************************/


/**
 * @brief
 * Send LCD command.
 *
 */

void LCD_SendCommand(
        uint8_t command)
{
    LCD_SendByte(
            command,
            LCD_MODE_COMMAND);

    /*
     * Only CLEAR and HOME require
     * long execution time.
     */
    if((command == LCD_CMD_CLEAR_DISPLAY) ||
       (command == LCD_CMD_RETURN_HOME))
    {
        HAL_Delay(LCD_COMMAND_DELAY_MS);
    }
}





/**
 * @brief
 * Send character data.
 *
 */
void LCD_SendData(
        uint8_t data)
{

    LCD_SendByte(
            data,
            LCD_MODE_DATA);

}







/******************************************************************************
 *                         LCD Initialization
 ******************************************************************************/


/**
 * @brief
 * Initialize LCD driver.
 *
 * @details
 *
 * Initializes:
 *
 *      - I2C interface
 *      - Driver state
 *      - HD44780 controller
 *      - Display parameters
 *
 */
HAL_StatusTypeDef LCD_Init(
        I2C_HandleTypeDef *hi2c,
        uint8_t address,
        uint8_t columns,
        uint8_t rows)
{


    /*
     * Validate I2C handle.
     */
    if(hi2c == NULL)
    {
        return HAL_ERROR;
    }




    /*
     * Validate LCD geometry.
     */
    if((columns == 0U) ||
       (rows == 0U))
    {
        return HAL_ERROR;
    }





    /*
     * Store hardware parameters.
     */
    lcd.hi2c =
            hi2c;


    lcd.address =
            address;


    lcd.columns =
            columns;


    lcd.rows =
            rows;





    /*
     * Initialize LCD state.
     */
    lcd.backlight_state =
            LCD_BACKLIGHT_DEFAULT;



    lcd.display_control =
            LCD_DISPLAY_ENABLE;



    lcd.display_mode =
            LCD_ENTRY_INCREMENT;





    /*
     * Wait for LCD power stabilization.
     */
    HAL_Delay(
            LCD_POWER_DELAY_MS);





    /*
     * Force HD44780 into 4-bit mode.
     */
    LCD_InitializeSequence();





    /*
     * Configure display.
     */
    LCD_Configure();





    return HAL_OK;

}






/******************************************************************************
 *                         HD44780 Startup Sequence
 ******************************************************************************/


/**
 * @brief
 * Force LCD controller into 4-bit mode.
 *
 * @details
 *
 * After power-up the LCD controller
 * state is undefined.
 *
 * This sequence synchronizes the controller.
 *
 */
static void LCD_InitializeSequence(void)
{

    HAL_Delay(20U);



    /*
     * Function reset sequence.
     */
    LCD_Write4Bits(
            0x30U);


    HAL_Delay(5U);



    LCD_Write4Bits(
            0x30U);


    HAL_Delay(5U);



    LCD_Write4Bits(
            0x30U);


    HAL_Delay(1U);




    /*
     * Enter 4-bit mode.
     */
    LCD_Write4Bits(
            0x20U);


    HAL_Delay(1U);

}





/******************************************************************************
 *                         Display Configuration
 ******************************************************************************/


/**
 * @brief
 * Configure HD44780 features.
 *
 */
static void LCD_Configure(void)
{


    /*
     * Function Set:
     *
     *      4 bit interface
     *      2 display lines
     *      5x8 font
     *
     */
    LCD_SendCommand(
            LCD_CMD_FUNCTION_SET |
            LCD_INTERFACE_4BIT |
            LCD_FUNCTION_2LINE |
            LCD_FONT_5X8);




    /*
     * Turn display off
     * during configuration.
     */
    LCD_SendCommand(
            LCD_CMD_DISPLAY_OFF);




    /*
     * Clear display RAM.
     */
    LCD_SendCommand(
            LCD_CMD_CLEAR_DISPLAY);




    /*
     * Configure entry mode.
     */
    lcd.display_mode =
            LCD_ENTRY_INCREMENT;



    LCD_SendCommand(
            LCD_CMD_ENTRY_MODE |
            lcd.display_mode);





    /*
     * Enable display.
     *
     * Cursor disabled.
     *
     * Blink disabled.
     */
    lcd.display_control =
            LCD_DISPLAY_ENABLE;



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);




    /*
     * Enable backlight.
     */
    LCD_BacklightOn();

}

/******************************************************************************
*                         Text Output Functions
******************************************************************************/


/**
* @brief
* Print one character.
*
* @param character
*      ASCII character.
*
*/
void LCD_PrintChar(
       char character)
{

   LCD_SendData(
           (uint8_t)character);

}







/**
* @brief
* Print string.
*
* @param text
*      Null terminated string.
*
*/
void LCD_Print(
       const char *text)
{

   if(text == NULL)
   {
       return;
   }




   while(*text != '\0')
   {

       LCD_PrintChar(
               *text);



       text++;

   }

}








/**
* @brief
* Print integer number.
*
* @param number
*      Signed integer value.
*
*/
void LCD_PrintNumber(
       int32_t number)
{

   char buffer[16];



   snprintf(
           buffer,
           sizeof(buffer),
           "%ld",
           number);



   LCD_Print(
           buffer);

}







/******************************************************************************
*                         Floating Point Output
******************************************************************************/


/**
* @brief
* Print floating point number.
*
* @details
*
* Used for measurement values:
*
*      Vin
*
*      Vout
*
*
* Example:
*
*      LCD_PrintFloat(220.5f,1);
*
*/
void LCD_PrintFloat(
       float number,
       uint8_t decimals)
{

   char buffer[24];



   snprintf(
           buffer,
           sizeof(buffer),
           "%.*f",
           decimals,
           number);



   LCD_Print(
           buffer);

}








/******************************************************************************
*                         Cursor Management
******************************************************************************/


/**
* @brief
* Set cursor position.
*
* @details
*
* HD44780 DDRAM mapping is not linear.
*
* Supported:
*
*      16x2
*
*      20x4
*
*/
void LCD_SetCursor(
       uint8_t column,
       uint8_t row)
{

   static const uint8_t row_address[] =
   {
       0x00U,
       0x40U,
       0x14U,
       0x54U
   };




   /*
    * Check row range.
    */
   if(row >= lcd.rows)
   {

       row =
           lcd.rows - 1U;

   }





   /*
    * Check column range.
    */
   if(column >= lcd.columns)
   {

       column =
           lcd.columns - 1U;

   }





   LCD_SendCommand(
           LCD_CMD_SET_DDRAM |
           (row_address[row] + column));

}








/**
* @brief
* Return cursor to home.
*
*/
void LCD_Home(void)
{

   LCD_SendCommand(
           LCD_CMD_RETURN_HOME);



   HAL_Delay(
           2U);

}








/******************************************************************************
*                         Display Clearing
******************************************************************************/


/**
* @brief
* Clear complete display.
*
*/
void LCD_Clear(void)
{
    LCD_SendCommand(
            LCD_CMD_CLEAR_DISPLAY);
}








/**
* @brief
* Clear selected LCD row.
*
* @param row
*      Row index.
*
*/
void LCD_ClearRow(
       uint8_t row)
{

   uint8_t index;



   LCD_SetCursor(
           0U,
           row);




   for(index = 0U;
       index < lcd.columns;
       index++)
   {

       LCD_PrintChar(
               ' ');

   }




   LCD_SetCursor(
           0U,
           row);

}








/******************************************************************************
*                         Display Control
******************************************************************************/


/**
* @brief
* Enable LCD display.
*
*/
void LCD_DisplayOn(void)
{

   lcd.display_control |=
           LCD_DISPLAY_ENABLE;



   LCD_SendCommand(
           LCD_CMD_DISPLAY_ON |
           lcd.display_control);

}








/**
* @brief
* Disable LCD display.
*
*/
void LCD_DisplayOff(void)
{

   lcd.display_control &=
           (uint8_t)(~LCD_DISPLAY_ENABLE);



   LCD_SendCommand(
           LCD_CMD_DISPLAY_OFF |
           lcd.display_control);

}








/**
* @brief
* Enable cursor.
*
*/
void LCD_CursorOn(void)
{

   lcd.display_control |=
           LCD_CURSOR_ENABLE;



   LCD_SendCommand(
           LCD_CMD_DISPLAY_ON |
           lcd.display_control);

}








/**
* @brief
* Disable cursor.
*
*/
void LCD_CursorOff(void)
{

   lcd.display_control &=
           (uint8_t)(~LCD_CURSOR_ENABLE);



   LCD_SendCommand(
           LCD_CMD_DISPLAY_ON |
           lcd.display_control);

}


/******************************************************************************
 *                         Cursor Blink Control
 ******************************************************************************/


/**
 * @brief
 * Enable cursor blinking.
 *
 */
void LCD_BlinkOn(void)
{

    lcd.display_control |=
            LCD_CURSOR_BLINK;



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);

}








/**
 * @brief
 * Disable cursor blinking.
 *
 */
void LCD_BlinkOff(void)
{

    lcd.display_control &=
            (uint8_t)(~LCD_CURSOR_BLINK);



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);

}








/******************************************************************************
 *                         Backlight Control
 ******************************************************************************/


/**
 * @brief
 * Enable LCD backlight.
 *
 * @details
 *
 * PCF8574 P3 controls the LED backlight.
 *
 */
void LCD_BacklightOn(void)
{

    lcd.backlight_state =
            LCD_PCF8574_BACKLIGHT_BIT;



    LCD_ExpanderWrite(
            lcd.backlight_state);

}








/**
 * @brief
 * Disable LCD backlight.
 *
 */
void LCD_BacklightOff(void)
{

    lcd.backlight_state =
            0U;



    LCD_ExpanderWrite(
            lcd.backlight_state);

}








/******************************************************************************
 *                         Custom Characters
 ******************************************************************************/


/**
 * @brief
 * Create HD44780 custom character.
 *
 * @param location
 *      CGRAM location 0...7
 *
 * @param pattern
 *      Eight byte bitmap.
 *
 */
void LCD_CreateChar(
        uint8_t location,
        uint8_t pattern[])
{

    uint8_t index;



    /*
     * HD44780 supports
     * only eight characters.
     */
    location &=
            0x07U;





    /*
     * Select CGRAM address.
     */
    LCD_SendCommand(
            LCD_CMD_SET_CGRAM |
            (location << 3U));





    /*
     * Write bitmap.
     */
    for(index = 0U;
        index < 8U;
        index++)
    {

        LCD_SendData(
                pattern[index]);

    }

}








/**
 * @brief
 * Print custom character.
 *
 * @param location
 *      Character index 0...7
 *
 */
void LCD_PrintCustomChar(
        uint8_t location)
{

    LCD_SendData(
            location & 0x07U);

}








/******************************************************************************
 *                         Display Shift
 ******************************************************************************/


/**
 * @brief
 * Shift display left.
 *
 */
void LCD_ScrollLeft(void)
{

    LCD_SendCommand(
            LCD_CMD_SHIFT_LEFT);

}








/**
 * @brief
 * Shift display right.
 *
 */
void LCD_ScrollRight(void)
{

    LCD_SendCommand(
            LCD_CMD_SHIFT_RIGHT);

}








/******************************************************************************
 *                         LCD Information
 ******************************************************************************/


/**
 * @brief
 * Get configured LCD columns.
 *
 */
uint8_t LCD_GetColumns(void)
{

    return lcd.columns;

}








/**
 * @brief
 * Get configured LCD rows.
 *
 */
uint8_t LCD_GetRows(void)
{

    return lcd.rows;

}








/**
 * @brief
 * Get PCF8574 address.
 *
 */
uint8_t LCD_GetAddress(void)
{

    return lcd.address;

}


/******************************************************************************
 *                         Driver State Management
 ******************************************************************************/


/**
 * @brief
 * Reset internal LCD driver state.
 *
 * @details
 *
 * Clears the internal driver context.
 *
 * Useful when:
 *
 *      - LCD module is disconnected.
 *      - I2C recovery is required.
 *      - Driver restart is needed.
 *
 */
void LCD_ResetState(void)
{

    memset(
            &lcd,
            0,
            sizeof(lcd));



    /*
     * Restore default backlight state.
     */
    lcd.backlight_state =
            LCD_BACKLIGHT_DEFAULT;

}








/**
 * @brief
 * Check LCD initialization status.
 *
 * @return
 *
 *      1:
 *          LCD initialized.
 *
 *      0:
 *          LCD not initialized.
 *
 */
uint8_t LCD_IsReady(void)
{

    if(lcd.hi2c != NULL)
    {

        return 1U;

    }



    return 0U;

}








/******************************************************************************
 *                         Internal Configuration Notes
 ******************************************************************************/


/*
 *
 * LCD Geometry Handling
 * =====================
 *
 *
 * This driver does not contain fixed LCD size.
 *
 *
 * Supported:
 *
 *
 *      20x4 LCD
 *
 *          columns = 20
 *          rows    = 4
 *
 *
 *      16x2 LCD
 *
 *          columns = 16
 *          rows    = 2
 *
 *
 *
 * The application selects the LCD size
 * during initialization.
 *
 *
 * Example:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          0x27,
 *          20,
 *          4);
 *
 *
 *
 * No modification inside this driver
 * is required when changing LCD size.
 *
 */







/******************************************************************************
 *                         PCF8574 Mapping Reference
 ******************************************************************************/


/*
 *
 * PCF8574 Pin Mapping:
 *
 *
 *
 *      PCF8574              HD44780
 *
 *
 *      P0  ----------------  RS
 *
 *
 *      P1  ----------------  RW
 *
 *
 *      P2  ----------------  EN
 *
 *
 *      P3  ----------------  Backlight
 *
 *
 *      P4  ----------------  D4
 *
 *
 *      P5  ----------------  D5
 *
 *
 *      P6  ----------------  D6
 *
 *
 *      P7  ----------------  D7
 *
 *
 *
 *
 * RW is not used.
 *
 * Driver operates in:
 *
 *
 *      Write Only Mode
 *
 *
 */







/******************************************************************************
 *                         I2C Address Notes
 ******************************************************************************/


/*
 *
 * PCF8574:
 *
 *      Address range:
 *
 *          0x20 - 0x27
 *
 *
 *
 * PCF8574A:
 *
 *      Address range:
 *
 *          0x38 - 0x3F
 *
 *
 *
 *
 * LCD_Init() expects:
 *
 *      7-bit address
 *
 *
 * Example:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          0x27,
 *          20,
 *          4);
 *
 *
 *
 *
 * STM32 HAL internally uses:
 *
 *
 *      0x27 << 1
 *
 *
 *      = 0x4E
 *
 *
 *
 * Do not pass 0x4E.
 *
 */







/******************************************************************************
 *                         Refresh Strategy
 ******************************************************************************/


/*
 *
 * Character LCD is slow compared
 * with STM32 execution speed.
 *
 *
 * Avoid:
 *
 *
 *      LCD_Clear()
 *
 *
 * inside fast loops.
 *
 *
 *
 * Recommended:
 *
 *
 *      LCD_SetCursor()
 *
 *      LCD_Print()
 *
 *      LCD_ClearRow()
 *
 *
 *
 * Advantages:
 *
 *
 *      - Lower I2C traffic
 *
 *      - Less flicker
 *
 *      - Faster UI response
 *
 *
 */







/******************************************************************************
 *                         Error Handling Notes
 ******************************************************************************/


/*
 *
 * Current implementation:
 *
 *
 *      HAL_I2C_Master_Transmit()
 *
 *
 * uses:
 *
 *
 *      HAL_MAX_DELAY
 *
 *
 *
 * This is acceptable for:
 *
 *
 *      - Monitoring systems
 *
 *      - User interface LCD
 *
 *
 *
 * Future improvements:
 *
 *
 *      - Timeout handling
 *
 *      - I2C recovery
 *
 *      - Non-blocking transfer
 *
 *      - DMA support
 *
 *
 */

/******************************************************************************
 *                         Compatibility Verification
 ******************************************************************************/


/*
 *
 * lcd_i2c.c Public API
 * ====================
 *
 *
 * Initialization:
 *
 *      LCD_Init()
 *
 *
 *
 * Basic Output:
 *
 *      LCD_Print()
 *
 *      LCD_PrintChar()
 *
 *      LCD_PrintNumber()
 *
 *      LCD_PrintFloat()
 *
 *
 *
 * Cursor Control:
 *
 *      LCD_SetCursor()
 *
 *      LCD_Home()
 *
 *
 *
 * Display Control:
 *
 *      LCD_Clear()
 *
 *      LCD_ClearRow()
 *
 *
 *
 * Display State:
 *
 *      LCD_DisplayOn()
 *
 *      LCD_DisplayOff()
 *
 *      LCD_CursorOn()
 *
 *      LCD_CursorOff()
 *
 *      LCD_BlinkOn()
 *
 *      LCD_BlinkOff()
 *
 *
 *
 * Backlight:
 *
 *      LCD_BacklightOn()
 *
 *      LCD_BacklightOff()
 *
 *
 *
 * Custom Characters:
 *
 *      LCD_CreateChar()
 *
 *      LCD_PrintCustomChar()
 *
 *
 *
 * Display Shift:
 *
 *      LCD_ScrollLeft()
 *
 *      LCD_ScrollRight()
 *
 *
 *
 * Information:
 *
 *      LCD_GetColumns()
 *
 *      LCD_GetRows()
 *
 *      LCD_GetAddress()
 *
 *
 *
 * Driver State:
 *
 *      LCD_ResetState()
 *
 *      LCD_IsReady()
 *
 *
 */








/******************************************************************************
 *                         Architecture Verification
 ******************************************************************************/


/*
 *
 * Module Dependency:
 *
 *
 *
 *      lcd_display.c
 *
 *              |
 *              v
 *
 *          lcd_i2c.c
 *
 *              |
 *              v
 *
 *          STM32 HAL I2C
 *
 *
 *
 *
 * This module does NOT depend on:
 *
 *
 *      - menu_controller
 *
 *      - menu_renderer
 *
 *      - menu_items
 *
 *      - ADC modules
 *
 *      - Alarm modules
 *
 *      - Application logic
 *
 *
 */







/******************************************************************************
 *                         Design Verification Checklist
 ******************************************************************************/


/*
 *
 * Verification:
 *
 *
 * [OK] HD44780 4-bit communication
 *
 *
 * [OK] PCF8574 I2C interface
 *
 *
 * [OK] 16x2 LCD support
 *
 *
 * [OK] 20x4 LCD support
 *
 *
 * [OK] Dynamic row/column configuration
 *
 *
 * [OK] DDRAM row addressing
 *
 *
 * [OK] Cursor positioning
 *
 *
 * [OK] Backlight control
 *
 *
 * [OK] Custom characters
 *
 *
 * [OK] Integer output
 *
 *
 * [OK] Float output
 *
 *
 * [OK] No menu dependency
 *
 *
 * [OK] No application dependency
 *
 *
 * [OK] HAL compatible
 *
 *
 */







/******************************************************************************
 *                         Final Notes
 ******************************************************************************/


/*
 *
 * lcd_i2c.c v3.1.0
 *
 *
 * Final role:
 *
 *
 *      Low Level LCD Hardware Driver
 *
 *
 *
 * Responsibility:
 *
 *
 *      Convert software commands into
 *      HD44780 signals through PCF8574.
 *
 *
 *
 * It is intentionally independent from:
 *
 *
 *      User Interface
 *
 *      Menu System
 *
 *      Measurement System
 *
 *
 *
 * The upper software layers should never
 * access PCF8574 or HD44780 directly.
 *
 *
 */







/******************************************************************************
 *
 *                              END OF FILE
 *
 *
 *      lcd_i2c.c
 *
 *
 *      STM32F103C8T6
 *
 *              +
 *
 *      PCF8574 I2C Backpack
 *
 *              +
 *
 *      HD44780 Character LCD
 *
 *
 ******************************************************************************/
