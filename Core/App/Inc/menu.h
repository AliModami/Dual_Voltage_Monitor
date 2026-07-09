/******************************************************************************
 * @file    menu.h
 * @brief   Menu Framework Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file defines the public interface of the menu framework.
 *
 *      The menu system is independent from:
 *
 *          - Hardware buttons
 *          - LCD display
 *          - Application peripherals
 *
 *
 *      Input flow:
 *
 *          Button Driver
 *                |
 *                v
 *          Button Application
 *                |
 *                v
 *          Menu Framework
 *
 *
 *      Display flow:
 *
 *          Menu Framework
 *                |
 *                v
 *          Menu Renderer
 *                |
 *                v
 *          LCD Driver
 *
 *-----------------------------------------------------------------------------
 *
 * Design Rules:
 *
 *      1. Menu owns navigation state.
 *
 *      2. Renderer only reads menu information.
 *
 *      3. Menu never accesses LCD hardware.
 *
 *      4. Hardware events are converted into commands
 *         before reaching this module.
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.2.0
 *
 * Change History :
 *
 *      1.2.0
 *
 *          Added read-only interface for incremental
 *          LCD renderer.
 *
 ******************************************************************************/


#ifndef MENU_H
#define MENU_H



#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "button_app.h"



/******************************************************************************
 *                              Menu Modes
 ******************************************************************************/

typedef enum
{
    MENU_MODE = 0,

    MONITOR_MODE,

    STREAM_MODE,

    SETTINGS_MODE,

    SYSTEM_INFO_MODE


} Menu_Mode_t;



/******************************************************************************
 *                              Menu Actions
 ******************************************************************************/

/*
 * Function pointer used by menu items.
 *
 * When ENTER is pressed, the action
 * belonging to the selected item executes.
 */

typedef void (*MenuAction_t)(void);



/******************************************************************************
 *                              Menu Item Structure
 ******************************************************************************/

typedef struct MenuItem
{
    /*
     * Text displayed on LCD.
     */

    const char *name;



    /*
     * Function executed when ENTER
     * is pressed.
     */

    MenuAction_t action;



    /*
     * Parent menu item.
     */

    struct MenuItem *parent;



    /*
     * First child item.
     */

    struct MenuItem *child;



    /*
     * Next item in same menu level.
     */

    struct MenuItem *next;



    /*
     * Previous item in same menu level.
     */

    struct MenuItem *prev;


} MenuItem_t;



/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu framework.
 */
void Menu_Init(void);



/**
 * @brief
 *      Execute periodic menu task.
 */
void Menu_Task(void);



/**
 * @brief
 *      Process command received from button layer.
 *
 * @param command
 *      Button application command.
 */
void Menu_ProcessCommand(Button_AppCommand_t command);



/**
 * @brief
 *      Get current application mode.
 *
 * @return
 *      Current menu mode.
 */
Menu_Mode_t Menu_GetMode(void);



/******************************************************************************
 *                      Renderer Read Only Interface
 ******************************************************************************/

/**
 * @brief
 *      Get currently selected menu item.
 *
 * @details
 *
 *      This function provides read-only access
 *      to renderer modules.
 *
 *      Ownership remains inside menu.c.
 *
 * @return
 *      Pointer to selected item.
 */
const MenuItem_t *Menu_GetCurrentItem(void);



/**
 * @brief
 *      Get root menu item.
 *
 * @details
 *
 *      Renderer uses this function to obtain
 *      menu hierarchy information.
 *
 * @return
 *      Pointer to root item.
 */
const MenuItem_t *Menu_GetRootItem(void);



/**
 * @brief
 *      Get menu title.
 *
 * @details
 *
 *      The renderer uses this string as
 *      fixed LCD title line.
 *
 *      Example:
 *
 *          Main Menu
 *          Settings Menu
 *
 * @return
 *      Pointer to title string.
 */
const char *Menu_GetCurrentMenuTitle(void);



/**
 * @brief
 *      Get LCD row position of menu item.
 *
 * @details
 *
 *      Used by incremental renderer.
 *
 *      The renderer can update only the
 *      cursor position instead of refreshing
 *      the entire LCD.
 *
 *      Return value:
 *
 *          0 : Not visible / invalid
 *
 *          1 : LCD row 1
 *
 *          2 : LCD row 2
 *
 *          3 : LCD row 3
 *
 * @param item
 *      Menu item pointer.
 *
 * @return
 *      LCD row number.
 */
uint8_t Menu_GetItemRow(const MenuItem_t *item);



#ifdef __cplusplus
}
#endif



#endif /* MENU_H */
