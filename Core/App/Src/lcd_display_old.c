/******************************************************************************
 *
 * @file    lcd_display.c
 *
 * @brief   LCD Display Abstraction Layer Implementation
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU:
 *
 *      STM32F103C8T6
 *
 *------------------------------------------------------------------------------
 *
 * Framework:
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This module provides the display abstraction layer between
 *      application modules and the low level LCD hardware driver.
 *
 *      Application modules should use this interface instead of
 *      accessing lcd_i2c.c directly.
 *
 *
 * Architecture:
 *
 *
 *      Application Layer
 *
 *          menu_renderer.c
 *          monitor.c
 *          stream.c
 *
 *                  |
 *                  v
 *
 *          lcd_display.c
 *
 *                  |
 *                  v
 *
 *          lcd_i2c.c
 *
 *                  |
 *                  v
 *
 *          PCF8574 + HD44780 LCD
 *
 *
 *------------------------------------------------------------------------------
 *
 * Responsibilities:
 *
 *      - Manage display state
 *      - Provide formatted text output
 *      - Handle LCD page information
 *      - Control refresh requests
 *      - Hide hardware implementation details
 *
 *
 * This module does NOT contain:
 *
 *      - Menu navigation logic
 *      - Button handling
 *      - Measurement processing
 *      - Alarm logic
 *
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      1.0.0 Clean Final
 *
 *------------------------------------------------------------------------------
 *
 ******************************************************************************/

/* -------------------------------------------------------------------------- */
/* Includes                                                                   */
/* -------------------------------------------------------------------------- */

#include "lcd_display.h"

#include "lcd_i2c.h"

#include <stdio.h>
#include <string.h>





/* -------------------------------------------------------------------------- */
/* Private Constants                                                          */
/* -------------------------------------------------------------------------- */


/**
 * @brief
 * Default LCD geometry.
 *
 * These values must match the physical LCD module.
 */
#define LCD_DISPLAY_DEFAULT_ADDRESS       LCD_I2C_ADDRESS_DEFAULT





/**
 * @brief
 * Maximum temporary formatting buffer.
 */
#define LCD_DISPLAY_BUFFER_SIZE           64U





/* -------------------------------------------------------------------------- */
/* Private Variables                                                          */
/* -------------------------------------------------------------------------- */


/**
 * @brief
 * Internal display module handle.
 *
 * The module is designed for one LCD instance.
 */
static LCD_DisplayHandle_t lcd_display_handle;





/**
 * @brief
 * Temporary formatting buffer.
 *
 * Used internally for:
 *
 *      - Number formatting
 *      - Floating point formatting
 *      - Line preparation
 */
static char lcd_display_buffer[LCD_DISPLAY_BUFFER_SIZE];






/* -------------------------------------------------------------------------- */
/* Private Function Prototypes                                                */
/* -------------------------------------------------------------------------- */


/**
 * @brief
 * Write one complete LCD row.
 *
 * @param row
 *      LCD row index.
 *
 * @param text
 *      Text to display.
 *
 */
static void LCD_Display_WriteRow(
        uint8_t row,
        const char *text);




/**
 * @brief
 * Limit text length according to LCD width.
 *
 * @param text
 *      Source string.
 *
 * @param destination
 *      Output buffer.
 *
 */
static void LCD_Display_FormatLine(
        const char *text,
        char *destination);





/**
 * @brief
 * Check whether display module is initialized.
 *
 * @return
 *
 *      1 : initialized
 *
 *      0 : not initialized
 *
 */
static uint8_t LCD_Display_IsInitialized(void);






/* ========================================================================== */
/* Initialization                                                             */
/* ========================================================================== */


/**
 * @brief
 * Initialize LCD display abstraction layer.
 *
 * @details
 *
 * Initialization sequence:
 *
 *      1. Store I2C handle.
 *
 *      2. Initialize low level LCD driver.
 *
 *      3. Enable backlight.
 *
 *      4. Clear LCD.
 *
 *      5. Initialize internal display state.
 *
 *
 * @param hi2c
 *      STM32 HAL I2C handle.
 *
 */
