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
 *      This module provides the application display interface.
 *
 *      It connects:
 *
 *
 *          Menu Renderer
 *                |
 *                v
 *
 *          LCD Display Layer
 *                |
 *                v
 *
 *          LCD I2C Driver
 *
 *
 *
 *      Responsibilities:
 *
 *      - Initialize display system.
 *      - Display title lines.
 *      - Display text lines.
 *      - Render page based menu.
 *
 *
 *      This module does not handle:
 *
 *      - Button processing
 *      - Menu navigation
 *      - Application logic
 *
 *
 ******************************************************************************/

#include "lcd_display.h"
#include "lcd_i2c.h"
#include "menu_renderer.h"
#include "menu_edit.h"
#include <stdio.h>

/*
 * --------------------------------------------------------------------------
 * Private definitions
 * --------------------------------------------------------------------------
 */


/*
 * LCD cursor symbol.
 *
 * Character LCD does not support
 * Unicode arrows by default.
 *
 * Therefore simple ASCII cursor
 * is used.
 */
#define LCD_MENU_CURSOR_CHAR       '>'




/*
 * --------------------------------------------------------------------------
 * Private variables
 * --------------------------------------------------------------------------
 */


/*
 * Display initialization status.
 *
 * 0:
 *      LCD not ready
 *
 * 1:
 *      LCD initialized
 */
static uint8_t lcd_display_initialized = 0U;




/*
 * --------------------------------------------------------------------------
 * Private helper functions
 * --------------------------------------------------------------------------
 */


/*
 * Write one complete LCD row.
 *
 * Character LCD keeps old characters
 * if the new text is shorter.
 *
 * Example:
 *
 * Previous:
 *
 *      Live Monitor
 *
 * New:
 *
 *      OK
 *
 * Without clearing:
 *
 *      OKve Monitor
 *
 * Therefore unused characters
 * are replaced with spaces.
 *
 */
static void LCD_Display_WriteRow(
        uint8_t row,
        const char *text);



/*
 * --------------------------------------------------------------------------
 * Public Functions
 * --------------------------------------------------------------------------
 */


/*
 * Initialize display abstraction layer.
 *
 * Example:
 *
 *      LCD_Display_Init(&hi2c1);
 *
 */
