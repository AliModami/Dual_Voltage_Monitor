/******************************************************************************
 * @file    menu_renderer.c
 * @brief   Character LCD Menu Renderer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module is responsible only for displaying
 *      the menu structure on a character LCD.
 *
 *      Responsibilities:
 *
 *          - Display menu title
 *          - Display visible menu items
 *          - Display selection cursor
 *          - Manage three-line menu window
 *
 *
 *      This module does NOT:
 *
 *          - Read buttons
 *          - Change menu state
 *          - Execute menu actions
 *          - Access ADC
 *          - Access UART
 *
 *
 *-----------------------------------------------------------------------------
 *
 * LCD Layout:
 *
 *
 *      +--------------------+
 *      | Main Menu          |
 *      | > Live Monitor     |
 *      |   Start Stream     |
 *      |   Settings         |
 *      +--------------------+
 *
 *
 * Navigation behavior:
 *
 *      The first three items are displayed.
 *
 *      Cursor moves down until the last
 *      visible row is reached.
 *
 *      After that, menu content scrolls.
 *
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.4.0
 *
 ******************************************************************************/



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_renderer.h"

#include "lcd_i2c.h"

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>




/******************************************************************************
 *                         Private Configuration
 ******************************************************************************/

/*
 * LCD rows.
 *
 * Row 0 is reserved for menu title.
 */

#define LCD_TITLE_ROW              0U


#define LCD_FIRST_MENU_ROW         1U


#define LCD_LAST_MENU_ROW          3U



/*
 * LCD columns.
 */

#define LCD_CURSOR_COLUMN          0U


#define LCD_TEXT_COLUMN            2U



/*
 * Character LCD width.
 */

#define LCD_WIDTH                  20U



/*
 * Number of visible menu items.
 *
 * LCD 20x4:
 *
 *      One title row
 *      Three menu rows
 */

#define MENU_VISIBLE_ITEMS         3U




/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * First menu item currently displayed
 * in LCD window.
 *
 * Example:
 *
 *      display_start_item = Live Monitor
 *
 * LCD:
 *
 *      Live Monitor
 *      Start Stream
 *      Settings
 *
 */
static const MenuItem_t *display_start_item = NULL;



/*
 * Last selected item displayed.
 *
 * Used to detect cursor movement.
 */

static const MenuItem_t *last_selected_item = NULL;



/*
 * Indicates first LCD drawing.
 */

static bool first_render = true;



/******************************************************************************
 *                     Private Function Prototypes
 ******************************************************************************/

/*
 * Draw complete menu screen.
 */
static void MenuRenderer_DrawFullMenu(void);



/*
 * Update only cursor position.
 */
static void MenuRenderer_UpdateCursor(
                    const MenuItem_t *old_item,
                    const MenuItem_t *new_item);



/*
 * Update visible menu window.
 */
static void MenuRenderer_UpdateWindow(void);



/*
 * Draw menu title.
 */
static void MenuRenderer_DrawTitle(void);



/*
 * Draw visible menu items.
 */
static void MenuRenderer_DrawItems(void);



/*
 * Draw one menu item line.
 */
static void MenuRenderer_DrawItem(
                    uint8_t row,
                    const MenuItem_t *item,
                    bool selected);



/*
 * Clear one LCD row.
 */
static void MenuRenderer_ClearLine(
                    uint8_t row);



/*
 * Find if selected item is outside visible window.
 */
static bool MenuRenderer_NeedScroll(
                    const MenuItem_t *selected);



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu renderer.
 */
void MenuRenderer_Init(void)
{
    display_start_item = NULL;


    last_selected_item = NULL;


    first_render = true;
}



/**
 * @brief
 *      Periodic renderer update.
 *
 * @details
 *
 *      First call:
 *
 *          Complete LCD drawing
 *
 *
 *      Later:
 *
 *          Cursor movement only
 *          or window scrolling
 *
 */
void MenuRenderer_Update(void)
{
    const MenuItem_t *current_item;



    current_item = Menu_GetCurrentItem();



    if(current_item == NULL)
    {
        return;
    }



    if(first_render == true)
    {
        display_start_item = current_item;


        MenuRenderer_DrawFullMenu();


        last_selected_item = current_item;


        first_render = false;


        return;
    }



    if(current_item != last_selected_item)
    {
        if(MenuRenderer_NeedScroll(current_item))
        {
            MenuRenderer_UpdateWindow();
        }
        else
        {
            MenuRenderer_UpdateCursor(
                    last_selected_item,
                    current_item);
        }



        last_selected_item = current_item;
    }
}
/******************************************************************************
 *                         Private Functions
 ******************************************************************************/