void LCD_Display_Init(
        I2C_HandleTypeDef *hi2c)
{


    /*
     * Clear internal state first.
     */
    memset(
            &lcd_display_handle,
            0,
            sizeof(lcd_display_handle));



    /*
     * Store I2C peripheral handle.
     */
    lcd_display_handle.hi2c =
            hi2c;



    /*
     * Initialize low level LCD hardware.
     *
     * Default project hardware:
     *
     *      LCD:
     *          20x4
     *
     *      Address:
     *          0x27
     */
    LCD_Init(
            hi2c,
            LCD_DISPLAY_DEFAULT_ADDRESS,
            LCD_DISPLAY_COLS,
            LCD_DISPLAY_ROWS);



    /*
     * Enable LCD backlight.
     */
    LCD_BacklightOn();



    /*
     * Clear physical display.
     */
    LCD_Clear();



    /*
     * Prepare initial display state.
     */
    lcd_display_handle.current_page =
            LCD_PAGE_NONE;



    lcd_display_handle.refresh_required =
            1U;



    /*
     * Initial title.
     */
    strcpy(
            lcd_display_handle.title,
            "");

}



/* -------------------------------------------------------------------------- */
/* End of Part 1/8                                                            */
/* -------------------------------------------------------------------------- */



/* ========================================================================== */
/* Basic Display Operations                                                   */
/* ========================================================================== */


/**
 * @brief
 * Clear complete LCD display.
 *
 * @details
 *
 * This function clears the physical LCD through
 * the low level LCD driver.
 *
 * The internal display state is preserved.
 *
 */
void LCD_Display_Clear(void)
{


    /*
     * Clear hardware display.
     */
    LCD_Clear();



    /*
     * After clearing the LCD, visible content
     * no longer represents the previous state.
     */
    lcd_display_handle.refresh_required =
            1U;

}






/**
 * @brief
 * Display page title.
 *
 * @details
 *
 * The title is stored internally and displayed
 * on the first LCD row.
 *
 *
 * @param title
 *      Title string.
 *
 */
void LCD_Display_ShowTitle(
        const char *title)
{


    /*
     * Ignore invalid input.
     */
    if(title == NULL)
    {
        return;
    }



    /*
     * Store title internally.
     */
    strncpy(
            lcd_display_handle.title,
            title,
            LCD_DISPLAY_TITLE_SIZE - 1U);



    /*
     * Ensure string termination.
     */
    lcd_display_handle.title[LCD_DISPLAY_TITLE_SIZE - 1U]
            =
            '\0';




    /*
     * Write title line.
     */
    LCD_Display_PrintLine(
            0U,
            lcd_display_handle.title);

}






/**
 * @brief
 * Print text on selected LCD row.
 *
 * @details
 *
 * The selected row is cleared before writing.
 *
 * This avoids remaining characters when the
 * new text is shorter than the previous text.
 *
 *
 * @param row
 *      LCD row index.
 *
 * @param text
 *      Text to display.
 *
 */
void LCD_Display_PrintLine(
        uint8_t row,
        const char *text)
{


    /*
     * Validate input string.
     */
    if(text == NULL)
    {
        return;
    }



    /*
     * Write formatted line.
     */
    LCD_Display_WriteRow(
            row,
            text);



    /*
     * Visible content changed.
     */
    lcd_display_handle.refresh_required =
            0U;

}







/**
 * @brief
 * Display floating point value with label.
 *
 * @details
 *
 * Example:
 *
 *      label = "Vin"
 *
 *      value = 220.5
 *
 *
 * Result:
 *
 *      Vin:220.5
 *
 *
 * @param row
 *      LCD row index.
 *
 * @param label
 *      Text label.
 *
 * @param value
 *      Floating point value.
 *
 * @param decimals
 *      Decimal digits.
 *
 */
void LCD_Display_PrintFloat(
        uint8_t row,
        const char *label,
        float value,
        uint8_t decimals)
{


    /*
     * Validate label pointer.
     */
    if(label == NULL)
    {
        return;
    }



    /*
     * Create formatted text.
     */
    snprintf(
            lcd_display_buffer,
            sizeof(lcd_display_buffer),
            "%s:%.*f",
            label,
            decimals,
            value);




    /*
     * Display generated text.
     */
    LCD_Display_PrintLine(
            row,
            lcd_display_buffer);

}








/* ========================================================================== */
/* Display Page Management                                                   */
/* ========================================================================== */


/**
 * @brief
 * Set current display page.
 *
 * @param page
 *      New page identifier.
 *
 */
void LCD_Display_SetPage(
        LCD_DisplayPage_t page)
{


    /*
     * Store active page.
     */
    lcd_display_handle.current_page =
            page;



    /*
     * Request redraw by application.
     */
    lcd_display_handle.refresh_required =
            1U;

}







/**
 * @brief
 * Get current display page.
 *
 * @return
 *
 *      Active display page.
 *
 */
