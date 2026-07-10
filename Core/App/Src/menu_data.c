/******************************************************************************
 * @file    menu_data.c
 * @brief   Menu Data Layer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file contains the complete menu tree definition.
 *
 *      This module is responsible only for storing menu structure.
 *
 *      Navigation logic is implemented in:
 *
 *              menu_engine.c
 *
 *      Display logic is implemented in:
 *
 *              menu_renderer.c
 *
 *
 *      Architecture:
 *
 *
 *              menu_data.c
 *
 *                    |
 *                    |
 *          +---------+---------+
 *          |                   |
 *          v                   v
 *
 *    menu_engine.c      menu_renderer.c
 *
 *
 *
 * Responsibilities:
 *
 *      - Create menu objects.
 *      - Define menu hierarchy.
 *      - Provide read-only access to menu data.
 *
 *
 * This module DOES NOT:
 *
 *      - Read buttons.
 *      - Control LCD.
 *      - Execute hardware operations.
 *      - Handle navigation.
 *
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      Rule 1:
 *
 *          All menu objects are constant.
 *
 *
 *      Rule 2:
 *
 *          Other modules must not modify menu data.
 *
 *
 *      Rule 3:
 *
 *          Menu text and actions are separated.
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

#include "menu_data.h"

#include "menu_actions.h"

#include <stddef.h>
#include <string.h>





/******************************************************************************
 *                         Private Menu Definitions
 ******************************************************************************/

/*
 * Forward declarations.
 *
 * Menu objects are referenced before their
 * complete definitions.
 */

static const Menu_t menu_main;



/******************************************************************************
 *                              Main Menu Items
 ******************************************************************************/

/*
 * Main Menu structure:
 *
 *
 *          Main Menu
 *
 *              |
 *      +-------+--------+---------+------------+
 *      |                |         |            |
 *      v                v         v            v
 *
 * Live Monitor   Start Stream  Settings   System Info
 *
 */



static const MenuItem_t menu_main_items[] =
{

    {
        .text = "Live Monitor",

        .type = MENU_ITEM_ACTION,

        .action = MenuAction_LiveMonitor,

        .submenu = NULL
    },


    {
        .text = "Start Stream",

        .type = MENU_ITEM_ACTION,

        .action = MenuAction_StartStream,

        .submenu = NULL
    },


    {
        .text = "Settings",

        .type = MENU_ITEM_SUBMENU,

        .action = NULL,

        .submenu = NULL
    },


    {
        .text = "System Info",

        .type = MENU_ITEM_ACTION,

        .action = MenuAction_SystemInfo,

        .submenu = NULL
    }

};




/******************************************************************************
 *                              Settings Menu
 ******************************************************************************/

/*
 * Settings submenu.
 *
 * Future expansion:
 *
 *      Settings
 *
 *          |
 *          +-- Voltage Limit
 *          |
 *          +-- Switch Time
 *          |
 *          +-- Alarm
 *
 *
 * Currently the submenu is prepared
 * for future application settings.
 *
 */



static const MenuItem_t menu_settings_items[] =
{

    {
        .text = "Voltage Limit",

        .type = MENU_ITEM_ACTION,

        .action = MenuAction_VoltageSetting,

        .submenu = NULL
    },


    {
        .text = "Switch Time",

        .type = MENU_ITEM_ACTION,

        .action = MenuAction_TimeSetting,

        .submenu = NULL
    },


    {
        .text = "Alarm",

        .type = MENU_ITEM_ACTION,

        .action = MenuAction_AlarmSetting,

        .submenu = NULL
    }

};




/******************************************************************************
 *                              Menu Objects
 ******************************************************************************/

/*
 * Main menu object.
 */

const Menu_t menu_main =
{

    .title = "Main Menu",


    .items = menu_main_items,


    .item_count =
            sizeof(menu_main_items) /
            sizeof(menu_main_items[0]),


    .parent = NULL

};




/*
 * Settings menu object.
 */

static const Menu_t menu_settings =
{

    .title = "Settings",


    .items = menu_settings_items,


    .item_count =
            sizeof(menu_settings_items) /
            sizeof(menu_settings_items[0]),


    .parent = &menu_main

};


/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Get root menu.
 *
 * @details
 *
 *      Returns the entry point of the
 *      complete application menu tree.
 *
 *
 * @return
 *
 *      Pointer to main menu.
 *
 */
const Menu_t *MenuData_GetRoot(void)
{
    return &menu_main;
}



/**
 * @brief
 *      Find menu item by text.
 *
 * @details
 *
 *      Searches all available menu items.
 *
 *      The current implementation searches
 *      the main menu and known submenus.
 *
 *      For larger systems this function
 *      can be replaced with ID based lookup.
 *
 */
const MenuItem_t *MenuData_FindItem(
        const char *text)
{
    uint8_t index;



    if(text == NULL)
    {
        return NULL;
    }



    /*
     * Search main menu items.
     */
    for(index = 0U;
        index < menu_main.item_count;
        index++)
    {
        if(strcmp(menu_main.items[index].text,
                  text) == 0)
        {
            return &menu_main.items[index];
        }
    }



    /*
     * Search settings menu items.
     */
    for(index = 0U;
        index < menu_settings.item_count;
        index++)
    {
        if(strcmp(menu_settings.items[index].text,
                  text) == 0)
        {
            return &menu_settings.items[index];
        }
    }



    return NULL;
}





/**
 * @brief
 *      Get number of items inside menu.
 *
 */
uint8_t MenuData_GetItemCount(
        const Menu_t *menu)
{
    if(menu == NULL)
    {
        return 0U;
    }


    return menu->item_count;
}





/**
 * @brief
 *      Get menu item by index.
 *
 */
const MenuItem_t *MenuData_GetItem(
        const Menu_t *menu,
        uint8_t index)
{

    if(menu == NULL)
    {
        return NULL;
    }



    if(index >= menu->item_count)
    {
        return NULL;
    }



    return &menu->items[index];
}





/**
 * @brief
 *      Validate menu structure.
 *
 * @details
 *
 *      Performs basic safety checks.
 *
 *      This function is mainly used during
 *      development.
 *
 */
bool MenuData_Validate(void)
{

    uint8_t index;



    /*
     * Main menu must exist.
     */
    if(menu_main.items == NULL)
    {
        return false;
    }



    if(menu_main.item_count == 0U)
    {
        return false;
    }



    /*
     * Check main menu items.
     */
    for(index = 0U;
        index < menu_main.item_count;
        index++)
    {

        if(menu_main.items[index].text == NULL)
        {
            return false;
        }

    }



    /*
     * Check settings menu.
     */
    if(menu_settings.items == NULL)
    {
        return false;
    }



    if(menu_settings.item_count == 0U)
    {
        return false;
    }



    return true;
}



/******************************************************************************
 *                              End Of File
 ******************************************************************************/
