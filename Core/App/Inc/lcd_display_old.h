/**
 ******************************************************************************
 * @file           lcd_display.h
 * @brief          LCD Display Abstraction Layer Interface
 *
 * @details
 * This file defines the public interface of the LCD Display layer.
 *
 * The Display layer provides an abstraction between the application logic
 * and the low-level LCD hardware driver.
 *
 * The purpose of this module is to allow application modules such as:
 *
 *      - menu.c
 *      - monitor.c
 *      - stream.c
 *
 * to request display operations without knowing any hardware details.
 *
 * The low-level communication with:
 *
 *      - I2C peripheral
 *      - PCF8574 backpack
 *      - HD44780 LCD controller
 *
 * is handled by lcd_i2c.c.
 *
 *
 * Architecture:
 *
 *      Application Layer
 *              |
 *              v
 *      lcd_display.c
 *      (Display management)
 *              |
 *              v
 *      lcd_i2c.c
 *      (Hardware driver)
 *              |
 *              v
 *      PCF8574 + LCD Module
 *
 *
 * @hardware
 *      MCU:
 *          STM32F103C8T6
 *
 *      Display:
 *          Character LCD
 *
 *      Interface:
 *          I2C using PCF8574 backpack
 *
 *
 * @design_note
 *
 * This module defines what should be displayed.
 *
 * It does not define how the LCD hardware is controlled.
 *
 * Keeping this separation allows future replacement of the LCD hardware
 * without changing the application modules.
 *
 *
 * @version    1.0.0
 * @date       2026
 * @author     Dual Voltage Monitor Project
 *
 *
 * @change_history
 *
 * Version 1.0.0:
 *      Initial professional documentation update.
 *
 ******************************************************************************
 */


#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H


/* --------------------------------------------------------------------------
 * Includes
 *
 * Required external definitions for this module.
 * --------------------------------------------------------------------------
 */


#include <stdint.h>

#include "stm32f1xx_hal.h"



/* --------------------------------------------------------------------------
 * LCD Display Configuration
 *
 * General configuration parameters used by the Display layer.
 *
 * These definitions avoid the use of magic numbers and keep the LCD
 * dimensions configurable from a single location.
 * --------------------------------------------------------------------------
 */


/**
 * @brief Default LCD column count.
 *
 * This value represents the number of character positions available
 * on each LCD row.
 */
#define LCD_DISPLAY_COLS              20U



/**
 * @brief Default LCD row count.
 *
 * This value represents the number of available display lines.
 */
#define LCD_DISPLAY_ROWS              4U



/**
 * @brief Maximum title string length.
 *
 * This buffer stores the current page title internally.
 */
#define LCD_DISPLAY_TITLE_SIZE        32U




/* --------------------------------------------------------------------------
 * Display Page Enumeration
 *
 * Defines the logical pages managed by the Display layer.
 *
 * Using an enumeration instead of numeric values improves:
 *
 *      - code readability
 *      - maintenance
 *      - future menu expansion
 *
 * --------------------------------------------------------------------------
 */


/**
 * @brief LCD display page identifiers.
 */
typedef enum
{

    /**
     * @brief No active display page.
     */
    LCD_PAGE_NONE = 0,


    /**
     * @brief Home or main screen.
     */
    LCD_PAGE_HOME,


    /**
     * @brief Live voltage monitoring screen.
     */
    LCD_PAGE_MONITOR,


    /**
     * @brief UART streaming display screen.
     */
    LCD_PAGE_STREAM,


    /**
     * @brief User configuration settings screen.
     */
    LCD_PAGE_SETTINGS,


    /**
     * @brief System information screen.
     */
    LCD_PAGE_SYSTEM_INFO


} LCD_DisplayPage_t;





/* --------------------------------------------------------------------------
 * LCD Display Handle
 *
 * Stores the internal state information of the Display layer.
 *
 * This structure follows the same design concept used by STM32 HAL handles.
 *
 * Keeping module status inside one structure avoids scattered global
 * variables and improves maintainability.
 *
 * --------------------------------------------------------------------------
 */


/**
 * @brief LCD Display module handle.
 *
 * Contains:
 *
 *      - Current page information
 *      - Refresh status
 *      - LCD title buffer
 *      - Associated I2C peripheral handle
 */
typedef struct
{

    /**
     * @brief Current page title text.
     *
     * Example:
     *
     *      "Voltage Monitor"
     */
    char title[LCD_DISPLAY_TITLE_SIZE];



    /**
     * @brief Currently active display page.
     */
    LCD_DisplayPage_t current_page;



    /**
     * @brief Display refresh request flag.
     *
     * Value meaning:
     *
     *      1 : Display update is required
     *
     *      0 : Current display content is valid
     */
    uint8_t refresh_required;



    /**
     * @brief Pointer to the I2C peripheral used by LCD.
     *
     * Example:
     *
     *      &hi2c1
     */
    I2C_HandleTypeDef *hi2c;



} LCD_DisplayHandle_t;
/* --------------------------------------------------------------------------
 * Public Function Prototypes
 *
 * The functions below represent the official API of the Display layer.
 *
 * Application modules should use only these functions.
 *
 * Direct access to lcd_i2c.c from application modules is not allowed.
 *
 * --------------------------------------------------------------------------
 */


