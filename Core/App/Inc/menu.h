/******************************************************************************
 * @file    menu.h
 * @brief   Menu Framework Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This module defines the public interface of the menu framework.
 *
 *      The menu system is independent from hardware buttons.
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
 *      The menu receives application commands and changes the
 *      current menu state accordingly.
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

/**
 * @brief
 *      Current application display mode.
 *
 *      Each mode represents a different application screen.
 */

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

/**
 * @brief
 *      Function type for menu item actions.
 *
 *      Each menu item can optionally execute an action.
 */

typedef void (*MenuAction_t)(void);



/******************************************************************************
 *                              Menu Item
 ******************************************************************************/

/**
 * @brief
 *      Menu item structure.
 *
 *      The menu is designed as a tree.
 *
 *      Example:
 *
 *              Main Menu
 *                  |
 *          ----------------
 *          |              |
 *       Monitor       Settings
 *
 */

typedef struct MenuItem
{
    const char *name;


    /*
     * Function executed when item is selected.
     */

    MenuAction_t action;


    /*
     * Tree navigation pointers.
     */

    struct MenuItem *parent;

    struct MenuItem *child;

    struct MenuItem *next;

    struct MenuItem *prev;


} MenuItem_t;



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu framework.
 */
void Menu_Init(void);



/**
 * @brief
 *      Execute menu processing task.
 *
 * @details
 *      This function must be called periodically
 *      from the main loop.
 */
void Menu_Task(void);



/**
 * @brief
 *      Process application button command.
 *
 * @param command
 *      Button application command.
 */
void Menu_ProcessCommand(Button_AppCommand_t command);



/**
 * @brief
 *      Get current menu mode.
 *
 * @return
 *      Current menu mode.
 */
Menu_Mode_t Menu_GetMode(void);



#ifdef __cplusplus
}
#endif


#endif /* MENU_H */