LCD_DisplayPage_t LCD_Display_GetPage(void)
{


    return lcd_display_handle.current_page;


}






/* -------------------------------------------------------------------------- */
/* End of Part 2/8                                                            */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */
/* Refresh Management                                                        */
/* ========================================================================== */


/**
 * @brief
 * Request display refresh.
 *
 * @details
 *
 * This function does not update the LCD immediately.
 *
 * It only marks the display state as invalid.
 *
 * The upper application layer can then decide
 * when the next refresh should occur.
 *
 */
void LCD_Display_RequestRefresh(void)
{


    /*
     * Mark display as requiring update.
     */
    lcd_display_handle.refresh_required =
            1U;

}






/**
 * @brief
 * Check refresh request status.
 *
 * @return
 *
 *      1 : Refresh required
 *
 *      0 : No refresh required
 *
 */
uint8_t LCD_Display_NeedRefresh(void)
{


    return lcd_display_handle.refresh_required;


}







/**
 * @brief
 * Clear refresh request flag.
 *
 * @details
 *
 * Called after the display content has been
 * updated successfully.
 *
 */
void LCD_Display_ClearRefreshFlag(void)
{


    /*
     * Display content is synchronized.
     */
    lcd_display_handle.refresh_required =
            0U;

}







/* ========================================================================== */
/* LCD Backlight Control                                                     */
/* ========================================================================== */


/**
 * @brief
 * Enable LCD backlight.
 *
 */
void LCD_Display_BacklightOn(void)
{


    /*
     * Forward request to hardware driver.
     */
    LCD_BacklightOn();

}







/**
 * @brief
 * Disable LCD backlight.
 *
 */
void LCD_Display_BacklightOff(void)
{


    /*
     * Forward request to hardware driver.
     */
    LCD_BacklightOff();

}







/* ========================================================================== */
/* Handle Access                                                             */
/* ========================================================================== */


/**
 * @brief
 * Get LCD display handle.
 *
 * @details
 *
 * Provides controlled access to the internal
 * display state structure.
 *
 *
 * Application modules should normally use
 * the public API instead of modifying the
 * returned structure directly.
 *
 *
 * @return
 *
 *      Pointer to display handle.
 *
 */
LCD_DisplayHandle_t *LCD_Display_GetHandle(void)
{


    return &lcd_display_handle;


}







/* ========================================================================== */
/* Private Helper Functions                                                  */
/* ========================================================================== */


/**
 * @brief
 * Write complete LCD row.
 *
 * @details
 *
 * The function:
 *
 *      1. Clears the selected row.
 *
 *      2. Formats text according to LCD width.
 *
 *      3. Writes the new content.
 *
 *
 * @param row
 *      LCD row index.
 *
 * @param text
 *      Text to display.
 *
 */
static void LCD_Display_WriteRow(
        uint8_t row,
        const char *text)
{


    char line[LCD_DISPLAY_COLS + 1U];



    /*
     * Validate row index.
     */
    if(row >= LCD_DISPLAY_ROWS)
    {
        return;
    }



    /*
     * Prepare fixed length LCD line.
     */
    LCD_Display_FormatLine(
            text,
            line);




    /*
     * Clear old row content.
     */
    LCD_ClearRow(
            row);




    /*
     * Position cursor at row beginning.
     */
    LCD_SetCursor(
            0U,
            row);




    /*
     * Write new row content.
     */
    LCD_Print(
            line);


}







/**
 * @brief
 * Format text for LCD row.
 *
 * @details
 *
 * Character LCDs have fixed width.
 *
 * This helper:
 *
 *      - limits excessive text length
 *      - pads unused characters with spaces
 *
 *
 * @param text
 *      Source text.
 *
 * @param destination
 *      Output line buffer.
 *
 */
static void LCD_Display_FormatLine(
        const char *text,
        char *destination)
{


    uint8_t index = 0U;



    /*
     * Protect against NULL pointer.
     */
    if(text == NULL || destination == NULL)
    {
        return;
    }



    /*
     * Copy characters until:
     *
     *      - End of string
     *      - LCD width reached
     */
    while((text[index] != '\0') &&
          (index < LCD_DISPLAY_COLS))
    {

        destination[index] =
                text[index];

        index++;

    }




    /*
     * Fill remaining characters.
     */
    while(index < LCD_DISPLAY_COLS)
    {

        destination[index] =
                ' ';

        index++;

    }




    /*
     * Terminate string.
     */
    destination[LCD_DISPLAY_COLS] =
            '\0';

}







