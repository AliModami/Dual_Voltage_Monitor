/******************************************************************************
 * @file           lcd_i2c.h
 * @brief          HD44780 Character LCD Driver Interface over I2C PCF8574
 *
 * @details
 * Low level hardware abstraction layer for character LCD modules using:
 *
 *      STM32F103C8T6
 *              |
 *              | I2C1
 *              |
 *          PCF8574 I/O Expander
 *              |
 *              | 4-bit Parallel Interface
 *              |
 *          HD44780 LCD Controller
 *
 *
 * Supported LCD Modules:
 *
 *      - 20x4 Character LCD
 *      - 16x2 Character LCD
 *
 *
 * LCD Change Configuration:
 *
 * This driver supports both 20x4 and 16x2 LCD modules.
 *
 * To change LCD type, application code only needs to change:
 *
 *      1. LCD address
 *      2. LCD number of columns
 *      3. LCD number of rows
 *
 *
 * Example:
 *
 *      For 20x4 LCD:
 *
 *          LCD_I2C_ADDRESS_DEFAULT
 *          LCD_DEFAULT_COLUMNS = 20
 *          LCD_DEFAULT_ROWS    = 4
 *
 *
 *      For 16x2 LCD:
 *
 *          LCD_I2C_ADDRESS_DEFAULT
 *          LCD_DEFAULT_COLUMNS = 16
 *          LCD_DEFAULT_ROWS    = 2
 *
 *
 * No modification is required inside lcd_i2c.c.
 *
 *
 *
 * Hardware:
 *
 *      MCU:
 *          STM32F103C8T6
 *
 *
 *      I2C Interface:
 *
 *          I2C1
 *
 *      Default Pins:
 *
 *          PB6  -> I2C1_SCL
 *          PB7  -> I2C1_SDA
 *
 *
 *      LCD Interface:
 *
 *          PCF8574 / PCF8574A Backpack
 *
 *
 *
 * @architecture
 *
 * Application Layer:
 *
 *      menu.c
 *      monitor.c
 *      stream.c
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
 *
 * This driver controls only LCD hardware.
 *
 * It does not contain:
 *
 *      - Menu logic
 *      - Measurement processing
 *      - User interface rules
 *      - Application screens
 *
 *
 *
 * @version     3.0.0
 * @date        2026
 * @author      Dual Voltage Monitor Project
 *
 *
 * @history
 *
 * Version 1.0.0
 *      Initial LCD I2C driver.
 *
 * Version 2.0.0
 *      Improved documentation and hardware abstraction.
 *
 * Version 3.0.0
 *      Added flexible LCD geometry configuration.
 *      Added clear documentation for 16x2 and 20x4 usage.
 *
 ******************************************************************************/

#ifndef LCD_I2C_H
#define LCD_I2C_H


/* --------------------------------------------------------------------------
 * Includes
 * -------------------------------------------------------------------------- */


#include <stdint.h>

#include "stm32f1xx_hal.h"



/* --------------------------------------------------------------------------
 * LCD Hardware Configuration
 *
 * Change these values according to the connected LCD module.
 *
 * IMPORTANT:
 *
 * The application should pass these values to LCD_Init().
 *
 * The driver source file lcd_i2c.c does not need modification.
 * -------------------------------------------------------------------------- */


/**
 * @brief Default PCF8574 7-bit I2C address.
 *
 * Common PCF8574 addresses:
 *
 *      0x20 - 0x27
 *
 *
 * Example:
 *
 *      If backpack address is 0x27:
 *
 *          LCD_Init(&hi2c1, 0x27, columns, rows);
 *
 */
#define LCD_I2C_ADDRESS_DEFAULT          0x27U



/**
 * @brief PCF8574A alternative address range.
 *
 * PCF8574A modules normally use:
 *
 *      0x38 - 0x3F
 *
 *
 * Example:
 *
 *      If backpack address is 0x3F:
 *
 *          LCD_Init(&hi2c1, 0x3F, columns, rows);
 *
 */
#define LCD_I2C_ADDRESS_PCF8574A         0x3FU




