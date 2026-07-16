/**
 ******************************************************************************
 * @file           lcd_display.c
 * @brief          LCD Display Abstraction Layer Implementation
 *
 * @details
 * This file implements the Display abstraction layer for the
 * Dual Voltage Monitor project.
 *
 * The purpose of this module is to provide a clean interface between
 * application modules and the low-level LCD hardware driver.
 *
 * Application modules should only request display operations through
 * this module.
 *
 * This module does not handle:
 *
 *      - I2C communication details
 *      - PCF8574 control
 *      - HD44780 command sequences
 *
 * Those responsibilities belong to:
 *
 *      lcd_i2c.c
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
 *      (LCD hardware driver)
 *              |
 *              v
 *      I2C + PCF8574 + LCD
 *
 *
 * @hardware
 *
 *      MCU:
 *          STM32F103C8T6
 *
 *      Display:
 *          Character LCD 20x4 / 16x2
 *
 *      Interface:
 *          I2C LCD backpack using PCF8574
 *
 *
 * @design_reason
 *
 * Keeping a separate Display layer prevents the application from
 * becoming dependent on a specific LCD controller or communication method.
 *
 * Example:
 *
 *      menu.c
 *          |
 *          v
 *      LCD_Display_PrintLine()
 *          |
 *          v
 *      lcd_i2c.c
 *          |
 *          v
 *      LCD Hardware
 *
 *
 * @version
 *      1.0.0
 *
 * @author
 *      Dual Voltage Monitor Project
 *
 *
 * @change_history
 *
 *      Version 1.0.0:
 *          Initial professional documentation update.
 *
 ******************************************************************************
 */


/* --------------------------------------------------------------------------
 * Includes
 *
 * External dependencies required by this module.
 * --------------------------------------------------------------------------
 */


#include "lcd_display.h"

#include "lcd_i2c.h"

#include <stdio.h>
#include <string.h>




/* --------------------------------------------------------------------------
 * Private Constants
 *
 * Module-specific constants.
 *
 * These values are kept private because they are only required internally
 * by this source file.
 *
 * --------------------------------------------------------------------------
 */


/**
 * @brief Default LCD I2C address.
 *
 * The value is defined by the LCD driver configuration.
 *
 * Keeping this definition here allows the Display layer to configure
 * the hardware driver without exposing low-level details to the application.
 */
#define LCD_DISPLAY_I2C_ADDRESS      LCD_I2C_ADDRESS_DEFAULT



/**
 * @brief Space character used for line clearing.
 *
 * Character LCD modules do not support deleting individual characters.
 * Writing spaces is the standard method to clear previous content.
 */
#define LCD_DISPLAY_SPACE_CHARACTER  ' '





/* --------------------------------------------------------------------------
 * Private Variables
 *
 * Internal state of the LCD Display module.
 *
 * This variable is private to this file and cannot be accessed directly
 * from other modules.
 *
 * --------------------------------------------------------------------------
 */


/**
 * @brief LCD Display module state handle.
 *
 * Stores:
 *
 *      - Current display page
 *      - Refresh status
 *      - Current title
 *      - Associated I2C peripheral
 *
 */
static LCD_DisplayHandle_t lcd_display;




/* --------------------------------------------------------------------------
 * Private Function Prototypes
 *
 * Functions declared here are only used internally by this module.
 *
 * --------------------------------------------------------------------------
 */


/**
 * @brief Clear a single LCD row.
 *
 * @param row
 *      Target LCD row index.
 *
 * @note
 *      This function is private and should not be called directly
 *      by application modules.
 */
static void LCD_Display_ClearLine(uint8_t row);





/* ==========================================================================
 * Initialization
 * ========================================================================== */


/**
 * @brief Initialize the LCD Display layer.
 *
 * @details
 * This function initializes both the Display abstraction layer and
 * the lower-level LCD driver.
 *
 * Initialization sequence:
 *
 *      1. Store the I2C handle.
 *      2. Initialize LCD hardware driver.
 *      3. Enable LCD backlight.
 *      4. Clear the display.
 *      5. Initialize internal display state.
 *
 *
 * @param hi2c
 *      Pointer to the I2C peripheral handle connected to LCD.
 *
 * @note
 *      The I2C peripheral must already be initialized before calling
 *      this function.
 *
 */
