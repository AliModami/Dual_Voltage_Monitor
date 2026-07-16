/******************************************************************************
 * @file           lcd_i2c.c
 * @brief          HD44780 Character LCD Driver over I2C PCF8574
 *
 * @details
 * Low level hardware driver for HD44780 based character LCD modules
 * connected through a PCF8574 I2C GPIO expander.
 *
 * Supported LCD modules:
 *
 *      - 16x2 Character LCD
 *      - 20x4 Character LCD
 *
 *
 * Hardware Interface:
 *
 *      STM32F103C8T6
 *
 *              |
 *              |
 *              | I2C1
 *              |
 *              v
 *
 *          PCF8574 / PCF8574A
 *
 *              |
 *              |
 *              | 4-bit parallel interface
 *              |
 *              v
 *
 *          HD44780 LCD Controller
 *
 *
 *
 * Software Architecture:
 *
 *
 *      Application Layer
 *
 *          menu.c
 *          monitor.c
 *          stream.c
 *
 *                 |
 *                 v
 *
 *          lcd_display.c
 *
 *                 |
 *                 v
 *
 *          lcd_i2c.c
 *
 *                 |
 *                 v
 *
 *          STM32 HAL I2C Driver
 *
 *
 *
 * This module is responsible only for LCD hardware control.
 *
 * It does not contain:
 *
 *      - Menu management
 *      - Voltage measurement logic
 *      - Alarm processing
 *      - Application screen design
 *
 *
 *
 * LCD Configuration:
 *
 * The LCD type is NOT selected inside this source file.
 *
 * The driver receives the LCD geometry during initialization:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          LCD_DEFAULT_COLUMNS,
 *          LCD_DEFAULT_ROWS);
 *
 *
 * To change LCD module:
 *
 *      Example 1:
 *
 *          20x4 LCD
 *
 *              columns = 20
 *              rows    = 4
 *
 *
 *      Example 2:
 *
 *          16x2 LCD
 *
 *              columns = 16
 *              rows    = 2
 *
 *
 * Only the initialization parameters should be changed.
 *
 * The driver source code does not require modification.
 *
 *
 *
 * I2C Address Configuration:
 *
 * PCF8574:
 *
 *      Typical range:
 *
 *          0x20 ... 0x27
 *
 *
 * PCF8574A:
 *
 *      Typical range:
 *
 *          0x38 ... 0x3F
 *
 *
 * The address passed to LCD_Init() is the 7-bit address.
 *
 * Example:
 *
 *      PCF8574 address = 0x27
 *
 *
 * STM32 HAL internally uses:
 *
 *      0x27 << 1 = 0x4E
 *
 *
 *
 * @version     3.0.0
 * @date        2026
 *
 * @author
 *      Dual Voltage Monitor Project
 *
 *
 * @history
 *
 * Version 1.0.0
 *      Initial LCD I2C driver implementation.
 *
 * Version 2.0.0
 *      Improved documentation and API structure.
 *
 * Version 3.0.0
 *      Reorganized implementation.
 *      Improved portability for different LCD sizes.
 *
 ******************************************************************************/



/* --------------------------------------------------------------------------
 * Includes
 * -------------------------------------------------------------------------- */


#include "lcd_i2c.h"


#include <stdio.h>
#include <string.h>






/*
 * Function set parameters.
 *
 * HD44780 Function Set command bits:
 *
 *      DL  : Interface length
 *
 *          0 = 4-bit interface
 *
 *
 *      N   : Display lines
 *
 *          0 = 1 line
 *
 *          1 = 2 lines
 *
 *
 *      F   : Character font
 *
 *          0 = 5x8 dots
 *
 */
#define LCD_FUNCTION_4BIT                0x00U
#define LCD_FUNCTION_2LINE               0x08U
#define LCD_FUNCTION_FONT_5X8            0x00U





/* --------------------------------------------------------------------------
 * Private Timing Constants
 * -------------------------------------------------------------------------- */


/**
 * @brief
 * HD44780 enable pulse duration.
 *
 * The Enable signal must remain active long enough
 * for the LCD controller to latch the data.
 */
#define LCD_ENABLE_DELAY_MS              1U



