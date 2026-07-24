/******************************************************************************
 *
 * @file    menu_controller.h
 *
 * @brief   Page Based Menu Controller Interface
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor - Page Based Menu Prototype
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file defines the public interface of the Page Based Menu Engine.
 *
 *      Responsibilities:
 *
 *      - Manage current page.
 *      - Manage selected item.
 *      - Handle UP / DOWN navigation.
 *      - Handle ENTER and BACK operations.
 *      - Provide menu state information for renderer.
 *
 *      This module has no dependency on:
 *
 *      - LCD driver
 *      - Button driver
 *      - Hardware layer
 *
 ******************************************************************************/

#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "menu_items.h"



/*
 * Menu controller state.
 *
 * This structure contains runtime information only.
 */
typedef struct
{

    /*
     * Current active page.
     */
    MenuPageId_t current_page;


    /*
     * Current cursor position inside page.
     *
     * Range:
     *
     * 0 ... item_count-1
     */
    uint8_t selected_item;


} MenuControllerState_t;



/*
 * Initialize menu controller.
 *
 * Initial state:
 *
 * Page:
 *      MAIN MENU PAGE 0
 *
 * Cursor:
 *      First item
 *
 */
void MenuController_Init(void);



/*
 * Move cursor up.
 *
 * Rules:
 *
 * - Cursor decreases.
 * - Stops at first item.
 * - No wrap around.
 *
 */
void MenuController_MoveUp(void);



/*
 * Move cursor down.
 *
 * Rules:
 *
 * - Cursor increases.
 * - Stops at last item.
 * - No wrap around.
 *
 */
void MenuController_MoveDown(void);



/*
 * Move to next page.
 *
 * Used for Page Based navigation.
 *
 * Example:
 *
 * MAIN MENU PAGE 0
 *        |
 *        v
 * MAIN MENU PAGE 1
 *
 */
void MenuController_NextPage(void);



/*
 * Move to previous page.
 *
 */
void MenuController_PreviousPage(void);



/*
 * ENTER button handler.
 *
 * Behavior:
 *
 * MENU_ITEM_SUBMENU:
 *
 *      Open child page
 *
 *
 * MENU_ITEM_ACTION:
 *
 *      Execute callback
 *
 */
void MenuController_Enter(void);



/*
 * BACK button handler.
 *
 * Behavior:
 *
 * - Return to parent page.
 * - Restore previous cursor position of parent page
 *
 */
void MenuController_Back(void);



/*
 * Get current page.
 *
 * Used by renderer.
 *
 */
const MenuPage_t *MenuController_GetCurrentPage(void);



/*
 * Get selected item index.
 *
 * Used by renderer for cursor drawing.
 *
 */
uint8_t MenuController_GetSelectedIndex(void);



/*
 * Get selected item information.
 *
 */
const MenuItem_t *MenuController_GetSelectedItem(void);



/*
 * Get current page identifier.
 *
 */
MenuPageId_t MenuController_GetCurrentPageId(void);



#ifdef __cplusplus
}
#endif


#endif /* MENU_CONTROLLER_H */
