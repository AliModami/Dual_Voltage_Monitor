/******************************************************************************
 * @file    menu.c
 * @brief   Menu Framework Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file implements the application menu framework.
 *
 *      The menu module is responsible for:
 *
 *          - Managing menu tree structure
 *          - Tracking current selected item
 *          - Processing application button commands
 *          - Changing application modes
 *
 *      Hardware dependency:
 *
 *          NONE
 *
 *      Input source:
 *
 *          button_app.c
 *
 *      Data flow:
 *
 *
 *          GPIO Buttons
 *                |
 *                v
 *          buttons.c
 *                |
 *                v
 *          button_app.c
 *                |
 *                v
 *          menu.c
 *                |
 *                v
 *          Application Screens
 *
 *
 *      The menu module does not access:
 *
 *          - GPIO
 *          - LCD hardware
 *          - UART
 *          - ADC
 *
 *      This separation keeps the framework reusable.
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      1. Menu receives commands, not hardware events.
 *
 *      2. Navigation logic remains inside this module.
 *
 *      3. Display code is separated from menu logic.
 *
 *      4. Menu items are represented using a tree structure.
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 * Created :
 *      2026-07-08
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
 * Current operating mode of the application.
 *
 * At startup the system enters the main menu.
 */

static Menu_Mode_t current_mode = MENU_MODE;



/*
 * Pointer to currently selected menu item.
 *
 * This pointer moves through the menu tree
 * when UP and DOWN commands are received.
 */

static MenuItem_t *current_item = NULL;



/******************************************************************************
 *                         Private Function Prototypes
 ******************************************************************************/

/**
 * @brief
 *      Move selection to next item.
 */
static void Menu_MoveNext(void);



/**
 * @brief
 *      Move selection to previous item.
 */
static void Menu_MovePrevious(void);



/**
 * @brief
 *      Execute selected item.
 */
static void Menu_EnterItem(void);



/**
 * @brief
 *      Return to parent item.
 */
static void Menu_Back(void);



/**
 * @brief
 *      Action executed for Monitor item.
 */
static void Menu_ActionMonitor(void);



/**
 * @brief
 *      Action executed for Stream item.
 */
static void Menu_ActionStream(void);



/**
 * @brief
 *      Action executed for Settings item.
 */
static void Menu_ActionSettings(void);



/**
 * @brief
 *      Action executed for System Info item.
 */
static void Menu_ActionSystemInfo(void);

/******************************************************************************
 *                         Menu Tree Definition
 ******************************************************************************/

/*
 * Forward declarations of menu items.
 *
 * Each item contains pointers to other items.
 * Therefore, declarations are required before
 * the actual initialization.
 */

static MenuItem_t menu_root;

static MenuItem_t menu_monitor;

static MenuItem_t menu_stream;

static MenuItem_t menu_settings;

static MenuItem_t menu_system_info;



/******************************************************************************
 *                         Menu Item Initialization
 ******************************************************************************/

/*
 * Root menu item.
 *
 * This item represents the top level
 * of the menu tree.
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
 * Live Monitor menu item.
 */

static MenuItem_t menu_monitor =
{
    .name   = "Live Monitor",

    .action = Menu_ActionMonitor,

    .parent = &menu_root,

    .child  = NULL,

    .next   = &menu_stream,

    .prev   = &menu_system_info
};



/*
 * UART Stream menu item.
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
 * Settings menu item.
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
 * System information menu item.
 */

static MenuItem_t menu_system_info =
{
    .name   = "System Info",

    .action = Menu_ActionSystemInfo,

    .parent = &menu_root,

    .child  = NULL,

    .next   = &menu_monitor,

    .prev   = &menu_settings
};



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu framework.
 *
 * @details
 *
 *      At startup:
 *
 *          - Select main menu
 *          - Select first available item
 *          - Reset current mode
 *
 */
void Menu_Init(void)
{
    current_mode = MENU_MODE;


    /*
     * First selectable item.
     */

    current_item = &menu_monitor;
}



/**
 * @brief
 *      Execute periodic menu task.
 *
 * @details
 *
 *      Reserved for future operations:
 *
 *          - LCD refresh
 *          - Menu timeout
 *          - Screen management
 *
 */
void Menu_Task(void)
{

}



/**
 * @brief
 *      Process application button command.
 *
 * @param command
 *      Command generated by Button Application layer.
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
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Move selection to next menu item.
 *
 * @details
 *      The menu uses linked navigation.
 *
 *      The next pointer defines the forward direction.
 */
static void Menu_MoveNext(void)
{
    if(current_item == NULL)
    {
        return;
    }



    if(current_item->next != NULL)
    {
        current_item = current_item->next;
    }
}



/**
 * @brief
 *      Move selection to previous menu item.
 *
 * @details
 *      The previous pointer defines the reverse direction.
 */
static void Menu_MovePrevious(void)
{
    if(current_item == NULL)
    {
        return;
    }



    if(current_item->prev != NULL)
    {
        current_item = current_item->prev;
    }
}



/**
 * @brief
 *      Execute selected menu item action.
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
 *      Return to parent menu item.
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
    }
}



/******************************************************************************
 *                         Menu Item Actions
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
 *      Enter Settings screen.
 */
static void Menu_ActionSettings(void)
{
    current_mode = SETTINGS_MODE;
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
 *                         Public Mode Interface
 ******************************************************************************/

/**
 * @brief
 *      Return current application mode.
 *
 * @return
 *      Current menu/application mode.
 */
Menu_Mode_t Menu_GetMode(void)
{
    return current_mode;
}



/******************************************************************************
 *                              End of File
 ******************************************************************************/