/**
 * @brief
 * General command execution delay.
 */
#define LCD_COMMAND_DELAY_MS             2U



/**
 * @brief
 * LCD power stabilization delay.
 *
 * After power application the LCD controller
 * requires time before accepting commands.
 */
#define LCD_POWER_ON_DELAY_MS            50U



/**
 * @brief
 * Default backlight state.
 *
 * Backlight is enabled after initialization.
 */
#define LCD_BACKLIGHT_DEFAULT            LCD_PCF8574_BACKLIGHT_BIT





/* --------------------------------------------------------------------------
 * Private HD44780 Control Definitions
 * -------------------------------------------------------------------------- */


/**
 * @brief
 * LCD command mode.
 *
 * RS = 0
 */
#define LCD_MODE_COMMAND                 0U



/**
 * @brief
 * LCD data mode.
 *
 * RS = 1
 */
#define LCD_MODE_DATA                    1U




/**
 * @brief
 * Display control bits.
 *
 * HD44780 register:
 *
 *      DB2 : Display enable
 *      DB1 : Cursor enable
 *      DB0 : Cursor blink
 */
#define LCD_DISPLAY_ON_BIT               0x04U
#define LCD_CURSOR_ON_BIT                0x02U
#define LCD_BLINK_ON_BIT                 0x01U




/**
 * @brief
 * Entry mode increment bit.
 *
 * Cursor automatically moves right
 * after writing a character.
 */
#define LCD_ENTRY_INCREMENT              0x02U




/**
 * @brief
 * Function set configuration.
 *
 * HD44780 is operated in:
 *
 *      4-bit mode
 *      5x8 font
 *
 *
 * The number of display rows is supplied
 * dynamically during LCD_Init().
 *
 * The same driver supports:
 *
 *      16x2
 *      20x4
 */
#define LCD_FUNCTION_4BIT                0x00U
#define LCD_FUNCTION_FONT_5X8            0x00U




/**
 * @brief
 * Display shift commands.
 */
#define LCD_CMD_SHIFT_LEFT               0x18U
#define LCD_CMD_SHIFT_RIGHT              0x1CU





/* --------------------------------------------------------------------------
 * Private Driver Instance
 * -------------------------------------------------------------------------- */


/**
 * @brief
 * Internal LCD driver context.
 *
 * The current implementation supports one LCD module.
 */
static LCD_HandleTypeDef lcd;





/* --------------------------------------------------------------------------
 * Private Function Prototypes
 * -------------------------------------------------------------------------- */


static void LCD_ExpanderWrite(uint8_t data);

static void LCD_PulseEnable(uint8_t data);

static void LCD_Write4Bits(uint8_t data);

static void LCD_Send(
        uint8_t value,
        uint8_t mode);

static void LCD_InitializeSequence(void);

static void LCD_ConfigureDisplay(void);




/* ==========================================================================
 * Low Level I2C Communication
 * ========================================================================== */


/**
 * @brief
 * Write one byte to PCF8574 I/O expander.
 *
 * @details
 *
 * This function is the lowest hardware communication layer.
 *
 * All LCD data transfer finally reaches this function.
 *
 *
 * Communication path:
 *
 *
 *      LCD_Send()
 *
 *          |
 *          v
 *
 *      LCD_Write4Bits()
 *
 *          |
 *          v
 *
 *      LCD_ExpanderWrite()
 *
 *          |
 *          v
 *
 *      HAL_I2C_Master_Transmit()
 *
 *
 *
 * @param data
 *      Current output state of PCF8574.
 *
 */
