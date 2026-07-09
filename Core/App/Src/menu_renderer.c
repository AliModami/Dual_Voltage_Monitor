/******************************************************************************
 * @file    menu_renderer.c
 * @brief   LCD Menu Renderer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module is responsible ONLY for rendering the menu on the LCD.
 *
 *      The renderer never modifies menu state.
 *
 *      Renderer responsibilities:
 *
 *          • Read menu information
 *          • Draw LCD title
 *          • Draw visible menu items
 *          • Draw current selection
 *          • Refresh LCD only when necessary
 *
 *      Renderer does NOT:
 *
 *          • Read GPIO
 *          • Process buttons
 *          • Navigate menus
 *          • Execute menu actions
 *          • Access ADC
 *          • Access UART
 *
 *-----------------------------------------------------------------------------
 *
 *                      Software Architecture
 *
 *          +----------------------+
 *          |    GPIO Buttons      |
 *          +----------+-----------+
 *                     |
 *                     v
 *          +----------------------+
 *          |     buttons.c        |
 *          +----------+-----------+
 *                     |
 *                     v
 *          +----------------------+
 *          |   button_app.c       |
 *          +----------+-----------+
 *                     |
 *                     v
 *          +----------------------+
 *          |      menu.c          |
 *          +----------+-----------+
 *                     |
 *                     | Read Only
 *                     v
 *          +----------------------+
 *          | menu_renderer.c      |
 *          +----------+-----------+
 *                     |
 *                     v
 *          +----------------------+
 *          |     lcd_i2c.c        |
 *          +----------------------+
 *
 *-----------------------------------------------------------------------------
 *
 * LCD Layout
 *
 *      LCD 20x4
 *
 *      +----------------------+
 *      | Main Menu            |
 *      | > Live Monitor       |
 *      |   Start Stream       |
 *      |   Settings           |
 *      +----------------------+
 *
 *-----------------------------------------------------------------------------
 *
 * Design Rules
 *
 *      Rule 1
 *      -------
 *      Renderer never changes application state.
 *
 *      Rule 2
 *      -------
 *      Renderer always asks Menu module for information.
 *
 *      Rule 3
 *      -------
 *      LCD driver remains completely independent.
 *
 *      Rule 4
 *      -------
 *      Renderer may be replaced in future by:
 *
 *          • TFT Renderer
 *          • OLED Renderer
 *          • UART Renderer
 *
 *      without changing menu.c.
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 ******************************************************************************/

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_renderer.h"

#include "lcd_i2c.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>



/******************************************************************************
 *                      Private Configuration
 ******************************************************************************/

/*
 * LCD rows.
 */

#define MENU_TITLE_ROW         0U

#define MENU_LINE1_ROW         1U

#define MENU_LINE2_ROW         2U

#define MENU_LINE3_ROW         3U



/*
 * LCD columns.
 */

#define MENU_ARROW_COLUMN      0U

#define MENU_TEXT_COLUMN       2U



/*
 * LCD width.
 *
 * Used when clearing lines.
 */

#define LCD_LINE_LENGTH        20U



/******************************************************************************
 *                      Private Variables
 ******************************************************************************/

/*
 * Last rendered menu item.
 *
 * Used to avoid unnecessary LCD refresh.
 */

static const MenuItem_t *last_item = NULL;



/*
 * Last rendered application mode.
 */

static Menu_Mode_t last_mode = MENU_MODE;



/******************************************************************************
 *                  Private Function Prototypes
 ******************************************************************************/

/*
 * Draw currently active application screen.
 */

static void MenuRenderer_DrawCurrentScreen(void);



/*
 * Draw menu page.
 */

static void MenuRenderer_DrawMenu(void);



/*
 * Draw title line.
 */

static void MenuRenderer_DrawTitle(void);



/*
 * Draw visible menu entries.
 */

static void MenuRenderer_DrawItems(void);



/*
 * Draw one menu entry.
 */

