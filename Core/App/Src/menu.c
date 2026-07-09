/******************************************************************************
 * @file    menu.c
 * @brief   Menu Framework Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file implements the application menu framework.
 *
 *      Responsibilities:
 *
 *          - Manage menu tree
 *          - Track selected item
 *          - Process navigation commands
 *          - Execute menu actions
 *          - Provide read-only information for renderer
 *
 *      This module does NOT access:
 *
 *          - LCD
 *          - GPIO
 *          - ADC
 *          - UART
 *
 *-----------------------------------------------------------------------------
 *
 * Architecture:
 *
 *          Buttons
 *             |
 *             v
 *       button_app.c
 *             |
 *             v
 *          menu.c
 *             |
 *             v
 *    menu_renderer.c
 *             |
 *             v
 *       lcd_i2c.c
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.2.0
 *
 ******************************************************************************/



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu.h"

#include <stddef.h>




/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * Current application mode.
 */

static Menu_Mode_t current_mode = MENU_MODE;



/*
 * Currently selected menu item.
 */

static MenuItem_t *current_item = NULL;



/*
 * Current menu title.
 *
 * Used by renderer as fixed LCD header.
 */

static const char *current_title = "Main Menu";




/******************************************************************************
 *                         Private Function Prototypes
 ******************************************************************************/

/*
 * Move selection downward.
 */
static void Menu_MoveNext(void);



/*
 * Move selection upward.
 */
static void Menu_MovePrevious(void);



/*
 * Execute selected item.
 */
static void Menu_EnterItem(void);



/*
 * Return to parent menu.
 */
static void Menu_Back(void);



/*
 * Menu actions.
 */

static void Menu_ActionMonitor(void);

static void Menu_ActionStream(void);

static void Menu_ActionSettings(void);

static void Menu_ActionSystemInfo(void);




/******************************************************************************
 *                         Menu Tree Declaration
 ******************************************************************************/

/*
 * Forward declarations.
 */

static MenuItem_t menu_root;

static MenuItem_t menu_monitor;

static MenuItem_t menu_stream;

static MenuItem_t menu_settings;

static MenuItem_t menu_system_info;




/******************************************************************************
 *                         Menu Tree Definition
 ******************************************************************************/

/*
 * Root item.
 */

static MenuItem_t menu_root =
{
    .name   = "Main Menu",

    .action = NULL,

    .parent = NULL,

    .child  = &menu_monitor,

    .next   = NULL,

    .prev   = NULL
};



/*
 * Live Monitor item.
 */

static MenuItem_t menu_monitor =
{
    .name   = "Live Monitor",

    .action = Menu_ActionMonitor,

    .parent = &menu_root,

    .child  = NULL,

    /*
     * Normal linked list.
     *
     * No wrap around.
     */

    .next   = &menu_stream,

    .prev   = NULL
};



/*
 * Stream item.
 */

static MenuItem_t menu_stream =
{
    .name   = "Start Stream",

    .action = Menu_ActionStream,

    .parent = &menu_root,

    .child  = NULL,

    .next   = &menu_settings,

    .prev   = &menu_monitor
};



/*
 * Settings item.
 */

static MenuItem_t menu_settings =
{
    .name   = "Settings",

    .action = Menu_ActionSettings,

    .parent = &menu_root,

    .child  = NULL,

    .next   = &menu_system_info,

    .prev   = &menu_stream
};



/*
 * System information item.
 */

static MenuItem_t menu_system_info =
{
    .name   = "System Info",

    .action = Menu_ActionSystemInfo,

    .parent = &menu_root,

    .child  = NULL,

    /*
     * Last item.
     *
     * DOWN stops here.
     */

    .next   = NULL,

    .prev   = &menu_settings
};



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu framework.
 */
void Menu_Init(void)
{
    current_mode = MENU_MODE;


    current_title = "Main Menu";


    /*
     * First selectable item.
     */

    current_item = &menu_monitor;
}



/**
 * @brief
 *      Periodic menu task.
 */
void Menu_Task(void)
{

}



/**
 * @brief
 *      Process button command.
 */
void Menu_ProcessCommand(Button_AppCommand_t command)
{
    switch(command)
    {
        case BUTTON_CMD_UP:

            Menu_MovePrevious();

            break;



        case BUTTON_CMD_DOWN:

            Menu_MoveNext();

            break;



        case BUTTON_CMD_ENTER:

            Menu_EnterItem();

            break;



        case BUTTON_CMD_BACK:

            Menu_Back();

            break;



        default:

            break;
    }
}

/******************************************************************************
 *                  Read Only Renderer Interface
 ******************************************************************************/

/**
 * @brief
 *      Return current selected menu item.
 *
 * @details
 *      Renderer uses this function to know
 *      which item is selected.
 *
 *      No modification is allowed.
 *
 * @return
 *      Pointer to current item.
 */
const MenuItem_t *Menu_GetCurrentItem(void)
{
    return current_item;
}



/**
 * @brief
 *      Return root menu item.
 *
 * @return
 *      Pointer to root item.
 */
const MenuItem_t *Menu_GetRootItem(void)
{
    return &menu_root;
}