static void LCD_ExpanderWrite(uint8_t data)
{


    /*
     * PCF8574 uses a 7-bit I2C address.
     *
     * STM32 HAL requires the address
     * in shifted format:
     *
     *      HAL Address = 7-bit Address << 1
     *
     *
     * Example:
     *
     *      LCD backpack address:
     *
     *          0x27
     *
     *
     *      HAL transmitted address:
     *
     *          0x4E
     *
     */
    HAL_I2C_Master_Transmit(
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
 * @details
 *
 * HD44780 captures data on the falling edge
 * of the Enable signal.
 *
 *
 * Pulse sequence:
 *
 *
 *      EN = HIGH
 *
 *          |
 *          | delay
 *
 *          v
 *
 *      EN = LOW
 *
 *
 *
 * @param data
 *      Current PCF8574 output state.
 *
 */
static void LCD_PulseEnable(uint8_t data)
{


    /*
     * Set Enable line HIGH.
     */
    LCD_ExpanderWrite(
            data | LCD_PCF8574_ENABLE_BIT);



    HAL_Delay(
            LCD_ENABLE_DELAY_MS);



    /*
     * Set Enable line LOW.
     *
     * Data is latched by LCD controller
     * at this moment.
     */
    LCD_ExpanderWrite(
            data &
            (uint8_t)(~LCD_PCF8574_ENABLE_BIT));



    HAL_Delay(
            LCD_ENABLE_DELAY_MS);


}





/**
 * @brief
 * Send four bits to LCD data bus.
 *
 * @details
 *
 * HD44780 operates in 4-bit mode.
 *
 * Only data lines:
 *
 *      D4
 *      D5
 *      D6
 *      D7
 *
 * are used.
 *
 *
 * PCF8574 mapping:
 *
 *
 *      P4 -> LCD D4
 *
 *      P5 -> LCD D5
 *
 *      P6 -> LCD D6
 *
 *      P7 -> LCD D7
 *
 *
 *
 * @param data
 *      Upper nibble aligned to bits P4-P7.
 *
 */
static void LCD_Write4Bits(uint8_t data)
{


    uint8_t output;



    /*
     * Preserve current backlight state.
     */
    output =
            data |
            lcd.backlight_state;



    /*
     * Write data to PCF8574.
     */
    LCD_ExpanderWrite(output);



    /*
     * Generate LCD latch pulse.
     */
    LCD_PulseEnable(output);


}





/**
 * @brief
 * Send complete byte to HD44780 controller.
 *
 * @details
 *
 * The LCD is configured in 4-bit mode.
 *
 * Therefore each byte is divided into:
 *
 *
 *      High nibble
 *
 *      Low nibble
 *
 *
 * Example:
 *
 *      Data byte:
 *
 *          0x41
 *
 *
 * Binary:
 *
 *          0100 0001
 *
 *
 * Transmission:
 *
 *          0100
 *
 *          0001
 *
 *
 *
 * @param value
 *      Byte to transmit.
 *
 * @param mode
 *
 *      LCD_MODE_COMMAND:
 *
 *          Send command.
 *
 *
 *      LCD_MODE_DATA:
 *
 *          Send character data.
 *
 */
static void LCD_Send(
        uint8_t value,
        uint8_t mode)
{


    uint8_t high;

    uint8_t low;



    /*
     * Extract high nibble.
     *
     * Example:
     *
     *      value = 0xAB
     *
     *      high = 0xA0
     */
    high =
            value & 0xF0U;



    /*
     * Extract low nibble
     * and move it to upper position.
     *
     * Example:
     *
     *      value = 0xAB
     *
     *      low = 0xB0
     */
    low =
            (uint8_t)((value << 4U) & 0xF0U);



    /*
     * Select LCD register.
     *
     *
     * RS = 0
     *
     *      Command register
     *
     *
     * RS = 1
     *
     *      Data register
     */
    if(mode == LCD_MODE_DATA)
    {

        high |= LCD_PCF8574_RS_BIT;

        low  |= LCD_PCF8574_RS_BIT;

    }



    /*
     * Send high nibble first.
     */
    LCD_Write4Bits(high);



    /*
     * Send low nibble second.
     */
    LCD_Write4Bits(low);


}






/* ==========================================================================
 * Public Command / Data Interface
 * ========================================================================== */


/**
 * @brief
 * Send HD44780 command.
 *
 * @param command
 *      LCD controller command byte.
 *
 */
void LCD_SendCommand(uint8_t command)
{


    LCD_Send(
            command,
            LCD_MODE_COMMAND);



    /*
     * Clear display and return-home commands
     * require longer execution time.
     */
    if((command == LCD_CMD_CLEAR_DISPLAY) ||
       (command == LCD_CMD_RETURN_HOME))
    {

        HAL_Delay(
                LCD_COMMAND_DELAY_MS);

    }


}





/**
 * @brief
 * Send one character data byte.
 *
 * @param data
 *      ASCII character.
 *
 */
void LCD_SendData(uint8_t data)
{


    LCD_Send(
            data,
            LCD_MODE_DATA);


}






/* ==========================================================================
 * LCD Initialization
 * ========================================================================== */


/**
 * @brief
 * Initialize LCD hardware driver.
 *
 * @details
 *
 * This function initializes:
 *
 *      - I2C communication parameters
 *      - Internal driver state
 *      - HD44780 startup sequence
 *      - Display configuration
 *
 *
 * LCD geometry is configurable.
 *
 * The same driver supports:
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
 * No source code modification is required
 * when changing LCD size.
 *
 * Only the initialization parameters
 * must be changed in the application layer.
 *
 *
 * Example:
 *
 * For 20x4 LCD:
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          20,
 *          4);
 *
 *
 *
 * For 16x2 LCD:
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          16,
 *          2);
 *
 *
 *
 * @param hi2c
 *      Pointer to STM32 HAL I2C handle.
 *
 * @param address
 *      7-bit PCF8574 address.
 *
 *
 *      Common values:
 *
 *          PCF8574:
 *
 *              0x20 - 0x27
 *
 *
 *          PCF8574A:
 *
 *              0x38 - 0x3F
 *
 *
 *
 * @param columns
 *      LCD number of columns.
 *
 *      Supported:
 *
 *          16
 *
 *          20
 *
 *
 *
 * @param rows
 *      LCD number of rows.
 *
 *      Supported:
 *
 *          2
 *
 *          4
 *
 *
 *
 * @return
 *
 *      HAL_OK:
 *
 *          Initialization successful.
 *
 *
 *      HAL_ERROR:
 *
 *          Invalid parameter.
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
     * Store hardware configuration.
     *
     * These values define the connected LCD.
     *
     * The driver does not assume a fixed LCD size.
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
     * Initialize default hardware states.
     */
    lcd.backlight_state =
            LCD_BACKLIGHT_DEFAULT;



    /*
     * Initial display state:
     *
     *      Display ON
     *
     *      Cursor OFF
     *
     *      Blink OFF
     */
    lcd.display_control =
            LCD_DISPLAY_ON_BIT;



    /*
     * Entry mode:
     *
     *      Cursor moves right
     *
     *      Display does not shift
     */
    lcd.display_mode =
            LCD_ENTRY_INCREMENT;



    /*
     * Allow LCD power supply
     * to become stable.
     */
    HAL_Delay(
            LCD_POWER_ON_DELAY_MS);



    /*
     * Execute HD44780 power-up sequence.
     */
    LCD_InitializeSequence();



    /*
     * Configure LCD operating mode.
     */
    LCD_ConfigureDisplay();



    return HAL_OK;

}







/**
 * @brief
 * Execute HD44780 initialization sequence.
 *
 * @details
 *
 * After power-up, HD44780 controller state
 * is undefined.
 *
 * The recommended startup sequence forces
 * the controller into 4-bit mode.
 *
 *
 * Sequence:
 *
 *
 *      0x03
 *
 *      0x03
 *
 *      0x03
 *
 *      0x02
 *
 *
 * This procedure is required because
 * the LCD may start in either:
 *
 *      8-bit mode
 *
 *      4-bit mode
 *
 *
 */
static void LCD_InitializeSequence(void)
{


    /*
     * Initial stabilization delay.
     */
    HAL_Delay(20U);




    /*
     * First synchronization command.
     */
    LCD_Write4Bits(0x30U);

    HAL_Delay(5U);




    /*
     * Second synchronization command.
     */
    LCD_Write4Bits(0x30U);

    HAL_Delay(5U);




    /*
     * Third synchronization command.
     */
    LCD_Write4Bits(0x30U);

    HAL_Delay(1U);




    /*
     * Switch controller to 4-bit mode.
     */
    LCD_Write4Bits(0x20U);

    HAL_Delay(1U);

}







/**
 * @brief
 * Configure LCD controller features.
 *
 * @details
 *
 * Configures:
 *
 *      - Interface mode
 *      - Display state
 *      - Cursor behavior
 *      - Entry mode
 *
 */
static void LCD_ConfigureDisplay(void)
{


    /*
     * Function Set command.
     *
     * Configuration:
     *
     *      4-bit interface
     *
     *      2 display lines
     *
     *      5x8 font
     *
     *
     * Note:
     *
     * HD44780 uses the same
     * function set command for
     * both 16x2 and 20x4 displays.
     *
     * The number of rows is handled
     * by DDRAM addressing.
     */
    LCD_SendCommand(
            LCD_CMD_FUNCTION_SET |
            LCD_FUNCTION_4BIT |
            LCD_FUNCTION_2LINE |
            LCD_FUNCTION_FONT_5X8);




    /*
     * Disable display during setup.
     */
    LCD_SendCommand(
            LCD_CMD_DISPLAY_OFF);




    /*
     * Clear display memory.
     */
    LCD_SendCommand(
            LCD_CMD_CLEAR_DISPLAY);




    /*
     * Configure cursor movement.
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
            LCD_DISPLAY_ON_BIT;



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON);




    /*
     * Enable LCD backlight.
     */
    LCD_BacklightOn();

}





/* ==========================================================================
 * Text Output Functions
 * ========================================================================== */


/**
 * @brief
 * Print one character.
 *
 * @param character
 *      ASCII character.
 *
 */
void LCD_PrintChar(char character)
{

    LCD_SendData(
            (uint8_t)character);

}






/**
 * @brief
 * Print null terminated string.
 *
 * @param text
 *      String pointer.
 *
 */
void LCD_Print(const char *text)
{


    if(text == NULL)
    {
        return;
    }




    while(*text)
    {

        LCD_PrintChar(*text);

        text++;

    }

}






/**
 * @brief
 * Print signed integer value.
 *
 * @param number
 *      Integer number.
 *
 */
void LCD_PrintNumber(int32_t number)
{


    char buffer[16];



    snprintf(
            buffer,
            sizeof(buffer),
            "%ld",
            number);



    LCD_Print(buffer);

}







/* ==========================================================================
 * Floating Point Output
 * ========================================================================== */


/**
 * @brief
 * Print floating point value.
 *
 * @details
 *
 * Used for measured values such as:
 *
 *      Vin  = 220.5V
 *
 *      Vout = 219.8V
 *
 *
 * @param number
 *      Floating point value.
 *
 * @param decimals
 *      Number of digits after decimal point.
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



    LCD_Print(buffer);

}







/* ==========================================================================
 * Cursor Position Control
 * ========================================================================== */


/**
 * @brief
 * Set LCD cursor position.
 *
 * @details
 *
 * HD44780 controllers do not store LCD rows
 * sequentially in DDRAM memory.
 *
 *
 * Address mapping:
 *
 *
 * 20x4 LCD:
 *
 *      Row 0 -> 0x00
 *
 *      Row 1 -> 0x40
 *
 *      Row 2 -> 0x14
 *
 *      Row 3 -> 0x54
 *
 *
 *
 * 16x2 LCD:
 *
 *      Row 0 -> 0x00
 *
 *      Row 1 -> 0x40
 *
 *
 *
 * The driver automatically uses the
 * required mapping.
 *
 * Application code does not need
 * to change when switching between:
 *
 *
 *      20x4 LCD
 *
 *      16x2 LCD
 *
 *
 *
 * Only LCD_Init() parameters
 * must be changed:
 *
 *
 *      columns
 *
 *      rows
 *
 *
 *
 * @param column
 *      Column index starting from zero.
 *
 * @param row
 *      Row index starting from zero.
 *
 */
void LCD_SetCursor(
        uint8_t column,
        uint8_t row)
{


    /*
     * HD44780 DDRAM address table.
     *
     * Supported displays:
     *
     *      16x2
     *
     *      20x4
     *
     *
     * Row selection is controlled
     * by lcd.rows value.
     */
    static const uint8_t row_address[] =
    {
        0x00U,
        0x40U,
        0x14U,
        0x54U
    };




    /*
     * Protect against invalid row.
     */
    if(row >= lcd.rows)
    {

        row =
            lcd.rows - 1U;

    }





    /*
     * Protect against invalid column.
     */
    if(column >= lcd.columns)
    {

        column =
            lcd.columns - 1U;

    }





    /*
     * Set DDRAM address.
     */
    LCD_SendCommand(
            LCD_CMD_SET_DDRAM |
            (row_address[row] + column));

}







/**
 * @brief
 * Return cursor to home position.
 *
 * @details
 *
 * Cursor moves to DDRAM address 0.
 *
 * Display content remains unchanged.
 *
 */
void LCD_Home(void)
{

    LCD_SendCommand(
            LCD_CMD_RETURN_HOME);



    HAL_Delay(2U);

}







/* ==========================================================================
 * Display Clear Functions
 * ========================================================================== */


/**
 * @brief
 * Clear complete LCD display.
 *
 * @details
 *
 * Clears DDRAM memory and moves
 * cursor to home position.
 *
 *
 * @note
 *
 * The clear command requires
 * longer execution time.
 *
 * Avoid using it continuously
 * in fast update loops.
 *
 */
void LCD_Clear(void)
{

    LCD_SendCommand(
            LCD_CMD_CLEAR_DISPLAY);



    HAL_Delay(2U);

}







/**
 * @brief
 * Clear one LCD row.
 *
 * @details
 *
 * Character LCD controllers do not
 * provide a direct row erase command.
 *
 * This function overwrites the selected
 * row with spaces.
 *
 *
 * It is useful for dynamic screens:
 *
 *      Monitor display
 *
 *      Streaming display
 *
 *      Status messages
 *
 *
 * @param row
 *      Row index.
 *
 */
void LCD_ClearRow(uint8_t row)
{

    uint8_t i;



    LCD_SetCursor(
            0U,
            row);




    for(i = 0U; i < lcd.columns; i++)
    {

        LCD_PrintChar(' ');

    }




    LCD_SetCursor(
            0U,
            row);

}








/* ==========================================================================
 * Display Control
 * ========================================================================== */


/**
 * @brief
 * Turn display ON.
 *
 */
void LCD_DisplayOn(void)
{

    lcd.display_control |=
            LCD_DISPLAY_ON_BIT;



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);

}







