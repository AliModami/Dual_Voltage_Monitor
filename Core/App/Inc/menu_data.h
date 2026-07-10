/******************************************************************************
 * @file    menu_data.h
 * @brief   Menu Data Layer Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file defines the public interface of the menu data layer.
 *
 *      The purpose of this module is to separate the menu structure
 *      definition from menu navigation and rendering logic.
 *
 *
 *      Architecture:
 *
 *
 *              +----------------+
 *              |   menu.h       |
 *              |----------------|
 *              | MenuItem_t     |
 *              | Menu_t         |
 *              | MenuState_t    |
 *              +----------------+
 *                       |
 *                       v
 *
 *              +----------------+
 *              | menu_data.h    |
 *              +----------------+
 *                       |
 *                       v
 *
 *              +----------------+
 *              | menu_data.c    |
 *              +----------------+
 *                       |
 *          +------------+------------+
 *          |                         |
 *          v                         v
 *   menu_engine.c            menu_renderer.c
 *
 *
 *
 * Responsibilities:
 *
 *      This module provides:
 *
 *          - Access to root menu.
 *          - Access to submenu objects.
 *          - Menu item searching.
 *          - Menu tree validation.
 *
 *
 * This module DOES NOT:
 *
 *          - Read buttons.
 *          - Control LCD.
 *          - Execute hardware functions.
 *          - Change application states.
 *
 *
 * Design reason:
 *
 *      Keeping menu data separated from logic allows:
 *
 *          1. Changing menu structure without modifying engine code.
 *
 *          2. Reusing the renderer with different menus.
 *
 *          3. Keeping application actions independent.
 *
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      Rule 1:
 *
 *          All menu objects are created inside menu_data.c.
 *
 *
 *      Rule 2:
 *
 *          Other modules access menu objects only through
 *          the functions declared in this file.
 *
 *
 *      Rule 3:
 *
 *          Menu structure is treated as read-only.
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


#ifndef MENU_DATA_H
#define MENU_DATA_H



#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

/*
 * menu.h is the single owner of:
 *
 *      - MenuItem_t
 *      - Menu_t
 *      - MenuAction_t
 *
 *
 * This prevents duplicate type definitions
 * in different modules.
 */
#include "menu.h"



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Get root menu.
 *
 * @details
 *
 *      The root menu is the first menu displayed
 *      after system startup.
 *
 *
 *      Example:
 *
 *
 *              Main Menu
 *
 *              +----------------+
 *              | Live Monitor   |
 *              | Start Stream   |
 *              | Settings       |
 *              | System Info    |
 *              +----------------+
 *
 *
 * @return
 *
 *      Pointer to root menu object.
 *
 */
const Menu_t *MenuData_GetRoot(void);





/**
 * @brief
 *      Find menu item by text identifier.
 *
 * @details
 *
 *      This function searches inside the complete
 *      menu tree.
 *
 *
 *      It is mainly intended for:
 *
 *          - Debugging.
 *          - Testing.
 *          - Future configuration systems.
 *
 *
 * @param text
 *
 *      Menu item text.
 *
 *
 * @return
 *
 *      Pointer to menu item.
 *
 *      NULL if item does not exist.
 *
 */
const MenuItem_t *MenuData_FindItem(
        const char *text);





/**
 * @brief
 *      Get number of items inside a menu.
 *
 * @param menu
 *
 *      Menu object.
 *
 *
 * @return
 *
 *      Number of menu items.
 *
 */
uint8_t MenuData_GetItemCount(
        const Menu_t *menu);





/**
 * @brief
 *      Get menu item by index.
 *
 * @details
 *
 *      Navigation engine uses this function
 *      to move between menu items.
 *
 *
 * @param menu
 *
 *      Current menu.
 *
 *
 * @param index
 *
 *      Item index.
 *
 *
 * @return
 *
 *      Pointer to menu item.
 *
 */
const MenuItem_t *MenuData_GetItem(
        const Menu_t *menu,
        uint8_t index);





/**
 * @brief
 *      Validate menu structure.
 *
 * @details
 *
 *      Checks:
 *
 *          - NULL menu pointers.
 *          - Empty menu entries.
 *          - Invalid item count.
 *          - Broken submenu relations.
 *
 *
 *      This function is mainly useful during
 *      development and debugging.
 *
 *
 * @return
 *
 *      true:
 *          Menu structure is valid.
 *
 *
 *      false:
 *          Menu contains an error.
 *
 */
bool MenuData_Validate(void);



/******************************************************************************
 *                         External Menu Objects
 ******************************************************************************/

/*
 * Main application menu.
 *
 * Defined inside menu_data.c
 *
 * This object is read-only.
 */
extern const Menu_t menu_main;



#ifdef __cplusplus
}
#endif



#endif /* MENU_DATA_H */