/**
 * @brief
 *      Draw complete menu screen.
 *
 * @details
 *
 *      This function is used only when:
 *
 *          - LCD starts
 *          - Menu structure changes
 *          - Window scrolling happens
 *
 *      Normal UP/DOWN movement does not call
 *      this function to avoid LCD flicker.
 */
static void MenuRenderer_DrawFullMenu(void)
{
    LCD_Clear();


    MenuRenderer_DrawTitle();


    MenuRenderer_DrawItems();
}



/**
 * @brief
 *      Update only cursor position.
 *
 * @details
 *
 *      This function does not clear LCD.
 *
 *      It only removes the old cursor
 *      and draws the new cursor.
 *
 *      This eliminates visible flicker.
 */
static void MenuRenderer_UpdateCursor(
                    const MenuItem_t *old_item,
                    const MenuItem_t *new_item)
{
    uint8_t old_row;

    uint8_t new_row;



    old_row = LCD_FIRST_MENU_ROW;


    new_row = LCD_FIRST_MENU_ROW;



    /*
     * Find old cursor position.
     */

    {
        const MenuItem_t *item;


        item = display_start_item;


        while(item != NULL)
        {
            if(item == old_item)
            {
                break;
            }


            old_row++;


            item = item->next;
        }
    }



    /*
     * Find new cursor position.
     */

    {
        const MenuItem_t *item;


        item = display_start_item;


        new_row = LCD_FIRST_MENU_ROW;


        while(item != NULL)
        {
            if(item == new_item)
            {
                break;
            }


            new_row++;


            item = item->next;
        }
    }



    /*
     * Remove old cursor.
     */

    LCD_SetCursor(
            old_row,
            LCD_CURSOR_COLUMN);


    LCD_PrintChar(' ');



    /*
     * Draw new cursor.
     */

    LCD_SetCursor(
            new_row,
            LCD_CURSOR_COLUMN);


    LCD_PrintChar('>');
}



/**
 * @brief
 *      Check if selected item needs scrolling.
 *
 * @details
 *
 *      Example:
 *
 *      Visible:
 *
 *          Live Monitor
 *          Start Stream
 *          Settings
 *
 *
 *      Selected:
 *
 *          System Info
 *
 *
 *      Window must move down.
 */
static bool MenuRenderer_NeedScroll(
                    const MenuItem_t *selected)
{
    const MenuItem_t *item;


    uint8_t count;



    item = display_start_item;


    count = 0U;



    while(item != NULL)
    {
        if(item == selected)
        {
            return false;
        }



        item = item->next;


        count++;



        if(count >= MENU_VISIBLE_ITEMS)
        {
            break;
        }
    }



    return true;
}



/**
 * @brief
 *      Update visible menu window.
 *
 * @details
 *
 *      Moves the display window one item down.
 *
 *
 * Example:
 *
 * Before:
 *
 *      Live Monitor
 *      Start Stream
 *      Settings
 *
 *
 * After:
 *
 *      Start Stream
 *      Settings
 *      System Info
 *
 */
static void MenuRenderer_UpdateWindow(void)
{
    if(display_start_item == NULL)
    {
        return;
    }



    if(display_start_item->next != NULL)
    {
        display_start_item =
                display_start_item->next;
    }



    MenuRenderer_DrawFullMenu();
}



/**
 * @brief
 *      Draw menu title.
 *
 * @details
 *
 *      The title remains fixed.
 *
 *      It is always located
 *      on LCD row zero.
 */
static void MenuRenderer_DrawTitle(void)
{
    const MenuItem_t *root;



    root = Menu_GetRootItem();



    MenuRenderer_ClearLine(
            LCD_TITLE_ROW);



    LCD_SetCursor(
            LCD_TITLE_ROW,
            0U);



    if(root != NULL)
    {
        LCD_Print(root->name);
    }
}



/**
 * @brief
 *      Draw visible menu items.
 *
 * @details
 *
 *      Three menu items are displayed.
 *
 *      The selected item receives cursor.
 */