void LCD_Display_Init(
        I2C_HandleTypeDef *hi2c)
{

    /*
     * Initialize low level LCD driver.
     *
     * Default:
     *
     *      Address : 0x27
     *
     *      Size:
     *
     *          20 columns
     *          4 rows
     */
    if(LCD_Init(
            hi2c,
            LCD_I2C_ADDRESS_DEFAULT,
            LCD_DEFAULT_COLUMNS,
            LCD_DEFAULT_ROWS) == HAL_OK)
    {

        lcd_display_initialized = 1U;


        /*
         * Clear startup garbage.
         */
        LCD_Clear();


        /*
         * Allow HD44780 controller
         * to complete clear command.
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
 *
 * This function belongs to display layer.
 *
 * It prevents display artifacts caused by
 * old characters remaining on LCD.
 *
 */
static void LCD_Display_WriteRow(
        uint8_t row,
        const char *text)
{

    uint8_t column = 0U;



    if(text == 0)
    {
        return;
    }



    /*
     * Move cursor to beginning of row.
     */
    LCD_SetCursor(
            0U,
            row);



    /*
     * Write text characters.
     */
    while((text[column] != '\0') &&
          (column < LCD_DEFAULT_COLUMNS))
    {

        LCD_PrintChar(
                text[column]);

        column++;

    }



    /*
     * Fill remaining columns with spaces.
     *
     * This removes old characters.
     */
    while(column < LCD_DEFAULT_COLUMNS)
    {

        LCD_PrintChar(' ');

        column++;

    }

}




/*
 * Show title line.
 *
 * LCD row 0 is reserved for page title.
 *
 * Example:
 *
 *      LCD_Display_ShowTitle("MAIN MENU");
 *
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
 * Print text on selected LCD row.
 *
 * Rows:
 *
 *      0 ... 3
 *
 * Example:
 *
 *      LCD_Display_PrintLine(
 *              1,
 *              "Live Monitor");
 *
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
 * Clear complete display.
 *
 */
void LCD_Display_Clear(void)
{

    if(lcd_display_initialized == 0U)
    {
        return;
    }



    LCD_Clear();


    /*
     * HD44780 clear command
     * requires extra execution time.
     */
    HAL_Delay(5U);

}




/*
 * Render menu screen.
 *
 * Data source:
 *
 *      MenuRenderer_GetData()
 *
 *
 * LCD format:
 *
 *
 *      Row 0:
 *          Page title
 *
 *      Row 1-3:
 *          Menu items
 *
 *
 * Cursor:
 *
 *      > Item
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



    if(menu == 0)
    {
        return;
    }





    /*
     * Check parameter edit mode.
     */

//    if(menu->mode == MENU_RENDER_MODE_EDIT)
//    {
//
//        char value_line[21];
//
//
//        /*
//         * Update edit screen without LCD clear.
//         *
//         * Only affected rows are rewritten.
//         * This avoids flicker.
//         */
//
//
//        /*
//         * Row 0:
//         * Edit title
//         */
//        LCD_Display_ShowTitle(
//                menu->edit_title);
//
//
//
//        /*
//         * Prepare value text.
//         */
//        snprintf(
//                value_line,
//                sizeof(value_line),
//                "Value: %ld",
//                menu->edit_value);
//
//
//
//        /*
//         * Row 1:
//         * Current value
//         */
//        LCD_Display_PrintLine(
//                1U,
//                value_line);
//
//
//
//        /*
//         * Clear unused rows.
//         *
//         * Prevent old menu text remaining.
//         */
//        LCD_Display_PrintLine(
//                2U,
//                "                    ");
//
//
//
//        /*
//         * Show edit controls.
//         *
//         * UP    : Increase value
//         * DOWN  : Decrease value
//         * ENTER : Confirm value
//         */
//        LCD_Display_PrintLine(
//                3U,
//                "+  -       Enter=Set");
//
//
//
//        return;
//
//    }



    if(menu->mode == MENU_RENDER_MODE_EDIT)
    {

        char value_line[21];

        MenuEditTarget_t target;


        target = MenuEdit_GetTarget();



        /*
         * Row 0:
         * Edit title
         */
        LCD_Display_ShowTitle(
                menu->edit_title);



        /*
         * Generate human readable value.
         */
        switch(target)
        {


            case EDIT_BAUD_RATE:

                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%ld bps",
                        menu->edit_value);

                break;



            case EDIT_SAMPLE_RATE:

                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%ld ms",
                        menu->edit_value);

                break;



            case EDIT_ALARM_ENABLE:


                if(menu->edit_value)
                {
                    snprintf(
                            value_line,
                            sizeof(value_line),
                            "ON");
                }
                else
                {
                    snprintf(
                            value_line,
                            sizeof(value_line),
                            "OFF");
                }

                break;




            case EDIT_LOW_VOLTAGE_LIMIT:

                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%ld V",
                        menu->edit_value);

                break;




            case EDIT_HIGH_VOLTAGE_LIMIT:

                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%ld V",
                        menu->edit_value);

                break;




            case EDIT_INPUT_VOLTAGE_OFFSET:

                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%.1f V",
                        (float)menu->edit_value / 10.0f);

                break;



            case EDIT_OUTPUT_VOLTAGE_OFFSET:

                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%.1f V",
                        (float)menu->edit_value / 10.0f);

                break;









            case EDIT_ALARM_MODE:


//                if(menu->edit_value == CONFIG_ALARM_REPEAT)
//                {
//                    snprintf(
//                            value_line,
//                            sizeof(value_line),
//                            "REPEAT");
//                }
//                else
//                {
//                    snprintf(
//                            value_line,
//                            sizeof(value_line),
//                            "ONCE");
//                }


            	if(menu->edit_value != 0)
            	{
            	    snprintf(
            	        value_line,
            	        sizeof(value_line),
            	        "Repeat");
            	}
            	else
            	{
            	    snprintf(
            	        value_line,
            	        sizeof(value_line),
            	        "Once");
            	}






                break;



            default:


                snprintf(
                        value_line,
                        sizeof(value_line),
                        "%ld",
                        menu->edit_value);

                break;

        }



        /*
         * Row 1:
         * Current value
         */
        LCD_Display_PrintLine(
                1U,
                value_line);



        /*
         * Row 2:
         * Empty
         */
        LCD_Display_PrintLine(
                2U,
                "                    ");



        /*
         * Row 3:
         * Controls
         */
        LCD_Display_PrintLine(
                3U,
                "UP/DN  ENTER=SAVE");



        return;

    }




    /*
     * Draw page title.
     */
    LCD_Display_WriteRow(
            0U,
            menu->title);



    /*
     * Draw visible menu items.
     */
    for(uint8_t i = 0U;
        i < MENU_RENDER_VISIBLE_ITEMS;
        i++)
    {

        char line[21];


        uint8_t index = 0U;



        /*
         * Clear local buffer.
         */
        for(uint8_t j = 0U;
            j < sizeof(line);
            j++)
        {
            line[j] = '\0';
        }


        /*
         * Draw cursor.
         *
         * Selected item:
         *
         *      > Item
         *
         * Non selected item:
         *
         *        Item
         */
        if(i == menu->cursor_position)
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
         * Copy item text.
         */
        if(i < menu->item_count)
        {

            const char *item =
                    menu->items[i];


            if(item != 0)
            {


            	uint8_t text_index = 0U;


            	while((item[text_index] != '\0') &&
            	      (index < LCD_DEFAULT_COLUMNS))
            	{

            	    line[index] =
            	            item[text_index];

            	    index++;
            	    text_index++;

            	}


            }

        }



        /*
         * Write menu row.
         *
         * LCD row:
         *
         *      1 ... 3
         */
        LCD_Display_WriteRow(
                i + 1U,
                line);

    }

}


/*
 * --------------------------------------------------------------------------
 * Advanced display helper functions
 * --------------------------------------------------------------------------
 */


/*
 * Return internal LCD handle.
 *
 * This function is provided for advanced
 * application access.
 *
 * Normal application code should use:
 *
 *      LCD_Display_PrintLine()
 *
 *      LCD_Display_RenderMenu()
 *
 *
 * Note:
 *
 * Current architecture uses the low level
 * driver internal handle.
 *
 * Therefore this function returns the
 * address of the LCD driver context.
 *
 */
void *LCD_Display_GetHandle(void)
{

    /*
     * The current LCD driver implementation
     * manages its internal context.
     *
     * No direct application access is required.
     */
    return 0;

}



/*
 * --------------------------------------------------------------------------
 * Internal validation helpers
 * --------------------------------------------------------------------------
 */


/*
 * Check display readiness.
 *
 * Private helper used internally.
 */
//static uint8_t LCD_Display_IsReady(void)
//{
//
//    return lcd_display_initialized;
//
//}



/*
 * --------------------------------------------------------------------------
 * End of display abstraction layer
 * --------------------------------------------------------------------------
 */

/*
 * --------------------------------------------------------------------------
 * Display State Interface
 * --------------------------------------------------------------------------
 */


/*
 * Check LCD display initialization state.
 *
 * Return:
 *
 *      1 :
 *          Display ready
 *
 *      0 :
 *          Display not initialized
 *
 *
 * This function is intentionally kept
 * private because application code should
 * not depend on internal display state.
 */




/*
 * --------------------------------------------------------------------------
 * End of file
 * --------------------------------------------------------------------------
 */


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
 *      PCF8574 I2C Backpack
 *
 *              +
 *
 *      HD44780 Character LCD
 *
 *
 ******************************************************************************/