/**
 * @brief
 * Turn display OFF.
 *
 */
void LCD_DisplayOff(void)
{

    lcd.display_control &=
            (uint8_t)(~LCD_DISPLAY_ON_BIT);



    LCD_SendCommand(
            LCD_CMD_DISPLAY_OFF |
            lcd.display_control);

}







/**
 * @brief
 * Enable cursor display.
 *
 */
void LCD_CursorOn(void)
{

    lcd.display_control |=
            LCD_CURSOR_ON_BIT;



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);

}







/**
 * @brief
 * Disable cursor display.
 *
 */
void LCD_CursorOff(void)
{

    lcd.display_control &=
            (uint8_t)(~LCD_CURSOR_ON_BIT);



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);

}







/**
 * @brief
 * Enable cursor blinking.
 *
 */
void LCD_BlinkOn(void)
{

    lcd.display_control |=
            LCD_BLINK_ON_BIT;



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
            (uint8_t)(~LCD_BLINK_ON_BIT);



    LCD_SendCommand(
            LCD_CMD_DISPLAY_ON |
            lcd.display_control);

}








/* ==========================================================================
 * Backlight Control
 * ========================================================================== */


/**
 * @brief
 * Enable LCD backlight.
 *
 * @details
 *
 * PCF8574 backpack controls the LCD
 * backlight through P3 output.
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







/* ==========================================================================
 * Custom Character Generation
 * ========================================================================== */