static void MenuRenderer_DrawItems(void)
{
    const MenuItem_t *item;


    uint8_t row;



    item = display_start_item;


    row = LCD_FIRST_MENU_ROW;



    while((item != NULL) &&
          (row <= LCD_LAST_MENU_ROW))
    {
        bool selected;



        selected =
            (item == Menu_GetCurrentItem());



        MenuRenderer_DrawItem(
                row,
                item,
                selected);



        item = item->next;


        row++;
    }



    /*
     * Clear unused LCD rows.
     */

    while(row <= LCD_LAST_MENU_ROW)
    {
        MenuRenderer_ClearLine(row);


        row++;
    }
}
/******************************************************************************
 *                      Menu Item Rendering
 ******************************************************************************/

/**
 * @brief
 *      Draw one menu item.
 *
 * @param row
 *      LCD row where item is displayed.
 *
 * @param item
 *      Menu item information.
 *
 * @param selected
 *      true:
 *          Draw cursor.
 *
 *      false:
 *          Draw normal text.
 *
 * @details
 *
 *      This function only handles one LCD line.
 *
 *      It does not modify menu state.
 */
static void MenuRenderer_DrawItem(
                    uint8_t row,
                    const MenuItem_t *item,
                    bool selected)
{
    if(item == NULL)
    {
        return;
    }



    /*
     * Clear line before writing.
     */
    MenuRenderer_ClearLine(row);



    /*
     * Draw cursor position.
     */
    LCD_SetCursor(
            row,
            LCD_CURSOR_COLUMN);



    if(selected == true)
    {
        LCD_PrintChar('>');
    }
    else
    {
        LCD_PrintChar(' ');
    }



    /*
     * Draw menu text.
     */
    LCD_SetCursor(
            row,
            LCD_TEXT_COLUMN);



    LCD_Print(item->name);
}



/**
 * @brief
 *      Clear one LCD row.
 *
 * @param row
 *      LCD row number.
 *
 * @details
 *
 *      Only the requested row is cleared.
 *
 *      Other rows remain unchanged.
 */
static void MenuRenderer_ClearLine(
                    uint8_t row)
{
    uint8_t column;



    LCD_SetCursor(
            row,
            0U);



    for(column = 0U;
        column < LCD_WIDTH;
        column++)
    {
        LCD_PrintChar(' ');
    }
}



/******************************************************************************
 *                          Design Notes
 ******************************************************************************/

/*
 * Rendering Strategy
 *
 * ------------------
 *
 * This renderer intentionally avoids LCD_Clear()
 * during normal navigation.
 *
 *
 * Previous behavior:
 *
 *      Button press
 *            |
 *            v
 *      LCD_Clear()
 *            |
 *            v
 *      Redraw everything
 *
 *
 * Result:
 *
 *      - Visible flicker
 *      - Slow refresh
 *      - Unnecessary I2C traffic
 *
 *
 *
 * New behavior:
 *
 *      Button press
 *            |
 *            v
 *      Move cursor only
 *
 *
 * When window reaches the end:
 *
 *      Scroll menu window
 *            |
 *            v
 *      Redraw visible area
 *
 *
 * This provides:
 *
 *      - Smooth cursor movement
 *      - Minimal LCD traffic
 *      - Better user experience
 */



/******************************************************************************
 *                          Future Expansion
 ******************************************************************************/

/*
 * Possible future improvements:
 *
 *      - Submenu title support
 *
 *      - Settings menu renderer
 *
 *      - Custom LCD characters
 *
 *      - Persian character mapping
 *
 *      - Long menu scrolling
 *
 *      - Menu icons
 *
 *
 * The current architecture keeps these additions
 * isolated from menu.c.
 */



/******************************************************************************
 *                          File Summary
 ******************************************************************************/

/*
 * Public Functions:
 *
 *      MenuRenderer_Init()
 *
 *      MenuRenderer_Update()
 *
 *
 *
 * Private Functions:
 *
 *      MenuRenderer_DrawFullMenu()
 *
 *      MenuRenderer_UpdateCursor()
 *
 *      MenuRenderer_UpdateWindow()
 *
 *      MenuRenderer_DrawTitle()
 *
 *      MenuRenderer_DrawItems()
 *
 *      MenuRenderer_DrawItem()
 *
 *      MenuRenderer_ClearLine()
 *
 *      MenuRenderer_NeedScroll()
 *
 */



/******************************************************************************
 *                              End of File
 ******************************************************************************/
