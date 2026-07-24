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
 *      Runtime controller for page based menu system.
 *
 *      Responsibilities:
 *
 *      - Manage current page.
 *      - Manage cursor position.
 *      - Handle UP / DOWN navigation.
 *      - Handle ENTER operation.
 *      - Handle BACK operation.
 *
 *      Design rules:
 *
 *      - No LCD dependency.
 *      - No button dependency.
 *      - No hardware dependency.
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
 * Runtime menu state.
 */
static MenuControllerState_t menu_state;


/*
 * Cursor memory for each page.
 */
static uint8_t page_cursor[MENU_PAGE_COUNT];



/*
 * ============================================================================
 * Private Helper Functions
 * ============================================================================
 */


/*
 * Return current page pointer.
 */
static const MenuPage_t *MenuController_GetPage(void)
{
    return MenuItems_GetPage(menu_state.current_page);
}




/*
 * Reset cursor position.
 */
static void MenuController_ResetCursor(void)
{
    menu_state.selected_item = 0U;
}




/*
 * Validate cursor position.
 */
static void MenuController_ValidateCursor(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if(page == 0)
    {
        menu_state.selected_item = 0U;
        return;
    }


    if(menu_state.selected_item >= page->item_count)
    {
        menu_state.selected_item = 0U;
    }

}




/*
 * Save cursor of current page.
 */
static void MenuController_SaveCursor(void)
{
    if(menu_state.current_page < MENU_PAGE_COUNT)
    {
        page_cursor[menu_state.current_page] =
                menu_state.selected_item;
    }
}




/*
 * Restore cursor of current page.
 */
static void MenuController_RestoreCursor(void)
{
    if(menu_state.current_page < MENU_PAGE_COUNT)
    {
        menu_state.selected_item =
                page_cursor[menu_state.current_page];
    }


    MenuController_ValidateCursor();
}




/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */


/*
 * Initialize controller.
 */
void MenuController_Init(void)
{

    menu_state.current_page =
            MENU_PAGE_MAIN_0;


    menu_state.selected_item = 0U;


    for(uint8_t i = 0U;
        i < MENU_PAGE_COUNT;
        i++)
    {
        page_cursor[i] = 0U;
    }

}



/*
 * ============================================================================
 * Cursor Navigation
 * ============================================================================
 */


/*
 * Move cursor up.
 */
void MenuController_MoveUp(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if(page == 0)
    {
        return;
    }


    MenuController_ValidateCursor();



    /*
     * Move inside current page.
     */
    if(menu_state.selected_item > 0U)
    {
        menu_state.selected_item--;

        MenuController_SaveCursor();

        return;
    }



    /*
     * Page transition is allowed only
     * when previous page exists.
     */
    if(page->previous_page != MENU_INVALID_PAGE)
    {

        MenuController_SaveCursor();


        menu_state.current_page =
                page->previous_page;


        page = MenuController_GetPage();


        if(page != 0)
        {
            menu_state.selected_item =
                    page->item_count - 1U;

            MenuController_SaveCursor();
        }

    }

}

/*
 * ============================================================================
 * Move cursor down.
 * ============================================================================
 *
 * Behavior:
 *
 * - Move inside current page.
 * - If current page has next logical page:
 *      move to next page only when
 *      cursor is already on last item.
 *
 * ============================================================================
 */

void MenuController_MoveDown(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if(page == 0)
    {
        return;
    }


    MenuController_ValidateCursor();



    /*
     * Normal movement inside current page.
     */
    if(menu_state.selected_item <
       (page->item_count - 1U))
    {

        menu_state.selected_item++;

        MenuController_SaveCursor();

        return;

    }




    /*
     * Cursor is already on last item.
     *
     * Move to next page only if
     * current page belongs to a page chain.
     *
     */
    if(page->next_page != MENU_INVALID_PAGE)
    {

        MenuController_SaveCursor();



        menu_state.current_page =
                page->next_page;



        MenuController_ResetCursor();



        MenuController_SaveCursor();

    }

}



/*
 * ============================================================================
 * Page Navigation
 * ============================================================================
 */


/*
 * Move to next page manually.
 *
 * Used for logical page chains.
 */
void MenuController_NextPage(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if(page == 0)
    {
        return;
    }



    if(page->next_page != MENU_INVALID_PAGE)
    {

        MenuController_SaveCursor();



        menu_state.current_page =
                page->next_page;



        MenuController_ResetCursor();



        MenuController_SaveCursor();

    }

}




/*
 * Move to previous page manually.
 */
