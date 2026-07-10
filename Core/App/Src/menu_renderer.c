/******************************************************************************
 * @file    menu_renderer.c
 * @brief   Menu Renderer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file implements the menu display layer.
 *
 *      The renderer converts menu engine state into
 *      LCD output.
 *
 *
 *      Responsibilities:
 *
 *          - Read current menu state.
 *          - Draw menu title.
 *          - Draw visible menu items.
 *          - Show selected item marker.
 *          - Control refresh requests.
 *
 *
 *      This module DOES NOT:
 *
 *          - Process buttons.
 *          - Change menu state.
 *          - Execute menu actions.
 *          - Access hardware except LCD driver.
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
 *
 *
 *          menu_engine.c
 *
 *                |
 *                v
 *
 *          menu_renderer.c
 *
 *                |
 *                v
 *
 *             lcd_i2c.c
 *
 *
 *-----------------------------------------------------------------------------
 * Rendering Concept:
 *
 *
 * LCD 20x4:
 *
 *
 * +--------------------+
 * | Main Menu          |
 * +--------------------+
 * | > Live Monitor     |
 * |   Start Stream     |
 * |   Settings         |
 * +--------------------+
 *
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami
 *
 * Version :
 *      1.0.0
 *
 * Change History :
 *
 *      1.0.0
 *          Initial Plan A architecture version.
 *
 ******************************************************************************/



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_renderer.h"

#include "lcd_i2c.h"

#include <string.h>

#include <stdio.h>





/******************************************************************************
 *                         Private Constants
 ******************************************************************************/

/*
 * Character used for empty LCD space.
 *
 * Clearing unused characters prevents
 * old text remaining on LCD.
 */

#define LCD_EMPTY_CHARACTER          (' ')





/*
 * Maximum temporary line buffer.
 *
 * LCD width:
 *
 *      20 characters
 *
 */

#define MENU_RENDERER_BUFFER_SIZE    (21U)





/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * Renderer refresh request flag.
 *
 * When true:
 *
 *      LCD needs update.
 *
 */

static bool renderer_refresh_required = true;





/*
 * Temporary LCD line buffer.
 *
 * One extra byte is reserved
 * for NULL terminator.
 *
 */

static char lcd_line_buffer[
        MENU_RENDERER_BUFFER_SIZE];





/******************************************************************************
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Clear one LCD line.
 *
 * @param row
 *
 *      LCD row number.
 *
 * @details
 *
 *      Writes spaces across complete row.
 *
 *      This prevents leftover characters
 *      from previous longer strings.
 *
 */
static void MenuRenderer_ClearLine(
        uint8_t row)
{

    uint8_t index;



    LCD_SetCursor(row,0U);



    for(index = 0U;
        index < MENU_RENDERER_LCD_COLUMNS;
        index++)
    {

        LCD_PrintChar(
                LCD_EMPTY_CHARACTER);

    }

}





/**
 * @brief
 *      Prepare one menu item line.
 *
 * @param item
 *
 *      Menu item data.
 *
 * @param selected
 *
 *      Indicates selected item.
 *
 */
static void MenuRenderer_PrepareItemLine(
        const MenuItem_t *item,
        bool selected)
{

    uint8_t length;



    /*
     * Clear buffer.
     */

    memset(lcd_line_buffer,
           0,
           sizeof(lcd_line_buffer));



    if(item == NULL)
    {
        return;
    }



    /*
     * Add selection marker.
     */

    if(selected)
    {
        lcd_line_buffer[0] =
                MENU_SELECTED_MARKER;
    }
    else
    {
        lcd_line_buffer[0] =
                MENU_NORMAL_MARKER;
    }



    /*
     * Add one space after marker.
     */

    lcd_line_buffer[1] = ' ';



    /*
     * Copy menu item text.
     */

    strncpy(
            &lcd_line_buffer[2],
            item->text,
            MENU_RENDERER_LCD_COLUMNS - 2U);



    /*
     * Ensure string termination.
     */

    lcd_line_buffer[
            MENU_RENDERER_LCD_COLUMNS] = '\0';



    /*
     * Calculate current length.
     */

    length =
        strlen(lcd_line_buffer);



    /*
     * Fill remaining characters.
     */

    while(length < MENU_RENDERER_LCD_COLUMNS)
    {

        lcd_line_buffer[length] =
                LCD_EMPTY_CHARACTER;

        length++;

    }



    lcd_line_buffer[
            MENU_RENDERER_LCD_COLUMNS] = '\0';

}