/**
 * @brief Default LCD configuration.
 *
 *
 * Select the connected LCD type here.
 *
 *
 * 20x4 LCD configuration:
 *
 *      Columns = 20
 *      Rows    = 4
 *
 *
 *      #define LCD_DEFAULT_COLUMNS 20U
 *      #define LCD_DEFAULT_ROWS     4U
 *
 *
 *
 * 16x2 LCD configuration:
 *
 *      Columns = 16
 *      Rows    = 2
 *
 *
 *      #define LCD_DEFAULT_COLUMNS 16U
 *      #define LCD_DEFAULT_ROWS     2U
 *
 *
 *
 * Only these values need to be changed when replacing LCD module.
 *
 */
#define LCD_DEFAULT_COLUMNS              20U
#define LCD_DEFAULT_ROWS                  4U



/* --------------------------------------------------------------------------
 * HD44780 Command Definitions
 *
 * These constants replace raw numeric values.
 * -------------------------------------------------------------------------- */


/**
 * @brief Clear display command.
 */
#define LCD_CMD_CLEAR_DISPLAY            0x01U



/**
 * @brief Return cursor to home position.
 */
#define LCD_CMD_RETURN_HOME              0x02U



/**
 * @brief Entry mode configuration.
 *
 * Cursor moves right.
 * Display does not shift.
 */
#define LCD_CMD_ENTRY_MODE               0x06U



/**
 * @brief Display ON command.
 *
 * Display ON.
 * Cursor OFF.
 * Blink OFF.
 */
#define LCD_CMD_DISPLAY_ON               0x0CU



/**
 * @brief Display OFF command.
 */
#define LCD_CMD_DISPLAY_OFF              0x08U



/**
 * @brief Function set command.
 *
 * 4-bit interface.
 * 2-line mode.
 * 5x8 font.
 *
 * Note:
 *
 * HD44780 uses the same command for:
 *
 *      16x2 LCD
 *      20x4 LCD
 *
 * because both displays internally use
 * two controller lines.
 */
#define LCD_CMD_FUNCTION_SET             0x28U



/**
 * @brief Set CGRAM address command.
 */
#define LCD_CMD_SET_CGRAM                0x40U



/**
 * @brief Set DDRAM address command.
 */
#define LCD_CMD_SET_DDRAM                0x80U

/* --------------------------------------------------------------------------
 * PCF8574 Backpack Pin Mapping
 *
 * Default LCD I2C backpack wiring:
 *
 *
 *      PCF8574          HD44780 LCD
 *
 *      P0  ------------ RS
 *
 *      P1  ------------ RW
 *
 *      P2  ------------ EN
 *
 *      P3  ------------ Backlight
 *
 *      P4  ------------ D4
 *
 *      P5  ------------ D5
 *
 *      P6  ------------ D6
 *
 *      P7  ------------ D7
 *
 *
 * RW line is not used because this driver operates
 * in write-only mode.
 * -------------------------------------------------------------------------- */


/**
 * @brief Register Select bit.
 *
 * RS = 0:
 *
 *      Command register
 *
 *
 * RS = 1:
 *
 *      Data register
 */
#define LCD_PCF8574_RS_BIT                0x01U



/**
 * @brief Read/Write bit.
 *
 * The driver never reads from LCD.
 */
#define LCD_PCF8574_RW_BIT                0x02U



/**
 * @brief Enable pulse bit.
 *
 * HD44780 captures data on EN falling edge.
 */
#define LCD_PCF8574_ENABLE_BIT            0x04U



/**
 * @brief Backlight control bit.
 */
#define LCD_PCF8574_BACKLIGHT_BIT         0x08U





/* --------------------------------------------------------------------------
 * LCD Driver Handle Structure
 * -------------------------------------------------------------------------- */


/**
 * @brief LCD driver context structure.
 *
 * This structure stores all runtime information
 * required to control one LCD module.
 *
 *
 * The structure supports both:
 *
 *      - 20x4 LCD
 *      - 16x2 LCD
 *
 *
 * The LCD size is defined by:
 *
 *      columns
 *
 *      rows
 *
 */