/* -------------------------------------------------------------------------- */
/* End of Part 3/8                                                            */
/* -------------------------------------------------------------------------- */


/* ========================================================================== */
/* Private State Validation                                                  */
/* ========================================================================== */


/**
 * @brief
 * Check display initialization state.
 *
 * @details
 *
 * The display layer is considered initialized
 * when a valid I2C handle has been stored.
 *
 * This function prevents calling lower-level
 * LCD functions before initialization.
 *
 *
 * @return
 *
 *      1 : Display initialized
 *
 *      0 : Display not initialized
 *
 */
static uint8_t LCD_Display_IsInitialized(void)
{


    /*
     * Check stored I2C handle.
     */
    if(lcd_display_handle.hi2c != NULL)
    {

        return 1U;

    }



    return 0U;

}








/* ========================================================================== */
/* Extended Display Utility Functions                                         */
/* ========================================================================== */


/**
 * @brief
 * Internal display state synchronization helper.
 *
 * @details
 *
 * This function updates the refresh state after
 * successful display operations.
 *
 * It is intentionally kept private because the
 * application layer should not control internal
 * refresh management directly.
 *
 */
static void LCD_Display_UpdateState(void)
{


    /*
     * Current visible LCD content is valid.
     */
    lcd_display_handle.refresh_required =
            0U;

}







/* ========================================================================== */
/* Additional Display Wrappers                                                */
/* ========================================================================== */


/**
 * @brief
 * Display integer value on selected LCD row.
 *
 * @details
 *
 * This helper is kept private because the public
 * interface currently exposes only floating point
 * formatted values.
 *
 * It may be used internally in future versions.
 *
 * @param row
 *      LCD row index.
 *
 * @param label
 *      Text label.
 *
 * @param value
 *      Integer value.
 *
 */
static void LCD_Display_PrintInteger(
        uint8_t row,
        const char *label,
        int32_t value)
{


    /*
     * Validate label.
     */
    if(label == NULL)
    {
        return;
    }




    /*
     * Create formatted string.
     */
    snprintf(
            lcd_display_buffer,
            sizeof(lcd_display_buffer),
            "%s:%ld",
            label,
            value);




    /*
     * Output generated text.
     */
    LCD_Display_PrintLine(
            row,
            lcd_display_buffer);

}








/* ========================================================================== */
/* Display Lifecycle Management                                               */
/* ========================================================================== */


/**
 * @brief
 * Reset display module state.
 *
 * @details
 *
 * This function resets only the abstraction
 * layer state.
 *
 * The hardware driver state remains unchanged.
 *
 *
 * This is useful when:
 *
 *      - Application restarts display handling
 *      - A new screen system takes ownership
 *
 */
static void LCD_Display_ResetContext(void)
{


    /*
     * Clear title buffer.
     */
    memset(
            lcd_display_handle.title,
            0,
            sizeof(lcd_display_handle.title));




    /*
     * Reset page state.
     */
    lcd_display_handle.current_page =
            LCD_PAGE_NONE;




    /*
     * Force next refresh.
     */
    lcd_display_handle.refresh_required =
            1U;

}







/* ========================================================================== */
/* Compatibility Notes                                                        */
/* ========================================================================== */


/*
 *
 * This implementation intentionally keeps the
 * Display abstraction layer independent from:
 *
 *      - menu_engine.c
 *
 *      - menu_renderer.c
 *
 *      - monitor.c
 *
 *      - stream.c
 *
 *
 * The module provides only:
 *
 *
 *      LCD initialization
 *
 *      LCD text output
 *
 *      LCD page state management
 *
 *      LCD refresh flag handling
 *
 *      LCD backlight control
 *
 *
 *
 * Hardware access is performed only through:
 *
 *
 *      lcd_i2c.c
 *
 *
 */






/* -------------------------------------------------------------------------- */
/* End of Part 4/8                                                            */
/* -------------------------------------------------------------------------- */


/* ========================================================================== */
/* Display Page Helper Functions                                              */
/* ========================================================================== */


/**
 * @brief
 * Check current active page.
 *
 * @details
 *
 * This private helper provides a centralized
 * method for page validation.
 *
 * Future display pages can extend this logic
 * without changing application modules.
 *
 * @param page
 *      Display page identifier.
 *
 * @return
 *
 *      1 : Valid page
 *
 *      0 : Invalid page
 *
 */