/**
 * @brief
 * Create custom character.
 *
 * @details
 *
 * HD44780 provides eight programmable
 * character locations.
 *
 *
 * Each character contains:
 *
 *      8 rows
 *
 *      5 pixels per row
 *
 *
 * @param location
 *      Character location:
 *
 *          0 ... 7
 *
 *
 * @param pattern
 *      Eight byte character bitmap.
 *
 *
 * Example:
 *
 *      uint8_t symbol[8] =
 *      {
 *          0x04,
 *          0x0E,
 *          0x1F,
 *          0x11,
 *          0x11,
 *          0x11,
 *          0x1F,
 *          0x00
 *      };
 *
 *
 *      LCD_CreateChar(0,symbol);
 *
 */
void LCD_CreateChar(
        uint8_t location,
        uint8_t pattern[])
{

    uint8_t i;



    /*
     * HD44780 supports only
     * eight CGRAM locations.
     */
    location &=
            0x07U;



    /*
     * Select CGRAM address.
     *
     * Each custom character
     * occupies eight bytes.
     */
    LCD_SendCommand(
            LCD_CMD_SET_CGRAM |
            (location << 3U));




    /*
     * Write character pattern.
     */
    for(i = 0U; i < 8U; i++)
    {

        LCD_SendData(
                pattern[i]);

    }

}







