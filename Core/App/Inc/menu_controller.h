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
 *      Dual Voltage Monitor - Page Based Menu
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file defines the public interface of the
 *      Page Based Menu Controller.
 *
 *
 *      Responsibilities:
 *
 *      - Manage current menu page.
 *      - Manage selected item.
 *      - Handle UP / DOWN navigation.
 *      - Handle ENTER operation.
 *      - Handle BACK operation.
 *      - Provide menu state information for renderer.
 *
 *
 *      Navigation model:
 *
 *      - Pages are defined in menu_items.c.
 *      - Child navigation uses child_page.
 *      - Back navigation uses parent_page.
 *      - No history stack is used.
 *
 *
 *      This module has no dependency on:
 *
 *      - LCD driver.
 *      - Button driver.
 *      - Hardware layer.
 *      - I2C communication.
 *
 *****************************************************************************/


#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H



#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>


#include "menu_items.h"




/******************************************************************************
 *
 * Menu Controller State
 *
 *****************************************************************************/


/*
 * Runtime state of menu controller.
 *
 * This structure contains only
 * active navigation state.
 *
 */
typedef struct
{

    /*
     * Current active page.
     */
    MenuPageId_t current_page;



    /*
     * Selected item index
     * inside current page.
     *
     * Range:
     *
     *      0 ... item_count-1
     *
     */
    uint8_t selected_item;



} MenuControllerState_t;





/******************************************************************************
 *
 * Initialization
 *
 *****************************************************************************/


/*
 * Initialize menu controller.
 *
 *
 * Initial state:
 *
 *      Page:
 *          MENU_PAGE_MAIN_0
 *
 *
 *      Cursor:
 *          First item
 *
 */
void MenuController_Init(void);




/******************************************************************************
 *
 * Cursor Navigation
 *
 *****************************************************************************/


/*
 * Move cursor up.
 *
 *
 * Rules:
 *
 *      - Decrease selected item.
 *      - Stop at first item.
 *      - No wrap around.
 *
 */
void MenuController_MoveUp(void);




/*
 * Move cursor down.
 *
 *
 * Rules:
 *
 *      - Increase selected item.
 *      - Stop at last item.
 *      - No wrap around.
 *
 */
void MenuController_MoveDown(void);


/******************************************************************************
 *
 * Menu Operations
 *
 *****************************************************************************/


/*
 * ENTER button handler.
 *
 *
 * Behavior:
 *
 *
 * MENU_ITEM_SUBMENU:
 *
 *      Open child page.
 *
 *
 * MENU_ITEM_ACTION:
 *
 *      Execute callback function.
 *
 */
void MenuController_Enter(void);





/*
 * BACK button handler.
 *
 *
 * Behavior:
 *
 *      - Return to parent page.
 *      - Restore parent navigation context.
 *
 *
 * Navigation source:
 *
 *      parent_page
 *
 *
 * Note:
 *
 *      No history stack is used.
 *
 */
void MenuController_Back(void);





/******************************************************************************
 *
 * Menu State Access
 *
 *****************************************************************************/


/*
 * Get current active page.
 *
 *
 * Used by:
 *
 *      menu_renderer.c
 *
 *
 * Return:
 *
 *      Pointer to current MenuPage_t.
 *
 */
const MenuPage_t *MenuController_GetCurrentPage(void);





/*
 * Get selected item index.
 *
 *
 * Used by:
 *
 *      menu_renderer.c
 *
 *
 * Return:
 *
 *      Current cursor position
 *      inside active page.
 *
 */
uint8_t MenuController_GetSelectedIndex(void);





/*
 * Get selected menu item.
 *
 *
 * Used when application needs
 * information about current selection.
 *
 *
 * Return:
 *
 *      Pointer to selected MenuItem_t
 *
 *      NULL:
 *          Invalid selection
 *
 */
const MenuItem_t *MenuController_GetSelectedItem(void);





/*
 * Get current page identifier.
 *
 *
 * Return:
 *
 *      Current MenuPageId_t value.
 *
 */
MenuPageId_t MenuController_GetCurrentPageId(void);





/******************************************************************************
 *
 * End Of Public Interface
 *
 *****************************************************************************/


#ifdef __cplusplus
}
#endif



#endif /* MENU_CONTROLLER_H */



/******************************************************************************
 *
 * File End Verification
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      Clean Final v2.2.0
 *
 *
 * Changes:
 *
 *      - Removed history stack dependency.
 *      - Removed page navigation API.
 *      - Navigation now relies on menu_items page links.
 *      - Parent page handles BACK operation.
 *      - Simplified controller interface.
 *      - Preserved renderer compatibility.
 *
 *
 * Compatible with:
 *
 *      menu_items.h
 *      menu_controller.c v2.2.0
 *      menu_renderer.c
 *
 *****************************************************************************/
