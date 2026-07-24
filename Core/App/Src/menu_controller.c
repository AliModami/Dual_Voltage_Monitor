/******************************************************************************
 *
 * @file    menu_controller.c
 *
 * @brief   Page Based Menu Controller Implementation
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
 *      This file implements the runtime logic of the Page Based Menu Engine.
 *
 *      Responsibilities:
 *
 *      - Manage current page.
 *      - Manage selected item.
 *      - Handle cursor movement.
 *      - Handle page navigation.
 *      - Handle ENTER and BACK operations.
 *
 *      This module has no dependency on:
 *
 *      - LCD driver
 *      - Button driver
 *      - Hardware layer
 *
 ******************************************************************************/

#include "menu_controller.h"
#include "menu_edit.h"


/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


/*
 * Current runtime state of menu controller.
 */
static MenuControllerState_t menu_state;

/*
 * Remember last selected item
 * for every menu page.
 */
static uint8_t page_cursor[MENU_PAGE_COUNT];


/*
 * ============================================================================
 * Private Helper Functions
 * ============================================================================
 */


/*
 * Reset cursor position after changing page.
 *
 * Rule:
 *
 * Every new page starts with first item selected.
 */
static void MenuController_ResetCursor(void)
{
    menu_state.selected_item = 0U;
}




/*
 * Get current page safely.
 *
 * Returns:
 *
 *      Valid page pointer
 *
 * or:
 *
 *      NULL pointer
 */
static const MenuPage_t *MenuController_GetPage(void)
{
    return MenuItems_GetPage(menu_state.current_page);
}




/*
 * Validate selected cursor position.
 *
 * If current selection is outside
 * page item range, reset it.
 */
static void MenuController_ValidateCursor(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if (page == 0)
    {
        menu_state.selected_item = 0U;
        return;
    }



    if (menu_state.selected_item >= page->item_count)
    {
        menu_state.selected_item = 0U;
    }
}




/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */



/*
 * Initialize menu controller.
 *
 * Initial state:
 *
 *      Page:
 *          MAIN MENU PAGE 0
 *
 *      Cursor:
 *          First item
 */
void MenuController_Init(void)
{

    menu_state.current_page = MENU_PAGE_MAIN_0;


    menu_state.selected_item = 0U;
    for (uint8_t i = 0; i < MENU_PAGE_COUNT; i++)
    {
        page_cursor[i] = 0U;
    }

}




/*
 * Move cursor upward.
 *
 * Rules:
 *
 * - Cursor decreases.
 * - No wrap-around.
 * - Stops at first item.
 */



/*
 * Move cursor upward.
 *
 * Rules:
 *
 * - Cursor decreases.
 * - No wrap-around inside page.
 * - If cursor is already on first item
 *   and previous page exists:
 *
 *       Move to previous page
 *       Select last item
 *
 */
void MenuController_MoveUp(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if (page == 0)
    {
        return;
    }



    MenuController_ValidateCursor();



    /*
     * Normal cursor movement.
     */
    if (menu_state.selected_item > 0U)
    {
        menu_state.selected_item--;
        page_cursor[menu_state.current_page] =
                menu_state.selected_item;

        return;
    }



    /*
     * Cursor is already at first item.
     *
     * Check previous page.
     */
    if (page->previous_page != MENU_INVALID_PAGE)
    {

        menu_state.current_page =
                page->previous_page;


        page = MenuController_GetPage();



        if (page != 0)
        {

            /*
             * Select last item
             * of previous page.
             */
            menu_state.selected_item =
                    page->item_count - 1U;

        }
        else
        {

            menu_state.selected_item = 0U;

        }

    }

}




/*
 * Move cursor downward.
 *
 * Rules:
 *
 * - Cursor increases.
 * - No wrap-around.
 * - Stops at last item.
 */



/*
 * Move cursor downward.
 *
 * Rules:
 *
 * - Cursor increases.
 * - No wrap-around inside page.
 * - If cursor reaches last item
 *   and next page exists:
 *
 *       Move to next page
 *       Select first item
 *
 */