/**
 * @brief
 * Display custom character.
 *
 * @param location
 *      Character location:
 *
 *          0 ... 7
 *
 */
void LCD_PrintCustomChar(
        uint8_t location)
{

    LCD_SendData(
            location & 0x07U);

}








/* ==========================================================================
 * Display Shift Functions
 * ========================================================================== */


/**
 * @brief
 * Shift complete display content left.
 *
 */
void LCD_ScrollLeft(void)
{

    LCD_SendCommand(
            LCD_CMD_SHIFT_LEFT);

}







/**
 * @brief
 * Shift complete display content right.
 *
 */
void LCD_ScrollRight(void)
{

    LCD_SendCommand(
            LCD_CMD_SHIFT_RIGHT);

}







/* ==========================================================================
 * LCD Information Functions
 * ========================================================================== */


/**
 * @brief
 * Get LCD number of columns.
 *
 * @return
 *
 *      Configured column count.
 *
 *
 * Examples:
 *
 *      20  for 20x4 LCD
 *
 *      16  for 16x2 LCD
 *
 */
uint8_t LCD_GetColumns(void)
{

    return lcd.columns;

}







/**
 * @brief
 * Get LCD number of rows.
 *
 * @return
 *
 *      Configured row count.
 *
 *
 * Examples:
 *
 *      4  for 20x4 LCD
 *
 *      2  for 16x2 LCD
 *
 */