static void MenuRenderer_DrawItem(
                    uint8_t row,
                    const MenuItem_t *item,
                    bool selected);



/*
 * Clear one LCD line.
 */

static void MenuRenderer_ClearLine(uint8_t row);



/******************************************************************************
 *                      Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize renderer.
 *
 * @details
 *      LCD hardware has already been initialized
 *      before this function is called.
 */
void MenuRenderer_Init(void)
{
    last_item = NULL;

    last_mode = MENU_MODE;
}

/******************************************************************************
 *                      Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Periodic renderer task.
 *
 * @details
 *      This function should be called continuously
 *      from the application's main loop.
 *
 *      Renderer compares the current menu state with
 *      the last rendered state.
 *
 *      LCD is refreshed only when something changes.
 *
 *      Advantages:
 *
 *          • Less LCD flicker
 *          • Less I2C traffic
 *          • Better performance
 *
 */
void MenuRenderer_Update(void)
{
    const MenuItem_t *current_item;

    Menu_Mode_t current_mode;



    /*
     * Read current application state.
     */

    current_item = Menu_GetCurrentItem();

    current_mode = Menu_GetMode();



    /*
     * Has anything changed?
     */

    if ((current_item != last_item) ||
        (current_mode != last_mode))
    {
        MenuRenderer_DrawCurrentScreen();

        last_item = current_item;

        last_mode = current_mode;
    }
}



/**
 * @brief
 *      Force complete LCD redraw.
 *
 * @details
 *      This function ignores the internal cache
 *      and redraws the complete active screen.
 *
 *      Future modules may call this after:
 *
 *          • Language change
 *          • LCD reinitialization
 *          • Display recovery
 *
 */
void MenuRenderer_Refresh(void)
{
    MenuRenderer_DrawCurrentScreen();

    last_item = Menu_GetCurrentItem();

    last_mode = Menu_GetMode();
}



/******************************************************************************
 *                  Private Rendering Dispatcher
 ******************************************************************************/

/**
 * @brief
 *      Draw currently active application screen.
 *
 * @details
 *      Renderer decides which screen must be drawn
 *      according to the current application mode.
 *
 *      IMPORTANT
 *      =========
 *
 *      menu.c decides WHICH mode is active.
 *
 *      menu_renderer.c decides HOW it is drawn.
 *
 *      This separation keeps both modules independent.
 *
 */
static void MenuRenderer_DrawCurrentScreen(void)
{
    switch (Menu_GetMode())
    {
        case MENU_MODE:
        {
            MenuRenderer_DrawMenu();

            break;
        }



        case MONITOR_MODE:
        {
            /*
             * Future implementation.
             */

            LCD_Clear();

            LCD_SetCursor(0,0);
            LCD_Print("Live Monitor");

            break;
        }



        case STREAM_MODE:
        {
            /*
             * Future implementation.
             */

            LCD_Clear();

            LCD_SetCursor(0,0);
            LCD_Print("UART Stream");

            break;
        }



        case SETTINGS_MODE:
        {
            /*
             * Future implementation.
             */

            LCD_Clear();

            LCD_SetCursor(0,0);
            LCD_Print("Settings");

            break;
        }



        case SYSTEM_INFO_MODE:
        {
            /*
             * Future implementation.
             */

            LCD_Clear();

            LCD_SetCursor(0,0);
            LCD_Print("System Info");

            break;
        }



        default:
        {
            LCD_Clear();

            LCD_SetCursor(0,0);

            LCD_Print("Unknown Mode");

            break;
        }
    }
}



/******************************************************************************
 *                      Menu Rendering
 ******************************************************************************/

/**
 * @brief
 *      Draw complete menu screen.
 *
 * @details
 *      Rendering sequence:
 *
 *          1. Clear LCD
 *          2. Draw title
 *          3. Draw menu items
 *
 */