/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu renderer.
 *
 * @details
 *
 *      Renderer starts with refresh requested.
 *
 *      First update will draw complete menu.
 *
 */
void MenuRenderer_Init(void)
{

    renderer_refresh_required = true;


    memset(lcd_line_buffer,
           0,
           sizeof(lcd_line_buffer));

}





/**
 * @brief
 *      Update LCD menu display.
 *
 * @details
 *
 *      This is the main rendering function.
 *
 *      Sequence:
 *
 *          1. Check refresh request.
 *
 *          2. Clear LCD.
 *
 *          3. Draw title.
 *
 *          4. Draw menu items.
 *
 *          5. Clear engine refresh flag.
 *
 */
void MenuRenderer_Update(void)
{

    const Menu_t *menu;



    /*
     * Do nothing if refresh
     * is not required.
     */

    if(!renderer_refresh_required &&
       !MenuEngine_IsChanged())
    {
        return;
    }




    menu =
        MenuEngine_GetCurrentMenu();



    if(menu == NULL)
    {
        return;
    }




    /*
     * Clear complete display.
     */

    LCD_Clear();




    /*
     * Draw menu title.
     */

    MenuRenderer_DrawTitle(
            menu->title);




    /*
     * Draw visible items.
     */

    MenuRenderer_DrawItems();




    /*
     * Renderer update completed.
     */

    renderer_refresh_required = false;



    MenuEngine_ClearChangedFlag();

}





/**
 * @brief
 *      Force renderer refresh.
 *
 * @details
 *
 *      Used when an external application
 *      changes display content.
 *
 */
void MenuRenderer_ForceRefresh(void)
{

    renderer_refresh_required = true;

}





/**
 * @brief
 *      Draw menu title.
 *
 * @param title
 *
 *      Menu title string.
 *
 */
void MenuRenderer_DrawTitle(
        const char *title)
{

    if(title == NULL)
    {
        return;
    }



    LCD_SetCursor(
            0U,
            0U);



    memset(lcd_line_buffer,
           0,
           sizeof(lcd_line_buffer));



    strncpy(
            lcd_line_buffer,
            title,
            MENU_RENDERER_LCD_COLUMNS);



    lcd_line_buffer[
            MENU_RENDERER_LCD_COLUMNS] = '\0';



    LCD_Print(
            lcd_line_buffer);

}





/**
 * @brief
 *      Draw visible menu items.
 *
 * @details
 *
 *      Only items visible on LCD are drawn.
 *
 *      Example:
 *
 *          top_index = 2
 *
 *          LCD shows:
 *
 *              Item 2
 *              Item 3
 *              Item 4
 *
 */
void MenuRenderer_DrawItems(void)
{

    const Menu_t *menu;

    const MenuItem_t *item;

    uint8_t row;

    uint8_t index;

    uint8_t top_index;



    menu =
        MenuEngine_GetCurrentMenu();



    if(menu == NULL)
    {
        return;
    }




    top_index =
        MenuEngine_GetTopIndex();




    /*
     * Draw LCD visible rows.
     *
     * Row 0 is title.
     *
     * Items start from row 1.
     */

    row = 1U;



    index = top_index;



    while(row < MENU_RENDERER_LCD_ROWS)
    {

        /*
         * Clear current row first.
         */

        MenuRenderer_ClearLine(row);



        /*
         * Check available menu item.
         */

        item =
            MenuData_GetItem(
                    menu,
                    index);



        if(item != NULL)
        {

            MenuRenderer_PrepareItemLine(
                    item,
                    (index ==
                     MenuEngine_GetSelectedIndex()));



            LCD_SetCursor(
                    row,
                    0U);



            LCD_Print(
                    lcd_line_buffer);

        }



        row++;


        index++;

    }

}


/******************************************************************************
 *                         Renderer State Functions
 ******************************************************************************/

/**
 * @brief
 *      Clear renderer refresh request.
 *
 * @details
 *
 *      This function clears the internal
 *      renderer update flag.
 *
 *      Normally it is called after a
 *      successful LCD refresh.
 *
 */
void MenuRenderer_ClearRefresh(void)
{

    renderer_refresh_required = false;

}





/**
 * @brief
 *      Check renderer refresh status.
 *
 * @return
 *
 *      true:
 *          LCD update is required.
 *
 *      false:
 *          No update required.
 *
 */
bool MenuRenderer_IsRefreshRequired(void)
{

    return renderer_refresh_required;

}





/******************************************************************************
 *                              End Of File
 ******************************************************************************/