typedef struct
{

    /**
     * @brief STM32 HAL I2C handle.
     *
     * Example:
     *
     *      &hi2c1
     */
    I2C_HandleTypeDef *hi2c;



    /**
     * @brief PCF8574 7-bit I2C address.
     *
     * Example:
     *
     *      0x27
     *
     * Note:
     *
     * HAL internally shifts this value:
     *
     *      0x27 << 1 = 0x4E
     */
    uint8_t address;



    /**
     * @brief Number of LCD columns.
     *
     * Examples:
     *
     *      20 for 20x4 LCD
     *
     *      16 for 16x2 LCD
     */
    uint8_t columns;



    /**
     * @brief Number of LCD rows.
     *
     * Examples:
     *
     *      4 for 20x4 LCD
     *
     *      2 for 16x2 LCD
     */
    uint8_t rows;



    /**
     * @brief Current backlight state.
     */
    uint8_t backlight_state;



    /**
     * @brief Display control shadow register.
     *
     * Stores:
     *
     *      Display ON/OFF
     *
     *      Cursor ON/OFF
     *
     *      Blink ON/OFF
     */
    uint8_t display_control;



    /**
     * @brief Display entry mode shadow register.
     *
     * Controls:
     *
     *      Cursor direction
     *
     *      Display shift
     */
    uint8_t display_mode;


} LCD_HandleTypeDef;







/* --------------------------------------------------------------------------
 * LCD Initialization API
 * -------------------------------------------------------------------------- */


/**
 * @brief Initialize LCD module.
 *
 * @details
 *
 * This function:
 *
 *      - Stores LCD hardware configuration
 *
 *      - Initializes internal driver state
 *
 *      - Executes HD44780 initialization sequence
 *
 *      - Enables LCD display
 *
 *
 * Supported modules:
 *
 *      20x4
 *
 *      16x2
 *
 *
 * The LCD type is selected by:
 *
 *      columns
 *
 *      rows
 *
 *
 * Example:
 *
 * For 20x4 LCD:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          20U,
 *          4U);
 *
 *
 *
 * For 16x2 LCD:
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          16U,
 *          2U);
 *
 *
 *
 * @param hi2c
 *      Pointer to STM32 HAL I2C handle.
 *
 *
 * @param address
 *      7-bit PCF8574 address.
 *
 *
 * @param columns
 *      LCD character width.
 *
 *
 * @param rows
 *      LCD number of lines.
 *
 *
 * @return
 *
 *      HAL_OK:
 *
 *          LCD initialized successfully.
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
        uint8_t rows);





/* --------------------------------------------------------------------------
 * Basic Text Output Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Print null terminated string.
 *
 * Example:
 *
 *      LCD_Print("Voltage Monitor");
 *
 */
void LCD_Print(const char *text);




/**
 * @brief Print single character.
 *
 * @param character
 *      ASCII character.
 */
void LCD_PrintChar(char character);




/**
 * @brief Print signed integer value.
 *
 * Example:
 *
 *      LCD_PrintNumber(220);
 *
 */
void LCD_PrintNumber(int32_t value);




/**
 * @brief Print floating point value.
 *
 * Example:
 *
 *      LCD_PrintFloat(220.5f,1);
 *
 * Result:
 *
 *      220.5
 */
void LCD_PrintFloat(
        float value,
        uint8_t decimals);





/* --------------------------------------------------------------------------
 * Cursor Position Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Set LCD cursor position.
 *
 * @details
 *
 * HD44780 does not use continuous memory
 * addresses for LCD rows.
 *
 *
 * Internal DDRAM mapping:
 *
 *
 *      20x4 LCD:
 *
 *          Row 0 -> 0x00
 *          Row 1 -> 0x40
 *          Row 2 -> 0x14
 *          Row 3 -> 0x54
 *
 *
 *      16x2 LCD:
 *
 *          Row 0 -> 0x00
 *          Row 1 -> 0x40
 *
 *
 * The driver automatically selects the correct
 * row address according to the configured
 * number of rows.
 *
 *
 * Application code does not need to change
 * when replacing 20x4 with 16x2 LCD.
 *
 *
 * @param column
 *      Column number starting from zero.
 *
 *
 * @param row
 *      Row number starting from zero.
 *
 */
void LCD_SetCursor(
        uint8_t column,
        uint8_t row);





/**
 * @brief Return cursor to home position.
 *
 * The display content remains unchanged.
 *
 */
void LCD_Home(void);





/* --------------------------------------------------------------------------
 * Display Clear Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Clear complete LCD display.
 *
 * @details
 *
 * Clears DDRAM memory and moves cursor
 * to the home position.
 *
 *
 * Note:
 *
 * HD44780 clear command requires extra time.
 *
 * Avoid calling continuously in fast loops.
 *
 */