uint8_t LCD_GetRows(void)
{

    return lcd.rows;

}







/**
 * @brief
 * Get PCF8574 I2C address.
 *
 * @return
 *
 *      7-bit I2C address.
 *
 */
uint8_t LCD_GetAddress(void)
{

    return lcd.address;

}







/* ==========================================================================
 * Driver State Management
 * ========================================================================== */


/**
 * @brief
 * Reset internal driver state.
 *
 * @details
 *
 * This function clears the internal
 * driver context.
 *
 *
 * Useful when:
 *
 *      - LCD module is disconnected
 *
 *      - I2C recovery is required
 *
 *      - Driver restart is needed
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
 * Check LCD initialization state.
 *
 * @return
 *
 *      1:
 *
 *          LCD initialized.
 *
 *
 *      0:
 *
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






/* ==========================================================================
 * Driver Compatibility Notes
 * ========================================================================== */


/*
 *
 * This source file is compatible with:
 *
 *      lcd_i2c.h
 *
 *
 * Implemented public API:
 *
 *
 * Initialization:
 *
 *      LCD_Init()
 *
 *
 * Basic output:
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
 * Cursor control:
 *
 *      LCD_SetCursor()
 *
 *      LCD_Home()
 *
 *
 * Display clearing:
 *
 *      LCD_Clear()
 *
 *      LCD_ClearRow()
 *
 *
 * Display control:
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
 * Backlight:
 *
 *      LCD_BacklightOn()
 *
 *      LCD_BacklightOff()
 *
 *
 * Custom characters:
 *
 *      LCD_CreateChar()
 *
 *      LCD_PrintCustomChar()
 *
 *
 * Display shift:
 *
 *      LCD_ScrollLeft()
 *
 *      LCD_ScrollRight()
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
 * Driver state:
 *
 *      LCD_ResetState()
 *
 *      LCD_IsReady()
 *
 */