/* --------------------------------------------------------------------------
 * Initialization
 * --------------------------------------------------------------------------
 */


/**
 * @brief Initialize the LCD Display abstraction layer.
 *
 * @details
 * This function initializes the Display module and the lower-level
 * LCD driver.
 *
 * Initialization sequence:
 *
 *      1. Store the I2C handle.
 *      2. Initialize the LCD hardware driver.
 *      3. Enable LCD backlight.
 *      4. Clear the display.
 *      5. Prepare the initial display state.
 *
 *
 * @param hi2c
 *      Pointer to the I2C peripheral handle connected to the LCD.
 *
 * @note
 *      The I2C peripheral must be initialized before calling this function.
 *
 * @example
 *
 *      LCD_Display_Init(&hi2c1);
 *
 */
void LCD_Display_Init(I2C_HandleTypeDef *hi2c);





/* --------------------------------------------------------------------------
 * Basic Display Operations
 * --------------------------------------------------------------------------
 */


/**
 * @brief Clear the complete LCD display.
 *
 * @details
 * Removes all visible characters from the LCD and resets the cursor
 * position through the lower-level LCD driver.
 *
 */
void LCD_Display_Clear(void);




/**
 * @brief Display a page title.
 *
 * @details
 * The title is normally displayed on the first LCD row.
 *
 * The title is also stored internally for future use.
 *
 *
 * @param title
 *      Pointer to the title string.
 *
 * @note
 *      NULL pointer input is ignored.
 *
 */
void LCD_Display_ShowTitle(const char *title);





/**
 * @brief Print text on a specific LCD row.
 *
 * @details
 * The selected row is cleared before writing new content.
 *
 * This prevents old characters remaining when the new text is shorter
 * than the previous content.
 *
 *
 * @param row
 *      LCD row index.
 *
 *      Example for a 20x4 LCD:
 *
 *          Row 0 : First line
 *          Row 1 : Second line
 *          Row 2 : Third line
 *          Row 3 : Fourth line
 *
 *
 * @param text
 *      Text string to display.
 *
 */
void LCD_Display_PrintLine(uint8_t row,
                           const char *text);





/**
 * @brief Display a floating-point value with a label.
 *
 * @details
 * This function is intended for measured values such as:
 *
 *      Vin
 *      Vout
 *      Temperature
 *
 * It keeps formatting logic inside the Display layer and prevents
 * duplicated LCD formatting code across application modules.
 *
 *
 * @param row
 *      LCD row index.
 *
 * @param label
 *      Text displayed before the numeric value.
 *
 * @param value
 *      Floating-point value to display.
 *
 * @param decimals
 *      Number of decimal digits.
 *
 */
void LCD_Display_PrintFloat(uint8_t row,
                            const char *label,
                            float value,
                            uint8_t decimals);





/* --------------------------------------------------------------------------
 * Display Page Management
 * --------------------------------------------------------------------------
 */


/**
 * @brief Set the current LCD page identifier.
 *
 * @details
 * Used by menu and application modules to keep track of the active
 * display context.
 *
 *
 * @param page
 *      New display page identifier.
 *
 */
void LCD_Display_SetPage(LCD_DisplayPage_t page);





/**
 * @brief Get the current LCD page identifier.
 *
 * @return
 *      Current active display page.
 *
 */
LCD_DisplayPage_t LCD_Display_GetPage(void);





/* --------------------------------------------------------------------------
 * Refresh Management
 * --------------------------------------------------------------------------
 */


/**
 * @brief Request a display refresh.
 *
 * @details
 * The Display layer does not continuously rewrite the LCD.
 *
 * Instead, a refresh flag is used to indicate that the visible content
 * should be updated.
 *
 * This reduces:
 *
 *      - CPU usage
 *      - unnecessary I2C traffic
 *      - LCD flicker
 *
 */
void LCD_Display_RequestRefresh(void);





/**
 * @brief Check whether a display refresh is required.
 *
 * @return
 *
 *      1 : Refresh required
 *
 *      0 : No refresh required
 *
 */
uint8_t LCD_Display_NeedRefresh(void);





/**
 * @brief Clear the display refresh request flag.
 *
 * @details
 * Called after the display content has been updated successfully.
 *
 */
void LCD_Display_ClearRefreshFlag(void);





/* --------------------------------------------------------------------------
 * LCD Backlight Control
 * --------------------------------------------------------------------------
 */


/**
 * @brief Turn LCD backlight on.
 *
 */
void LCD_Display_BacklightOn(void);





/**
 * @brief Turn LCD backlight off.
 *
 */
void LCD_Display_BacklightOff(void);

/**
 * @brief
 * Get LCD display handle.
 *
 * @return
 * Pointer to LCD display handle.
 *
 */
LCD_DisplayHandle_t *LCD_Display_GetHandle(void);



#endif /* LCD_DISPLAY_H */


/******************************************************************************
 * End of File
 *
 * This header defines only the public interface of the LCD Display layer.
 *
 * The hardware-specific implementation remains inside lcd_i2c.c.
 *
 *****************************************************************************/