void LCD_Clear(void);





/**
 * @brief Clear one LCD row.
 *
 * @details
 *
 * The function overwrites one complete row
 * with spaces.
 *
 *
 * This is recommended for dynamic screens:
 *
 *      - Voltage monitor
 *      - Stream display
 *      - Status display
 *
 *
 * Works with:
 *
 *      - 20x4 LCD
 *      - 16x2 LCD
 *
 *
 * @param row
 *      Row index.
 *
 */
void LCD_ClearRow(uint8_t row);






/* --------------------------------------------------------------------------
 * Display Control Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Turn display ON.
 *
 * Display memory remains unchanged.
 *
 */
void LCD_DisplayOn(void);





/**
 * @brief Turn display OFF.
 *
 * Display memory remains unchanged.
 *
 */
void LCD_DisplayOff(void);





/**
 * @brief Enable cursor display.
 *
 */
void LCD_CursorOn(void);





/**
 * @brief Disable cursor display.
 *
 */
void LCD_CursorOff(void);





/**
 * @brief Enable blinking cursor.
 *
 */
void LCD_BlinkOn(void);





/**
 * @brief Disable blinking cursor.
 *
 */
void LCD_BlinkOff(void);






/* --------------------------------------------------------------------------
 * Backlight Control Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Enable LCD backlight.
 *
 */
void LCD_BacklightOn(void);





/**
 * @brief Disable LCD backlight.
 *
 */
void LCD_BacklightOff(void);







/* --------------------------------------------------------------------------
 * Custom Character Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Create custom character in CGRAM.
 *
 * @details
 *
 * HD44780 supports:
 *
 *      8 custom characters
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
 *      Character index:
 *
 *          0 ... 7
 *
 *
 * @param pattern
 *      Character bitmap data.
 *
 *
 * Example:
 *
 *
 *      uint8_t battery[8] =
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
 *      LCD_CreateChar(0,battery);
 *
 */
void LCD_CreateChar(
        uint8_t location,
        uint8_t pattern[]);





/**
 * @brief Print previously created custom character.
 *
 * @param location
 *      Character index:
 *
 *          0 ... 7
 *
 */
void LCD_PrintCustomChar(
        uint8_t location);






/* --------------------------------------------------------------------------
 * Display Shift Functions
 * -------------------------------------------------------------------------- */


/**
 * @brief Shift complete display content left.
 *
 */
void LCD_ScrollLeft(void);





/**
 * @brief Shift complete display content right.
 *
 */
void LCD_ScrollRight(void);







/* --------------------------------------------------------------------------
 * Low Level Command Interface
 *
 * These functions are intended for advanced users.
 *
 * Normal application code should use:
 *
 *      LCD_Clear()
 *      LCD_SetCursor()
 *      LCD_Print()
 *
 * -------------------------------------------------------------------------- */


/**
 * @brief Send raw HD44780 command.
 *
 * @param command
 *      LCD controller command.
 *
 */
void LCD_SendCommand(uint8_t command);





/**
 * @brief Send raw data byte.
 *
 * @param data
 *      ASCII data byte.
 *
 */
void LCD_SendData(uint8_t data);






/* --------------------------------------------------------------------------
 * LCD Information Functions
 *
 * These functions allow application code
 * to read current LCD configuration.
 *
 * Useful when supporting multiple LCD sizes.
 * -------------------------------------------------------------------------- */


/**
 * @brief Get configured LCD column count.
 *
 * @return
 *
 *      Number of columns.
 *
 */
uint8_t LCD_GetColumns(void);





/**
 * @brief Get configured LCD row count.
 *
 * @return
 *
 *      Number of rows.
 *
 */
uint8_t LCD_GetRows(void);





/**
 * @brief Get current PCF8574 address.
 *
 * @return
 *
 *      7-bit I2C address.
 *
 */
uint8_t LCD_GetAddress(void);






/* --------------------------------------------------------------------------
 * Driver State Management
 * -------------------------------------------------------------------------- */


/**
 * @brief Reset internal LCD driver state.
 *
 * @details
 *
 * This function clears the internal driver context.
 *
 * Useful when:
 *
 *      - LCD module is disconnected
 *      - I2C recovery is required
 *      - Driver restart is needed
 *
 */
void LCD_ResetState(void);