void MenuController_PreviousPage(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if(page == 0)
    {
        return;
    }



    if(page->previous_page != MENU_INVALID_PAGE)
    {

        MenuController_SaveCursor();



        menu_state.current_page =
                page->previous_page;



        MenuController_RestoreCursor();

    }

}




/*
 * ============================================================================
 * ENTER Operation
 * ============================================================================
 *
 * Priority:
 *
 * 1- Edit mode confirmation.
 *
 * 2- Open submenu.
 *
 * 3- Execute action callback.
 *
 * ============================================================================
 */


void MenuController_Enter(void)
{

    const MenuItem_t *item;



    /*
     * Editing has priority.
     */
    if(MenuEdit_IsActive())
    {

        MenuEdit_Confirm();

        return;

    }




    item = MenuController_GetSelectedItem();



    if(item == 0)
    {
        return;
    }




    /*
     * Open child page.
     */
    if(item->type == MENU_ITEM_SUBMENU)
    {

        if(item->child_page != MENU_INVALID_PAGE)
        {

            /*
             * Store parent cursor.
             */
            MenuController_SaveCursor();



            /*
             * Enter child page.
             */
            menu_state.current_page =
                    item->child_page;



            /*
             * Restore child cursor.
             */
            MenuController_RestoreCursor();

        }


        return;

    }




    /*
     * Execute action.
     */
    if(item->action != 0)
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
 * Edit Mode:
 *
 *      Cancel current editing.
 *
 *
 * Normal Mode:
 *
 *      Return to parent page.
 *
 *      Restore previous cursor position.
 *
 * ============================================================================
 */


void MenuController_Back(void)
{

    const MenuPage_t *page;



    /*
     * If value editing is active,
     * BACK cancels editing only.
     */
    if(MenuEdit_IsActive())
    {

        MenuEdit_Cancel();

        return;

    }



    page = MenuController_GetPage();



    if(page == 0)
    {
        return;
    }




    /*
     * Root page has no parent.
     */
    if(page->parent_page == MENU_INVALID_PAGE)
    {
        return;
    }




    /*
     * Save current page cursor.
     */
    MenuController_SaveCursor();




    /*
     * Move to parent page.
     */
    menu_state.current_page =
            page->parent_page;




    /*
     * Restore parent cursor.
     */
    MenuController_RestoreCursor();

}



/*
 * ============================================================================
 * Getter Functions
 * ============================================================================
 *
 * These functions provide read-only access
 * to controller state.
 *
 * Used by:
 *
 *      menu_renderer.c
 *
 * ============================================================================
 */



/*
 * Return current page information.
 */
const MenuPage_t *MenuController_GetCurrentPage(void)
{

    return MenuController_GetPage();

}




/*
 * Return selected item index.
 */
uint8_t MenuController_GetSelectedIndex(void)
{

    return menu_state.selected_item;

}




/*
 * Return selected item information.
 */
const MenuItem_t *MenuController_GetSelectedItem(void)
{

    const MenuPage_t *page;



    page = MenuController_GetPage();



    if(page == 0)
    {
        return 0;
    }




    if(menu_state.selected_item >=
       page->item_count)
    {
        return 0;
    }




    return &page->items[menu_state.selected_item];

}




/*
 * Return current page identifier.
 */
MenuPageId_t MenuController_GetCurrentPageId(void)
{

    return menu_state.current_page;

}


/*
 * ============================================================================
 * End Of File Notes
 * ============================================================================
 *
 * This version implements:
 *
 * - Page based navigation.
 * - Cursor memory for every page.
 * - Three visible items per page support.
 * - ENTER submenu handling.
 * - BACK parent navigation.
 * - Edit mode priority.
 * - No LCD dependency.
 * - No button dependency.
 *
 *
 * Navigation Example:
 *
 *
 * MAIN MENU PAGE 0
 *
 *   Live Monitor
 *   Start Stream
 *   Stream Settings
 *
 *
 *              DOWN
 *
 *
 * MAIN MENU PAGE 1
 *
 *   Alarm Settings
 *   Calibration
 *   Service Mode
 *
 *
 *
 * BACK Example:
 *
 *
 * Stream Settings
 *
 *        BACK
 *
 *          |
 *          v
 *
 * Main Menu Page 0
 *
 *
 *
 * Edit Example:
 *
 *
 * Baud Rate
 *
 *        ENTER
 *
 *          |
 *          v
 *
 * Edit Mode Active
 *
 *
 *        BACK
 *
 *          |
 *          v
 *
 * Cancel Edit
 *
 *
 *        ENTER
 *
 *          |
 *          v
 *
 * Confirm Value
 *
 *
 * ============================================================================
 */


/*
 * No additional code required.
 */


/*
 * ============================================================================
 * End Of File
 * ============================================================================
 */