void LCD_Display_Init(I2C_HandleTypeDef *hi2c)
{

    /*
     * Store the I2C peripheral handle.
     *
     * The Display layer keeps this reference because the LCD hardware
     * communication is performed through the lower-level driver.
     */
    lcd_display.hi2c = hi2c;



    /*
     * Initialize the LCD hardware driver.
     *
     * The Display layer only uses the public driver API.
     * Hardware-specific implementation remains hidden inside lcd_i2c.c.
     */
    LCD_Init(
            hi2c,
            LCD_DISPLAY_I2C_ADDRESS,
            LCD_DISPLAY_COLS,
            LCD_DISPLAY_ROWS);



    /*
     * Enable LCD backlight after successful initialization.
     */
    LCD_BacklightOn();



    /*
     * Clear any undefined content from LCD memory.
     */
    LCD_Clear();



    /*
     * Set initial display state.
     *
     * No page is active immediately after initialization.
     */
    lcd_display.current_page = LCD_PAGE_NONE;



    /*
     * Force first display update.
     */
    lcd_display.refresh_required = 1U;



    /*
     * Clear stored title buffer.
     */
    memset(
            lcd_display.title,
            0,
            sizeof(lcd_display.title));

}

/* ==========================================================================
 * Private Utility Functions
 * ========================================================================== */


/**
 * @brief Clear one LCD row.
 *
 * @details
 * Character LCD modules do not provide a command to remove individual
 * characters.
 *
 * Therefore, this function clears a row by overwriting all character
 * positions with blank spaces.
 *
 * Clearing only one row instead of the complete LCD prevents unnecessary
 * screen flicker and reduces I2C communication.
 *
 *
 * @param row
 *      LCD row index to clear.
 *
 */
static void LCD_Display_ClearLine(uint8_t row)
{

    uint8_t column;



    /*
     * Move cursor to the beginning of the selected row.
     */
    LCD_SetCursor(
            0U,
            row);



    /*
     * Replace all characters in this row with spaces.
     *
     * The LCD controller keeps previous characters visible unless they
     * are overwritten.
     */
    for(column = 0U;
        column < LCD_DISPLAY_COLS;
        column++)
    {
        LCD_PrintChar(
                LCD_DISPLAY_SPACE_CHARACTER);
    }



    /*
     * Return cursor to the beginning of the row.
     *
     * This allows the next write operation to start from a known position.
     */
    LCD_SetCursor(
            0U,
            row);

}






/* ==========================================================================
 * Basic Display Functions
 * ========================================================================== */


/**
 * @brief Clear the complete LCD display.
 *
 * @details
 * This function removes all visible LCD content by using the low-level
 * LCD driver.
 *
 * After clearing the screen, a refresh request is generated because
 * the visible display content has changed.
 *
 */
void LCD_Display_Clear(void)
{

    /*
     * Clear LCD content using the hardware abstraction driver.
     */
    LCD_Clear();



    /*
     * Notify the display manager that a new screen update is required.
     */
    lcd_display.refresh_required = 1U;

}






/**
 * @brief Display a title on the first LCD row.
 *
 * @details
 * This function stores the title internally and displays it on the
 * first LCD row.
 *
 * Keeping the title in the display handle allows future features such as:
 *
 *      - screen redraw
 *      - page restoration
 *      - menu navigation support
 *
 *
 * @param title
 *      Pointer to the title string.
 *
 */
void LCD_Display_ShowTitle(const char *title)
{

    /*
     * Ignore invalid input.
     *
     * Prevents accessing memory through a NULL pointer.
     */
    if(title == NULL)
    {
        return;
    }



    /*
     * Copy title into internal storage.
     *
     * The size limit prevents buffer overflow.
     */
    strncpy(
            lcd_display.title,
            title,
            LCD_DISPLAY_TITLE_SIZE - 1U);



    /*
     * Guarantee string termination.
     *
     * C strings must always end with '\0'.
     */
    lcd_display.title[LCD_DISPLAY_TITLE_SIZE - 1U] = '\0';



    /*
     * Clear previous title content.
     */
    LCD_Display_ClearLine(0U);



    /*
     * Position cursor at the first row.
     */
    LCD_SetCursor(
            0U,
            0U);



    /*
     * Write new title text.
     */
    LCD_Print(
            lcd_display.title);



    /*
     * Request display update.
     */
    lcd_display.refresh_required = 1U;

}






/**
 * @brief Print text on a selected LCD row.
 *
 * @details
 * The selected row is cleared before writing new content.
 *
 * This prevents remaining characters from previous longer strings.
 *
 * Example:
 *
 * Previous:
 *
 *      Voltage:220.50V
 *
 * New:
 *
 *      V:9V
 *
 * Without clearing:
 *
 *      V:9Vage:220.50V
 *
 * may remain visible.
 *
 *
 * @param row
 *      LCD row index.
 *
 * @param text
 *      Text string to display.
 *
 */
