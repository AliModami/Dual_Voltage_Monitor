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
 * Description:
 *
 *      This module provides the application display abstraction layer.
 *
 *      Responsibilities:
 *
 *      - Initialize LCD display layer.
 *      - Display application text.
 *      - Render page based menu.
 *      - Render parameter edit screen.
 *
 *
 *      This module does NOT handle:
 *
 *      - Button input.
 *      - Menu navigation.
 *      - Menu logic.
 *      - I2C communication.
 *      - HD44780 low level commands.
 *
 *
 * Architecture:
 *
 *
 *      menu_controller
 *             |
 *             v
 *
 *      menu_renderer
 *             |
 *             v
 *
 *      lcd_display
 *             |
 *             v
 *
 *      lcd_i2c
 *
 *
 ******************************************************************************/

#include "lcd_display.h"

#include "lcd_i2c.h"
#include "menu_renderer.h"
#include "menu_edit.h"

#include <string.h>


/*
 * ============================================================================
 * Private Definitions
 * ============================================================================
 */


/*
 * Cursor character used for selected menu item.
 */
#define LCD_MENU_CURSOR_CHAR     '>'



/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


/*
 * LCD initialization state.
 *
 * 0:
 *      Not initialized
 *
 * 1:
 *      Ready
 */
static uint8_t lcd_display_initialized = 0U;

static char lcd_line_cache[LCD_DEFAULT_ROWS][LCD_DEFAULT_COLUMNS + 1];

/*
 * ============================================================================
 * Private Function Prototypes
 * ============================================================================
 */


/*
 * Write complete LCD row.
 *
 * Remaining characters are filled
 * with spaces to remove old content.
 */
static void LCD_Display_WriteRow(
        uint8_t row,
        const char *text);



/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */


/*
 * Initialize LCD display layer.
 */
void LCD_Display_Init(
        I2C_HandleTypeDef *hi2c)
{

    if(LCD_Init(
            hi2c,
            LCD_I2C_ADDRESS_DEFAULT,
            LCD_DEFAULT_COLUMNS,
            LCD_DEFAULT_ROWS) == HAL_OK)
    {

        lcd_display_initialized = 1U;


        LCD_Clear();

        memset(lcd_line_cache,
               0,
               sizeof(lcd_line_cache));



        /*
         * HD44780 clear command delay.
         */
        HAL_Delay(5U);

    }
    else
    {

        lcd_display_initialized = 0U;

    }

}




/*
 * Write one complete LCD row.
 */
static void LCD_Display_WriteRow(
        uint8_t row,
        const char *text)
{

    char new_line[LCD_DEFAULT_COLUMNS + 1];


    uint8_t index = 0U;



    if(text == NULL)
    {
        return;
    }



    if(row >= LCD_DEFAULT_ROWS)
    {
        return;
    }



    /*
     * Create fixed length LCD line.
     */
    while((text[index] != '\0') &&
          (index < LCD_DEFAULT_COLUMNS))
    {
        new_line[index] = text[index];

        index++;
    }



    while(index < LCD_DEFAULT_COLUMNS)
    {
        new_line[index] = ' ';

        index++;
    }



    new_line[LCD_DEFAULT_COLUMNS] = '\0';



    /*
     * Skip identical update.
     */
    if(strcmp(lcd_line_cache[row],
              new_line) == 0)
    {
        return;
    }



    strcpy(lcd_line_cache[row],
           new_line);



    LCD_SetCursor(
            0U,
            row);



    for(index = 0U;
        index < LCD_DEFAULT_COLUMNS;
        index++)
    {

        LCD_PrintChar(
                new_line[index]);

    }

}


/*
 * Display title line.
 */
void LCD_Display_ShowTitle(
        const char *title)
{

    if(lcd_display_initialized == 0U)
    {
        return;
    }



    LCD_Display_WriteRow(
            0U,
            title);

}




/*
 * Display text on selected row.
 */
void LCD_Display_PrintLine(
        uint8_t row,
        const char *text)
{

    if(lcd_display_initialized == 0U)
    {
        return;
    }



    if(row >= LCD_DEFAULT_ROWS)
    {
        return;
    }



    LCD_Display_WriteRow(
            row,
            text);

}




/*
 * Clear LCD display.
 */
void LCD_Display_Clear(void)
{

    if(lcd_display_initialized == 0U)
    {
        return;
    }



    LCD_Clear();


    HAL_Delay(5U);

}


/*
 * ============================================================================
 * Menu Rendering
 * ============================================================================
 */


/*
 * Render complete menu page.
 *
 * Data source:
 *
 *      MenuRenderer_GetData()
 *
 *
 * LCD Layout:
 *
 *      Row 0:
 *          Page title
 *
 *      Row 1-3:
 *          Menu items
 *
 */