/* ==========================================================================
 * LCD Size Configuration Notes
 * ========================================================================== */


/*
 *
 * LCD SIZE CHANGE GUIDE
 * =====================
 *
 *
 * This driver supports:
 *
 *
 *      20x4 Character LCD
 *
 *
 *          Columns : 20
 *
 *          Rows    : 4
 *
 *
 *
 *      16x2 Character LCD
 *
 *
 *          Columns : 16
 *
 *          Rows    : 2
 *
 *
 *
 *
 * IMPORTANT:
 *
 * No modification is required
 * inside this lcd_i2c.c file.
 *
 *
 * The LCD size is selected only
 * during initialization.
 *
 *
 *
 * Example:
 *
 *
 * For 20x4 LCD:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          20,
 *          4);
 *
 *
 *
 * For 16x2 LCD:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          16,
 *          2);
 *
 *
 *
 *
 * The application layer should be
 * the only place where LCD geometry
 * is configured.
 *
 *
 * The hardware driver automatically
 * handles:
 *
 *
 *      - DDRAM row addressing
 *
 *      - Cursor limitation
 *
 *      - Row clearing
 *
 *      - Column limitation
 *
 *
 */







/* ==========================================================================
 * PCF8574 Hardware Mapping Reference
 * ========================================================================== */


/*
 *
 * PCF8574 Backpack Mapping:
 *
 *
 *
 *      PCF8574          HD44780 LCD
 *
 *
 *      P0  ------------  RS
 *
 *
 *      P1  ------------  RW
 *
 *
 *      P2  ------------  EN
 *
 *
 *      P3  ------------  Backlight
 *
 *
 *      P4  ------------  D4
 *
 *
 *      P5  ------------  D5
 *
 *
 *      P6  ------------  D6
 *
 *
 *      P7  ------------  D7
 *
 *
 *
 *
 * RW line is not used.
 *
 * This driver operates in:
 *
 *
 *      Write Only Mode
 *
 *
 */







/* ==========================================================================
 * I2C Address Notes
 * ========================================================================== */


/*
 *
 * PCF8574 address selection:
 *
 *
 *
 * PCF8574:
 *
 *      0x20 - 0x27
 *
 *
 *
 * PCF8574A:
 *
 *      0x38 - 0x3F
 *
 *
 *
 * Example:
 *
 *
 * Module marking:
 *
 *      PCF8574
 *
 *
 * Address:
 *
 *      0x27
 *
 *
 * Initialization:
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
 * STM32 HAL internally shifts:
 *
 *
 *      0x27 << 1
 *
 *
 *      = 0x4E
 *
 *
 * Do not pass:
 *
 *      0x4E
 *
 * to LCD_Init().
 *
 *
 */







/* ==========================================================================
 * LCD Refresh Strategy
 * ========================================================================== */


/*
 *
 * Character LCD modules are slow
 * compared with STM32 execution speed.
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
 * Recommended update method:
 *
 *
 *      LCD_SetCursor()
 *
 *      LCD_ClearRow()
 *
 *      LCD_Print()
 *
 *
 *
 * Benefits:
 *
 *
 *      - Less I2C traffic
 *
 *      - Reduced display flicker
 *
 *      - Faster user interface response
 *
 *
 */







/* ==========================================================================
 * Error Handling Notes
 * ========================================================================== */


/*
 *
 * Current implementation uses:
 *
 *
 *      HAL_I2C_Master_Transmit()
 *
 *
 * with:
 *
 *
 *      HAL_MAX_DELAY
 *
 *
 *
 * This is suitable for:
 *
 *
 *      - Monitoring devices
 *
 *      - Low speed user interfaces
 *
 *
 *
 * Future improvements may include:
 *
 *
 *      - Timeout handling
 *
 *      - I2C error recovery
 *
 *      - Non-blocking communication
 *
 *      - HAL callback support
 *
 *
 */







/* ==========================================================================
 * End Of File
 * ========================================================================== */


/**
 ******************************************************************************
 *
 *                      END OF FILE
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
 ******************************************************************************
 */