static uint8_t LCD_Display_IsValidPage(
        LCD_DisplayPage_t page)
{


    switch(page)
    {

        case LCD_PAGE_NONE:

        case LCD_PAGE_HOME:

        case LCD_PAGE_MONITOR:

        case LCD_PAGE_STREAM:

        case LCD_PAGE_SETTINGS:

        case LCD_PAGE_SYSTEM_INFO:

            return 1U;


        default:

            return 0U;

    }

}







/**
 * @brief
 * Store current page information.
 *
 * @details
 *
 * Internal helper used to keep page changes
 * consistent.
 *
 * @param page
 *      New display page.
 *
 */
static void LCD_Display_UpdatePage(
        LCD_DisplayPage_t page)
{


    /*
     * Ignore invalid page values.
     */
    if(LCD_Display_IsValidPage(page) == 0U)
    {
        return;
    }



    /*
     * Store new page.
     */
    lcd_display_handle.current_page =
            page;



    /*
     * Request redraw.
     */
    LCD_Display_RequestRefresh();

}








/* ========================================================================== */
/* Display Text Processing                                                   */
/* ========================================================================== */


/**
 * @brief
 * Copy display text safely.
 *
 * @details
 *
 * This helper prevents buffer overflow while
 * preparing internal display strings.
 *
 * @param destination
 *      Destination buffer.
 *
 * @param source
 *      Source text.
 *
 * @param size
 *      Destination size.
 *
 */
static void LCD_Display_CopyText(
        char *destination,
        const char *source,
        uint16_t size)
{


    /*
     * Validate pointers.
     */
    if((destination == NULL) ||
       (source == NULL) ||
       (size == 0U))
    {
        return;
    }



    /*
     * Copy limited text.
     */
    strncpy(
            destination,
            source,
            size - 1U);



    /*
     * Guarantee termination.
     */
    destination[size - 1U] =
            '\0';

}







/* ========================================================================== */
/* End of Part 5/8                                                            */
/* ========================================================================== */


/* ========================================================================== */
/* Display Diagnostic Functions                                                */
/* ========================================================================== */


/**
 * @brief
 * Prepare display status information.
 *
 * @details
 *
 * This private function prepares diagnostic
 * information that can be used by future
 * system information pages.
 *
 * The function does not directly write to LCD.
 *
 */
static void LCD_Display_PrepareStatus(void)
{


    /*
     * Clear temporary buffer.
     */
    memset(
            lcd_display_buffer,
            0,
            sizeof(lcd_display_buffer));



    /*
     * Store basic display status.
     */
    snprintf(
            lcd_display_buffer,
            sizeof(lcd_display_buffer),
            "LCD:%ux%u",
            LCD_DISPLAY_COLS,
            LCD_DISPLAY_ROWS);

}








/* ========================================================================== */
/* Display Initialization State                                               */
/* ========================================================================== */


/**
 * @brief
 * Verify display module before operation.
 *
 * @details
 *
 * This function is used internally before
 * performing display operations that require
 * initialized hardware.
 *
 *
 * @return
 *
 *      1 : Ready
 *
 *      0 : Not ready
 *
 */
static uint8_t LCD_Display_CheckReady(void)
{


    /*
     * Check abstraction layer state.
     */
    if(LCD_Display_IsInitialized() == 0U)
    {

        return 0U;

    }



    /*
     * Check low level LCD driver state.
     */
    if(LCD_IsReady() == 0U)
    {

        return 0U;

    }



    return 1U;

}








/* ========================================================================== */
/* Display Protected Operations                                               */
/* ========================================================================== */


/**
 * @brief
 * Safely clear one display line.
 *
 * @details
 *
 * This helper prevents direct hardware access
 * when display initialization has not completed.
 *
 * @param row
 *      LCD row number.
 *
 */
static void LCD_Display_SafeClearRow(
        uint8_t row)
{


    /*
     * Validate display state.
     */
    if(LCD_Display_CheckReady() == 0U)
    {
        return;
    }



    /*
     * Clear selected LCD row.
     */
    LCD_ClearRow(row);

}








/**
 * @brief
 * Safely position LCD cursor.
 *
 * @param column
 *      Cursor column.
 *
 * @param row
 *      Cursor row.
 *
 */
static void LCD_Display_SafeSetCursor(
        uint8_t column,
        uint8_t row)
{


    /*
     * Validate display state.
     */
    if(LCD_Display_CheckReady() == 0U)
    {
        return;
    }



    /*
     * Move LCD cursor.
     */
    LCD_SetCursor(
            column,
            row);

}