void LCD_Display_RenderMenu(void)
{

    const MenuRenderData_t *menu;



    if(lcd_display_initialized == 0U)
    {
        return;
    }



    menu = MenuRenderer_GetData();



    if(menu == NULL)
    {
        return;
    }




    /*
     * ------------------------------------------------------------------------
     * Parameter Edit Screen
     * ------------------------------------------------------------------------
     */
    if(menu->mode == MENU_RENDER_MODE_EDIT)
    {

        char value_line[21];



        /*
         * Row 0:
         * Edit title
         */
        LCD_Display_ShowTitle(
                menu->edit_title);



        /*
         * Convert internal value
         * to display text.
         *
         * This avoids float printf
         * dependency.
         */
        MenuEdit_GetDisplayString(
                value_line,
                sizeof(value_line));



        /*
         * Row 1:
         * Current value
         */
        LCD_Display_PrintLine(
                1U,
                value_line);



        /*
         * Row 2:
         * Empty line
         */
        LCD_Display_PrintLine(
                2U,
                "");



        /*
         * Row 3:
         * User controls
         */
        LCD_Display_PrintLine(
                3U,
                "UP/DN ENTER=SAVE");



        return;

    }





    /*
     * ------------------------------------------------------------------------
     * Normal Menu Page
     * ------------------------------------------------------------------------
     */



    /*
     * Row 0:
     * Page title
     */
    LCD_Display_WriteRow(
            0U,
            menu->title);





    /*
     * Rows 1-3:
     *
     * Visible menu items
     */
    for(uint8_t row = 0U;
        row < MENU_RENDER_VISIBLE_ITEMS;
        row++)
    {

        char line[21];

        uint8_t index = 0U;



        /*
         * Initialize line buffer.
         */
        for(uint8_t i = 0U;
            i < sizeof(line);
            i++)
        {

            line[i] = '\0';

        }





        /*
         * Add cursor.
         */
        if(row == menu->cursor_position)
        {

            line[index++] =
                    LCD_MENU_CURSOR_CHAR;


            line[index++] =
                    ' ';

        }
        else
        {

            line[index++] =
                    ' ';


            line[index++] =
                    ' ';

        }





        /*
         * Add menu item text.
         */
        if(row < menu->item_count)
        {

            const char *item;


            item = menu->items[row];



            if(item != NULL)
            {

                uint8_t item_index = 0U;



                while((item[item_index] != '\0') &&
                      (index < LCD_DEFAULT_COLUMNS))
                {

                    line[index] =
                            item[item_index];


                    index++;

                    item_index++;

                }

            }

        }





        /*
         * Write LCD row.
         *
         * Menu rows:
         *
         *      1 ... 3
         */
        LCD_Display_WriteRow(
                row + 1U,
                line);

    }

}

/*
 * ============================================================================
 * Display Utility Functions
 * ============================================================================
 */


/*
 * The display layer intentionally keeps
 * no application state.
 *
 * All displayed information is received
 * from:
 *
 *      menu_renderer
 *
 * or directly from application modules
 * through:
 *
 *      LCD_Display_PrintLine()
 *
 */



/*
 * ============================================================================
 * Internal Design Notes
 * ============================================================================
 */


/*
 * LCD_Display_RenderMenu()
 *
 * Rendering flow:
 *
 *
 *      menu_renderer
 *
 *          |
 *          v
 *
 *      MenuRenderData_t
 *
 *          |
 *          v
 *
 *      LCD_Display_RenderMenu()
 *
 *          |
 *          v
 *
 *      LCD_Display_WriteRow()
 *
 *          |
 *          v
 *
 *      lcd_i2c driver
 *
 *
 *
 * The display layer never changes
 * menu state.
 */



/*
 * ============================================================================
 * Flicker Prevention Rules
 * ============================================================================
 */


/*
 * This module does not call:
 *
 *      LCD_Clear()
 *
 * during normal rendering.
 *
 * Only modified rows are rewritten.
 *
 * This prevents:
 *
 *      - Visible blinking
 *      - Slow redraw
 *      - Cursor flicker
 *
 */



/*
 * ============================================================================
 * LCD Layout Rules
 * ============================================================================
 */


/*
 * Character LCD 20x4:
 *
 *
 * +--------------------+
 * | Title              |
 * +--------------------+
 * | Item 1             |
 * +--------------------+
 * | Item 2             |
 * +--------------------+
 * | Item 3             |
 * +--------------------+
 *
 *
 *
 * Edit Mode:
 *
 *
 * +--------------------+
 * | Edit Parameter     |
 * +--------------------+
 * | Current Value      |
 * +--------------------+
 * |                    |
 * +--------------------+
 * | Controls           |
 * +--------------------+
 *
 */



/*
 * ============================================================================
 * Compatibility Information
 * ============================================================================
 */


/*
 * Compatible with:
 *
 *      menu_renderer.h
 *      menu_edit.h
 *      lcd_i2c.h
 *      config.h
 *
 *
 * Does not depend on:
 *
 *      menu_controller internals
 *      button driver
 *      ADC driver
 *      UART driver
 *
 */



/*
 * ============================================================================
 * End Of File
 * ============================================================================
 */

/******************************************************************************
 *
 *                              END OF FILE
 *
 *
 *      lcd_display.c
 *
 *
 *      Version:
 *
 *          Clean Final v1.0.0
 *
 *
 *      Changes:
 *
 *          - Removed legacy edit rendering code.
 *          - Removed float formatting dependency.
 *          - Added MenuEdit_GetDisplayString() support.
 *          - Preserved Page Based Menu rendering.
 *          - Preserved LCD row based update.
 *          - Removed unnecessary LCD context exposure.
 *          - Improved separation between display and logic layers.
 *
 *
 *      Architecture:
 *
 *
 *          menu_controller
 *                 |
 *                 v
 *
 *          menu_renderer
 *                 |
 *                 v
 *
 *          lcd_display
 *                 |
 *                 v
 *
 *          lcd_i2c
 *
 *
 *
 *      Hardware:
 *
 *          MCU:
 *              STM32F103C8T6
 *
 *          Display:
 *              HD44780 Character LCD
 *
 *          Interface:
 *              PCF8574 I2C Backpack
 *
 *
 ******************************************************************************/