/**
 * @brief Check LCD driver initialization status.
 *
 * @return
 *
 *      1  LCD initialized
 *
 *      0  LCD not initialized
 *
 */
uint8_t LCD_IsReady(void);







/* --------------------------------------------------------------------------
 * Usage Example
 * -------------------------------------------------------------------------- */


/**
 * @example
 *
 *
 * Example 1:
 *
 *      20x4 LCD configuration
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          LCD_DEFAULT_COLUMNS_20X4,
 *          LCD_DEFAULT_ROWS_20X4);
 *
 *
 *
 * Example 2:
 *
 *
 *      16x2 LCD configuration
 *
 *
 *      LCD_Init(
 *          &hi2c1,
 *          LCD_I2C_ADDRESS_DEFAULT,
 *          LCD_DEFAULT_COLUMNS_16X2,
 *          LCD_DEFAULT_ROWS_16X2);
 *
 *
 *
 *
 * When changing LCD size:
 *
 *
 *      Only LCD_Init() parameters need to change.
 *
 *
 * No modification is required in:
 *
 *      - lcd_i2c.c
 *      - lcd_display.c
 *      - menu.c
 *
 *
 * The driver automatically handles:
 *
 *      - DDRAM row addressing
 *      - Cursor limitation
 *      - Row clearing
 *      - Column limitation
 *
 */







/* --------------------------------------------------------------------------
 * Hardware Configuration Notes
 * -------------------------------------------------------------------------- */


/**
 * @note
 *
 * LCD I2C Address Configuration:
 *
 *
 * The address depends on the PCF8574
 * backpack solder jumpers.
 *
 *
 *
 * Common addresses:
 *
 *
 * PCF8574:
 *
 *      0x20 - 0x27
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
 *      LCD_I2C_ADDRESS_DEFAULT = 0x27
 *
 *
 *
 * IMPORTANT:
 *
 * The value is a 7-bit I2C address.
 *
 *
 * STM32 HAL automatically performs:
 *
 *
 *      Address << 1
 *
 *
 * Example:
 *
 *
 *      0x27 << 1 = 0x4E
 *
 *
 * User should NOT enter 0x4E.
 *
 */





/**
 * @note
 *
 * LCD Size Configuration:
 *
 *
 * This driver supports:
 *
 *
 *      20 columns x 4 rows
 *
 *      16 columns x 2 rows
 *
 *
 *
 * Recommended configuration:
 *
 *
 * For 20x4 LCD:
 *
 *      LCD_DEFAULT_COLUMNS = 20
 *
 *      LCD_DEFAULT_ROWS    = 4
 *
 *
 *
 * For 16x2 LCD:
 *
 *      LCD_DEFAULT_COLUMNS = 16
 *
 *      LCD_DEFAULT_ROWS    = 2
 *
 *
 *
 * Changing the display size does not require
 * modification of the driver source code.
 *
 * Only the configuration values must change.
 *
 */







/* --------------------------------------------------------------------------
 * Driver Design Notes
 * -------------------------------------------------------------------------- */


/**
 * @note
 *
 * This driver is designed for one LCD instance.
 *
 *
 * Multiple LCD modules require:
 *
 *      - Multiple LCD_HandleTypeDef objects
 *      - Separate I2C addresses
 *      - Extended driver management
 *
 */





/**
 * @note
 *
 * Character LCD refresh:
 *
 *
 * Character LCD modules are slow compared
 * with STM32 execution speed.
 *
 *
 * Avoid:
 *
 *
 *      LCD_Clear()
 *
 *
 * inside fast update loops.
 *
 *
 * Recommended method:
 *
 *
 *      LCD_SetCursor()
 *
 *      LCD_ClearRow()
 *
 *      LCD_Print()
 *
 *
 * Benefits:
 *
 *      - No display flicker
 *      - Lower I2C traffic
 *      - Faster UI response
 *
 */





/**
 * @note
 *
 * Thread safety:
 *
 *
 * This driver is not designed for concurrent
 * access from multiple tasks.
 *
 *
 * If RTOS support is added:
 *
 *      Protect LCD functions with mutex.
 *
 */






/* --------------------------------------------------------------------------
 * Header End
 * -------------------------------------------------------------------------- */


#endif /* LCD_I2C_H */



/******************************************************************************
 *
 *                      END OF FILE
 *
 *
 *      lcd_i2c.h
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