/* -------------------------------------------------------------------------- */
/* End of Part 6/8                                                            */
/* -------------------------------------------------------------------------- */

/* ========================================================================== */
/* Display API Support Functions                                              */
/* ========================================================================== */


/**
 * @brief
 * Update display page and title together.
 *
 * @details
 *
 * This helper keeps page information and title
 * synchronized.
 *
 * It is intended for future internal use when
 * page based rendering is expanded.
 *
 * @param page
 *      Display page identifier.
 *
 * @param title
 *      Page title text.
 *
 */
static void LCD_Display_SetPageTitle(
        LCD_DisplayPage_t page,
        const char *title)
{


    /*
     * Update page information.
     */
    LCD_Display_UpdatePage(page);



    /*
     * Update title if available.
     */
    if(title != NULL)
    {

        LCD_Display_CopyText(
                lcd_display_handle.title,
                title,
                LCD_DISPLAY_TITLE_SIZE);

    }



}








/**
 * @brief
 * Prepare empty display line.
 *
 * @details
 *
 * Creates a blank LCD row buffer.
 *
 * This is used internally to avoid repeated
 * space filling code.
 *
 * @param buffer
 *      Output buffer.
 *
 */
static void LCD_Display_CreateEmptyLine(
        char *buffer)
{


    uint8_t index;



    /*
     * Validate pointer.
     */
    if(buffer == NULL)
    {
        return;
    }



    /*
     * Fill line with spaces.
     */
    for(index = 0U;
        index < LCD_DISPLAY_COLS;
        index++)
    {

        buffer[index] =
                ' ';

    }



    /*
     * Add string termination.
     */
    buffer[LCD_DISPLAY_COLS] =
            '\0';

}







/* ========================================================================== */
/* Display Shutdown Support                                                  */
/* ========================================================================== */


/**
 * @brief
 * Internal display shutdown preparation.
 *
 * @details
 *
 * This function only changes the abstraction
 * layer state.
 *
 * It does not disable the LCD hardware.
 *
 * Hardware shutdown, if required, should be
 * handled by the hardware driver layer.
 *
 */
static void LCD_Display_PrepareShutdown(void)
{


    /*
     * Clear current page.
     */
    lcd_display_handle.current_page =
            LCD_PAGE_NONE;



    /*
     * Force refresh after restart.
     */
    lcd_display_handle.refresh_required =
            1U;

}







/* ========================================================================== */
/* Final Compatibility Layer                                                 */
/* ========================================================================== */


/*
 *
 * Public API implemented by this module:
 *
 *
 * Initialization:
 *
 *      LCD_Display_Init()
 *
 *
 * Basic display:
 *
 *      LCD_Display_Clear()
 *
 *      LCD_Display_ShowTitle()
 *
 *      LCD_Display_PrintLine()
 *
 *      LCD_Display_PrintFloat()
 *
 *
 * Page management:
 *
 *      LCD_Display_SetPage()
 *
 *      LCD_Display_GetPage()
 *
 *
 * Refresh:
 *
 *      LCD_Display_RequestRefresh()
 *
 *      LCD_Display_NeedRefresh()
 *
 *      LCD_Display_ClearRefreshFlag()
 *
 *
 * Backlight:
 *
 *      LCD_Display_BacklightOn()
 *
 *      LCD_Display_BacklightOff()
 *
 *
 * Handle:
 *
 *      LCD_Display_GetHandle()
 *
 *
 *
 * Hardware access remains inside:
 *
 *      lcd_i2c.c
 *
 *
 */




/* -------------------------------------------------------------------------- */
/* End of Part 7/8                                                            */
/* -------------------------------------------------------------------------- */


/* ========================================================================== */
/* End Of Module Validation                                                  */
/* ========================================================================== */


/**
 * @brief
 * Final internal consistency check.
 *
 * @details
 *
 * This section intentionally contains no public
 * API additions.
 *
 * All hardware operations are routed through:
 *
 *      lcd_i2c.c
 *
 * The Display layer remains responsible only for:
 *
 *      - Display state management
 *      - Text formatting
 *      - Page information
 *      - Refresh control
 *
 */


/* ========================================================================== */
/* End Of File                                                                */
/* ========================================================================== */


/******************************************************************************
 *
 *                      END OF FILE
 *
 *
 *      lcd_display.c
 *
 *
 *      STM32F103C8T6
 *
 *              +
 *
 *      LCD Display Abstraction Layer
 *
 *              +
 *
 *      lcd_i2c Hardware Driver
 *
 *
 ******************************************************************************/