void MenuController_MoveDown(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if (page == 0)
    {
        return;
    }



    MenuController_ValidateCursor();



    /*
     * Normal cursor movement.
     */
    if (menu_state.selected_item <
        (page->item_count - 1U))
    {

        menu_state.selected_item++;
        page_cursor[menu_state.current_page] =
                menu_state.selected_item;
        return;

    }



    /*
     * Cursor is already on last item.
     *
     * Check next page.
     */
    if (page->next_page != MENU_INVALID_PAGE)
    {

        menu_state.current_page =
                page->next_page;


        MenuController_ResetCursor();

    }

}

/*
 * ============================================================================
 * Page Navigation
 * ============================================================================
 *
 * Page navigation is used only for logical pages.
 *
 * Example:
 *
 * MAIN PAGE 0
 *       |
 *       v
 * MAIN PAGE 1
 *       |
 *       v
 * MAIN PAGE 2
 *
 * Cursor resets after changing page.
 *
 * ============================================================================
 */


/*
 * Move to next page.
 */
void MenuController_NextPage(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if (page == 0)
    {
        return;
    }



    if (page->next_page != MENU_INVALID_PAGE)
    {

        menu_state.current_page = page->next_page;


        MenuController_ResetCursor();

    }

}




/*
 * Move to previous page.
 */
void MenuController_PreviousPage(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if (page == 0)
    {
        return;
    }



    if (page->previous_page != MENU_INVALID_PAGE)
    {

        menu_state.current_page = page->previous_page;


        MenuController_ResetCursor();

    }

}




/*
 * ============================================================================
 * ENTER Operation
 * ============================================================================
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
 * ============================================================================
 */


void MenuController_Enter(void)
{
    const MenuItem_t *item;


    item = MenuController_GetSelectedItem();



    if (item == 0)
    {
        return;
    }




    /*
     * Open submenu page.
     */
    if (item->type == MENU_ITEM_SUBMENU)
    {

        if (item->child_page != MENU_INVALID_PAGE)
        {

        	page_cursor[menu_state.current_page] =
        	        menu_state.selected_item;

        	menu_state.current_page =
        	        item->child_page;

        	menu_state.selected_item =
        	        page_cursor[menu_state.current_page];

        }


        return;
    }




    /*
     * Execute action callback.
     */
    if (item->action != 0)
    {
        item->action();
    }

}




/*
 * ============================================================================
 * BACK Operation
 * ============================================================================
 *
 * Behavior:
 *
 * - Return to parent page.
 * - Reset cursor.
 *
 * Example:
 *
 * Alarm Settings Page 1
 *
 *          BACK
 *
 *          |
 *          v
 *
 * Alarm Settings Page 0
 *
 * ============================================================================
 */


void MenuController_Back(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if (page == 0)
    {
        return;
    }




    /*
     * Root page has no parent.
     */
    if (page->parent_page == MENU_INVALID_PAGE)
    {
        return;
    }



    menu_state.current_page =
            page->parent_page;

    menu_state.selected_item =
            page_cursor[menu_state.current_page];

}

/*
 * ============================================================================
 * Getter Functions
 * ============================================================================
 *
 * These functions provide read-only access to the current
 * menu state for external modules.
 *
 * Used by:
 *
 *      menu_renderer.c
 *
 * The renderer does not modify menu state.
 *
 * ============================================================================
 */



/*
 * Get current page information.
 *
 * Returns:
 *
 *      Pointer to current MenuPage_t
 *
 *      NULL if page is invalid.
 */
const MenuPage_t *MenuController_GetCurrentPage(void)
{

    return MenuController_GetPage();

}




/*
 * Get current selected item index.
 *
 * Used by renderer to draw cursor.
 */
uint8_t MenuController_GetSelectedIndex(void)
{

    return menu_state.selected_item;

}




/*
 * Get selected menu item.
 *
 * Returns:
 *
 *      Pointer to selected MenuItem_t
 *
 *      NULL if selection is invalid.
 */
const MenuItem_t *MenuController_GetSelectedItem(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if (page == 0)
    {
        return 0;
    }




    /*
     * Safety check.
     */
    if (menu_state.selected_item >= page->item_count)
    {
        return 0;
    }



    return &page->items[menu_state.selected_item];

}




/*
 * Get current page identifier.
 */
MenuPageId_t MenuController_GetCurrentPageId(void)
{

    return menu_state.current_page;

}




/*
 * ============================================================================
 * End Of File
 * ============================================================================
 */