/**
 * @brief
 *      Return current menu title.
 *
 * @details
 *      Renderer displays this text
 *      on the fixed title row.
 *
 * @return
 *      Pointer to title string.
 */
const char *Menu_GetCurrentMenuTitle(void)
{
    return current_title;
}



/**
 * @brief
 *      Calculate LCD row of selected item.
 *
 * @details
 *      This function is used by the
 *      incremental renderer.
 *
 *      The current LCD design shows:
 *
 *          Row 0 : Title
 *
 *          Row 1 : Item above
 *
 *          Row 2 : Selected item
 *
 *          Row 3 : Item below
 *
 *
 *      Therefore the selected item
 *      is always rendered on row 2.
 *
 * @param item
 *      Menu item pointer.
 *
 * @return
 *      LCD row number.
 */
uint8_t Menu_GetItemRow(const MenuItem_t *item)
{
    if(item == NULL)
    {
        return 0U;
    }



    if(item == current_item)
    {
        return 2U;
    }



    return 0U;
}




/******************************************************************************
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Move selection downward.
 *
 * @details
 *
 *      Navigation stops at the last item.
 *
 *      No circular movement exists.
 *
 */
static void Menu_MoveNext(void)
{
    if(current_item == NULL)
    {
        return;
    }



    /*
     * Move only when next item exists.
     */

    if(current_item->next != NULL)
    {
        current_item = current_item->next;
    }
}



/**
 * @brief
 *      Move selection upward.
 *
 * @details
 *
 *      Navigation stops at first item.
 *
 *      No circular movement exists.
 *
 */
static void Menu_MovePrevious(void)
{
    if(current_item == NULL)
    {
        return;
    }



    /*
     * Move only when previous item exists.
     */

    if(current_item->prev != NULL)
    {
        current_item = current_item->prev;
    }
}



/**
 * @brief
 *      Execute selected item.
 */
static void Menu_EnterItem(void)
{
    if(current_item == NULL)
    {
        return;
    }



    if(current_item->action != NULL)
    {
        current_item->action();
    }
}



/**
 * @brief
 *      Return to parent menu.
 */
static void Menu_Back(void)
{
    if(current_item == NULL)
    {
        return;
    }



    if(current_item->parent != NULL)
    {
        current_item = current_item->parent;

        current_mode = MENU_MODE;

        current_title = "Main Menu";
    }
}



/******************************************************************************
 *                         Menu Actions
 ******************************************************************************/


/**
 * @brief
 *      Enter Live Monitor screen.
 */
static void Menu_ActionMonitor(void)
{
    current_mode = MONITOR_MODE;
}



/**
 * @brief
 *      Enter Stream screen.
 */
static void Menu_ActionStream(void)
{
    current_mode = STREAM_MODE;
}



/**
 * @brief
 *      Enter Settings menu.
 */
static void Menu_ActionSettings(void)
{
    current_mode = SETTINGS_MODE;


    /*
     * Future submenu expansion:
     *
     * current_title = "Settings Menu";
     *
     * when child items are added.
     */
}



/**
 * @brief
 *      Enter System Information screen.
 */
static void Menu_ActionSystemInfo(void)
{
    current_mode = SYSTEM_INFO_MODE;
}



/******************************************************************************
 *                         Additional Notes
 ******************************************************************************/

/*
 * Menu framework behavior:
 *
 *
 * 1) Navigation
 * ----------------
 *
 * The menu uses a linear linked list.
 *
 * Example:
 *
 *      Live Monitor
 *             |
 *             v
 *      Start Stream
 *             |
 *             v
 *       Settings
 *             |
 *             v
 *       System Info
 *
 *
 * The first item has:
 *
 *      prev = NULL
 *
 * The last item has:
 *
 *      next = NULL
 *
 *
 * Therefore:
 *
 *      UP at first item:
 *
 *          No action
 *
 *
 *      DOWN at last item:
 *
 *          No action
 *
 *
 * This creates a professional
 * non-circular menu behavior.
 *
 */



/*
 * Renderer cooperation:
 *
 * menu.c owns:
 *
 *      - Selection state
 *      - Navigation
 *      - Actions
 *
 *
 * menu_renderer.c owns:
 *
 *      - LCD drawing
 *      - Cursor display
 *      - Text positioning
 *
 *
 * The renderer never changes
 * the menu state.
 *
 */



/*
 * Future Settings submenu example:
 *
 *
 * Settings
 *
 *       |
 *       +---- High Voltage
 *
 *       |
 *       +---- Low Voltage
 *
 *       |
 *       +---- Switch Delay
 *
 *
 * At that time:
 *
 * menu_settings.child
 *
 * will point to the first
 * settings item.
 *
 */



/******************************************************************************
 *                         Version History
 ******************************************************************************/

/*
 *
 * Version 1.2.0
 *
 * Changes:
 *
 *      - Removed circular navigation
 *
 *      - Added fixed menu boundaries
 *
 *      - Added renderer title interface
 *
 *      - Added item row interface
 *
 *      - Prepared framework for
 *        incremental LCD rendering
 *
 *
 */



/******************************************************************************
 *                              End of File
 ******************************************************************************/