void LCD_Display_PrintLine(
        uint8_t row,
        const char *text)
{

    /*
     * Check row range.
     *
     * Prevents writing outside the physical LCD area.
     */
    if(row >= LCD_DISPLAY_ROWS)
    {
        return;
    }



    /*
     * Ignore invalid string pointers.
     */
    if(text == NULL)
    {
        return;
    }



    /*
     * Remove previous row content.
     */
    LCD_Display_ClearLine(
            row);



    /*
     * Move cursor to selected row beginning.
     */
    LCD_SetCursor(
            0U,
            row);



    /*
     * Write new text.
     */
    LCD_Print(
            text);



    /*
     * Mark display content as updated.
     */
    lcd_display.refresh_required = 1U;

}

/* ==========================================================================
 * Floating Point Display Functions
 * ========================================================================== */


/**
 * @brief Display a floating-point value with a text label.
 *
 * @details
 * This function formats a numeric value and displays it on a selected
 * LCD row.
 *
 * It keeps number formatting inside the Display layer so application
 * modules do not need to handle:
 *
 *      - string conversion
 *      - LCD cursor positioning
 *      - text writing
 *
 *
 * Example:
 *
 *      Label:
 *          "Vin:"
 *
 *      Value:
 *          220.5
 *
 *      Result:
 *          Vin:220.5
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
void LCD_Display_PrintFloat(
        uint8_t row,
        const char *label,
        float value,
        uint8_t decimals)
{

    char buffer[32];



    /*
     * Validate LCD row index.
     */
    if(row >= LCD_DISPLAY_ROWS)
    {
        return;
    }



    /*
     * Validate text pointer.
     */
    if(label == NULL)
    {
        return;
    }



    /*
     * Convert floating-point value into text format.
     *
     * The resulting string is stored temporarily before being sent
     * to the LCD display.
     *
     * The precision parameter is currently limited by the project
     * display requirements.
     */
    snprintf(
            buffer,
            sizeof(buffer),
            "%s%.*f",
            label,
            decimals,
            value);



    /*
     * Display the formatted string.
     */
    LCD_Display_PrintLine(
            row,
            buffer);

}







/* ==========================================================================
 * Display Page Management
 * ========================================================================== */


/**
 * @brief Set the current active display page.
 *
 * @details
 * The Display layer stores the current logical page information.
 *
 * This information is used by higher-level modules such as Menu Manager
 * to track the active screen.
 *
 *
 * @param page
 *      New active display page.
 *
 */
void LCD_Display_SetPage(
        LCD_DisplayPage_t page)
{

    /*
     * Store the new page identifier.
     */
    lcd_display.current_page = page;



    /*
     * A page change requires a new display rendering.
     */
    lcd_display.refresh_required = 1U;

}






/**
 * @brief Get the current active display page.
 *
 * @return
 *      Current LCD display page identifier.
 *
 */
LCD_DisplayPage_t LCD_Display_GetPage(void)
{

    return lcd_display.current_page;

}

/* ==========================================================================
 * Refresh Management
 * ========================================================================== */


/**
 * @brief Request a display refresh.
 *
 * @details
 * The LCD should not be continuously rewritten because frequent updates
 * increase CPU usage and I2C traffic.
 *
 * Instead, the Display layer uses a refresh flag.
 *
 * When content changes, this function sets the flag and allows the main
 * application flow to decide when the actual update should happen.
 *
 */
void LCD_Display_RequestRefresh(void)
{

    /*
     * Mark display content as requiring an update.
     */
    lcd_display.refresh_required = 1U;

}






/**
 * @brief Check whether LCD refresh is required.
 *
 * @return
 *
 *      1 : Refresh is required.
 *
 *      0 : No refresh is required.
 *
 */
uint8_t LCD_Display_NeedRefresh(void)
{

    return lcd_display.refresh_required;

}






/**
 * @brief Clear the display refresh flag.
 *
 * @details
 * This function should be called after the display content has been
 * updated successfully.
 *
 */
void LCD_Display_ClearRefreshFlag(void)
{

    /*
     * Mark current display content as synchronized.
     */
    lcd_display.refresh_required = 0U;

}






/* ==========================================================================
 * LCD Backlight Control
 * ========================================================================== */


/**
 * @brief Turn LCD backlight on.
 *
 * @details
 * This function provides an application-level interface for controlling
 * LCD backlight without accessing the low-level LCD driver directly.
 *
 */
void LCD_Display_BacklightOn(void)
{

    LCD_BacklightOn();

}






/**
 * @brief Turn LCD backlight off.
 *
 * @details
 * This function disables the LCD backlight through the hardware driver.
 *
 */
void LCD_Display_BacklightOff(void)
{

    LCD_BacklightOff();

}






/* ==========================================================================
 * End of File
 * ========================================================================== */


/**
 * @brief
 * End of lcd_display.c
 *
 * @note
 * This module provides the application-level display interface.
 *
 * Hardware-specific LCD communication remains isolated inside:
 *
 *      lcd_i2c.c
 *
 * Maintaining this separation allows future display hardware changes
 * without modifying application modules.
 *
 */
