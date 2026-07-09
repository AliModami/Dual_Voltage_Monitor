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
 *      1.1.0
 *
 * Created :
 *      2026-07-08
 *
 * Change History :
 *
 *      1.1.0
 *          Added read-only interface for renderer layer.
 *
 ******************************************************************************/

#ifndef MENU_H
#define MENU_H
#define MENU_HEADER_VERSION_110


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

typedef void (*MenuAction_t)(void);



/******************************************************************************
 *                              Menu Item
 ******************************************************************************/

typedef struct MenuItem
{
    const char *name;

    MenuAction_t action;

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
 *      Execute periodic menu task.
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
 *      Return current application mode.
 *
 * @return
 *      Current mode.
 */
Menu_Mode_t Menu_GetMode(void);



/******************************************************************************
 * @brief
 *      Return currently selected menu item.
 *
 * @details
 *      This function is read-only.
 *
 *      It allows renderer modules to know which menu
 *      item is currently selected without modifying
 *      the internal menu state.
 *
 * @return
 *      Pointer to current menu item.
 ******************************************************************************/
const MenuItem_t *Menu_GetCurrentItem(void);



/******************************************************************************
 * @brief
 *      Return root menu item.
 *
 * @details
 *      Renderer modules use this function to obtain
 *      the title of the current menu tree.
 *
 * @return
 *      Pointer to root menu item.
 ******************************************************************************/
const MenuItem_t *Menu_GetRootItem(void);



/******************************************************************************
 *                      Read Only Renderer Interface
 ******************************************************************************/

/**
 * @brief
 *      Return currently selected menu item.
 *
 * @details
 *      This function provides read-only access for
 *      modules such as Menu Renderer.
 *
 *      Ownership remains inside menu.c.
 *
 * @return
 *      Pointer to current menu item.
 */
const MenuItem_t *Menu_GetCurrentItem(void);



/**
 * @brief
 *      Return root menu item.
 *
 * @details
 *      Used by renderer or future diagnostics.
 *
 * @return
 *      Pointer to root menu.
 */
const MenuItem_t *Menu_GetRootItem(void);



#ifdef __cplusplus
}
#endif


#endif /* MENU_H */