static void MenuRenderer_DrawMenu(void)
{
    LCD_Clear();

    MenuRenderer_DrawTitle();

    MenuRenderer_DrawItems();
}

/**
 * @brief
 *      Draw menu title.
 *
 * @details
 *      The title is always displayed on the first LCD row.
 *
 *      At the moment the title is fixed because only one
 *      root menu exists.
 *
 *      Future versions may display the current submenu title.
 */
static void MenuRenderer_DrawTitle(void)
{
    const MenuItem_t *root;



    root = Menu_GetRootItem();



    MenuRenderer_ClearLine(MENU_TITLE_ROW);



    LCD_SetCursor(MENU_TITLE_ROW,
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
 *      The renderer displays three menu items:
 *
 *          Row 1 : Previous item
 *          Row 2 : Current item
 *          Row 3 : Next item
 *
 *      This produces a smooth navigation effect while
 *      keeping the renderer independent from menu logic.
 */
static void MenuRenderer_DrawItems(void)
{
    const MenuItem_t *current;

    const MenuItem_t *previous;

    const MenuItem_t *next;



    current = Menu_GetCurrentItem();



    if(current == NULL)
    {
        return;
    }



    previous = current->prev;

    next = current->next;



    /*
     * Draw previous menu item.
     */

    if(previous != NULL)
    {
        MenuRenderer_DrawItem(
                        MENU_LINE1_ROW,
                        previous,
                        false);
    }
    else
    {
        MenuRenderer_ClearLine(MENU_LINE1_ROW);
    }



    /*
     * Draw current menu item.
     */

    MenuRenderer_DrawItem(
                    MENU_LINE2_ROW,
                    current,
                    true);



    /*
     * Draw next menu item.
     */

    if(next != NULL)
    {
        MenuRenderer_DrawItem(
                        MENU_LINE3_ROW,
                        next,
                        false);
    }
    else
    {
        MenuRenderer_ClearLine(MENU_LINE3_ROW);
    }
}



/**
 * @brief
 *      Clear one LCD line.
 *
 * @param row
 *      LCD row number.
 *
 * @details
 *      Clears exactly one LCD row without affecting
 *      the remaining display.
 */
static void MenuRenderer_ClearLine(uint8_t row)
{
    uint8_t column;



    LCD_SetCursor(row,
                  0U);



    for(column = 0U;
        column < LCD_LINE_LENGTH;
        column++)
    {
        LCD_PrintChar(' ');
    }
}

/******************************************************************************
 * @brief
 *      Draw one menu item.
 *
 * @param row
 *      LCD row.
 *
 * @param item
 *      Pointer to menu item.
 *
 * @param selected
 *      true  -> draw selection arrow
 *      false -> draw normal item
 *
 * @details
 *
 *      Example:
 *
 *          > Live Monitor
 *
 *            Start Stream
 *
 *      This function only renders one line.
 *
 *      Navigation logic remains inside menu.c.
 *
 ******************************************************************************/
static void MenuRenderer_DrawItem(
                    uint8_t row,
                    const MenuItem_t *item,
                    bool selected)
{
    /*
     * Safety check.
     */

    if(item == NULL)
    {
        return;
    }



    /*
     * Clear current LCD line before drawing.
     */

    MenuRenderer_ClearLine(row);



    /*
     * Draw selection marker.
     */

    LCD_SetCursor(row,
                  MENU_ARROW_COLUMN);



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

    LCD_SetCursor(row,
                  MENU_TEXT_COLUMN);

    LCD_Print(item->name);
}



/******************************************************************************
 *                  Renderer Design Notes
 *
 *  Why separate DrawItem()?
 *
 *      Because every menu entry is rendered exactly
 *      the same way.
 *
 *      If one day we decide to:
 *
 *          - use custom characters
 *          - draw icons
 *          - animate cursor
 *          - invert colors (TFT)
 *
 *      only this function must change.
 *
 *      The rest of renderer remains untouched.
 *
 ******************************************************************************/

/******************************************************************************
 *                      Rendering Optimization Notes
 *
 *  Current Strategy
 *  ----------------
 *
 *  The renderer performs a complete redraw whenever:
 *
 *      • Selected menu item changes
 *      • Application mode changes
 *
 *  Because the LCD contains only four rows and the I2C bus
 *  is relatively slow compared with the MCU, a complete redraw
 *  keeps the implementation simple while still providing very
 *  good responsiveness.
 *
 *  Future versions may introduce:
 *
 *      • Dirty-line rendering
 *      • Dirty-region rendering
 *      • Character-level updates
 *
 *  if display performance ever becomes a limiting factor.
 *
 ******************************************************************************/

/******************************************************************************
 *                      Future Expansion Guide
 *
 *  The renderer has intentionally been designed so that adding
 *  a new application screen does NOT require modifications to
 *  the existing menu logic.
 *
 *  Example:
 *
 *      case MONITOR_MODE:
 *
 *          MonitorRenderer_Draw();
 *
 *          break;
 *
 *  or
 *
 *      case SETTINGS_MODE:
 *
 *          SettingsRenderer_Draw();
 *
 *          break;
 *
 *  Therefore:
 *
 *      menu.c
 *          controls navigation
 *
 *      menu_renderer.c
 *          controls presentation
 *
 *      monitor_renderer.c
 *          controls monitor screen
 *
 *      stream_renderer.c
 *          controls stream screen
 *
 ******************************************************************************/

/******************************************************************************
 *                      Maintenance Notes
 *
 *  IMPORTANT
 *  =========
 *
 *  Do NOT place application logic inside this module.
 *
 *  Renderer modules should NEVER:
 *
 *      • Change menu state
 *      • Execute actions
 *      • Read GPIO
 *      • Access ADC
 *      • Start UART
 *      • Modify settings
 *
 *  Renderer modules only display information.
 *
 ******************************************************************************/

/******************************************************************************
 *                      File Summary
 *
 *  Public Functions
 *
 *      MenuRenderer_Init()
 *
 *      MenuRenderer_Update()
 *
 *      MenuRenderer_Refresh()
 *
 *
 *  Private Functions
 *
 *      MenuRenderer_DrawCurrentScreen()
 *
 *      MenuRenderer_DrawMenu()
 *
 *      MenuRenderer_DrawTitle()
 *
 *      MenuRenderer_DrawItems()
 *
 *      MenuRenderer_DrawItem()
 *
 *      MenuRenderer_ClearLine()
 *
 ******************************************************************************/


/******************************************************************************
 *                      Version History
 *
 *  Version 1.0.0
 *  -------------
 *
 *      Initial implementation.
 *
 *      Features:
 *
 *          • Complete renderer architecture
 *          • LCD menu rendering
 *          • Read-only interaction with menu framework
 *          • Automatic refresh on state change
 *          • Cursor rendering
 *          • Modular design for future screens
 *
 *------------------------------------------------------------------------------
 *
 *  Planned Version 1.1.0
 *
 *      □ Dirty-line refresh
 *      □ Scrollable menus
 *      □ Submenu rendering
 *      □ UTF-8/Persian rendering layer
 *      □ Icon support
 *      □ TFT renderer compatibility
 *
 ******************************************************************************/

/******************************************************************************
 *                      Coding Standard Compliance
 *
 *  This module follows the project coding rules:
 *
 *      ✔ One responsibility
 *
 *          Renderer only.
 *
 *      ✔ Hardware independent
 *
 *          Uses LCD API only.
 *
 *      ✔ No GPIO access
 *
 *      ✔ No HAL dependency
 *
 *      ✔ Read-only access to Menu Framework
 *
 *      ✔ Fully documented
 *
 *      ✔ Easy to extend
 *
 ******************************************************************************/

/******************************************************************************
 *                              End of File
 ******************************************************************************/
