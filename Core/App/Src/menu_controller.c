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
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This module implements the runtime controller of the
 *      page based menu system.
 *
 *      Responsibilities:
 *
 *      - Manage active menu page.
 *      - Manage cursor position.
 *      - Handle UP / DOWN navigation.
 *      - Handle ENTER operation.
 *      - Handle BACK operation.
 *
 *
 *      This module does NOT handle:
 *
 *      - LCD rendering.
 *      - Button hardware.
 *      - Application hardware.
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
 * Current menu runtime state.
 */
static MenuControllerState_t menu_state;


/*
 * Cursor memory for each page.
 *
 * This allows returning to a page
 * with the previous selected item.
 */
static uint8_t page_cursor[MENU_PAGE_COUNT];





/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */


/*
 * Get current page object.
 */
static const MenuPage_t *MenuController_GetPage(void)
{

    return MenuItems_GetPage(
            menu_state.current_page);

}





///*
// * Reset current cursor.
// */
//static void MenuController_ResetCursor(void)
//{
//
//    menu_state.selected_item = 0U;
//
//}





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



    if(menu_state.selected_item >=
       page->item_count)
    {

        menu_state.selected_item = 0U;

    }

}





/*
 * Save current page cursor.
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
 * Restore current page cursor.
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
 * Initialize menu controller.
 */
void MenuController_Init(void)
{

    uint8_t i;


    menu_state.current_page =
            MENU_PAGE_MAIN_0;


    menu_state.selected_item = 0U;



    for(i = 0U;
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
     * Move cursor inside current page.
     */
    if(menu_state.selected_item > 0U)
    {

        menu_state.selected_item--;

        MenuController_SaveCursor();

    }
    else
    {

        /*
         * First item reached.
         *
         * Move to previous page if available.
         */
        if(page->previous_page != MENU_INVALID_PAGE)
        {

            MenuController_PreviousPage();

        }

    }

}





/*
 * Move cursor down.
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
     * Move inside current page.
     */
    if(menu_state.selected_item <
       (page->item_count - 1U))
    {

        menu_state.selected_item++;

        MenuController_SaveCursor();

    }


    /*
     * Current page last item reached.
     *
     * Move to next page if available.
     */
//    else if(page->next_page != MENU_INVALID_PAGE)
//    {
//
//        MenuController_NextPage();
//
//    }

   //------------------------------------------------------------
    else
    {
        if(page->next_page != MENU_INVALID_PAGE)
        {
            MenuController_NextPage();

            menu_state.selected_item = 0U;

            MenuController_SaveCursor();
        }
    }


    //------------------------------------------------------------




}


/*
 * ============================================================================
 * Page Navigation
 * ============================================================================
 */


/*
 * Move to next logical page.
 *
 * Used for pages containing
 * more than three items.
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



//        MenuController_RestoreCursor();

 //--------------------------------------------------------
        menu_state.selected_item = 0U;

        MenuController_SaveCursor();
 //---------------------------------------------------



    }

}





/*
 * Move to previous logical page.
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
 * 1- Confirm active edit operation.
 *
 * 2- Open child menu page.
 *
 * 3- Execute action callback.
 *
 */
void MenuController_Enter(void)
{

    const MenuItem_t *item;



    /*
     * Editing mode has priority.
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
     * Open submenu.
     */
    if(item->type == MENU_ITEM_SUBMENU)
    {

        if(item->child_page != MENU_INVALID_PAGE)
        {

            MenuController_SaveCursor();



            menu_state.current_page =
                    item->child_page;



            MenuController_RestoreCursor();

        }


        return;

    }



    /*
     * Execute menu action.
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
 * Edit Mode:
 *
 *      Cancel current value change.
 *
 *
 * Normal Mode:
 *
 *      Return to parent page.
 *
 */
void MenuController_Back(void)
{

    const MenuPage_t *page;



    /*
     * Cancel active edit.
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
     * Root page.
     */
    if(page->parent_page == MENU_INVALID_PAGE)
    {
        return;
    }



    MenuController_SaveCursor();



    menu_state.current_page =
            page->parent_page;



    MenuController_RestoreCursor();

}



/*
 * ============================================================================
 * Getter Functions
 * ============================================================================
 *
 * These functions provide read-only access
 * to the current menu state.
 *
 * Used by:
 *
 *      - menu_renderer.c
 *
 * ============================================================================
 */


/*
 * Get current page information.
 */
const MenuPage_t *MenuController_GetCurrentPage(void)
{

    return MenuController_GetPage();

}





/*
 * Get selected item index.
 */
uint8_t MenuController_GetSelectedIndex(void)
{

    return menu_state.selected_item;

}





/*
 * Get selected item information.
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



    return &page->items[
            menu_state.selected_item];

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
 *
 * Version:
 *
 *      menu_controller.c
 *      Clean Final v1.1.1
 *
 *
 * Verified:
 *
 *      - No duplicated static functions.
 *      - Compatible with menu_controller.h.
 *      - Compatible with menu_items.h.
 *      - Compatible with menu_edit.h.
 *      - No LCD dependency.
 *      - No button dependency.
 *
 * ============================================================================
 */


/*
 * ============================================================================
 * End Of File Notes
 * ============================================================================
 *
 * Implemented Features:
 *
 *      - Page based menu navigation.
 *      - Three visible item architecture support.
 *      - Cursor position management.
 *      - Cursor memory per page.
 *      - ENTER submenu handling.
 *      - ENTER action callback execution.
 *      - BACK parent page navigation.
 *      - Edit mode confirmation priority.
 *      - Edit mode cancel priority.
 *      - Renderer independent design.
 *
 *
 * Navigation Rules:
 *
 *      UP:
 *
 *          Move cursor upward.
 *          Stop at first item.
 *
 *
 *      DOWN:
 *
    Move cursor downward.

    If the last item is reached:
        Move to next page if available.

    No wrap around.
 *
 *
 *      ENTER:
 *
 *          If editing:
 *              Confirm value.
 *
 *          Else if submenu:
 *              Open child page.
 *
 *          Else:
 *              Execute action.
 *
 *
 *      BACK:
 *
 *          If editing:
 *              Cancel edit.
 *
 *          Else:
 *              Return to parent page.
 *
 *
 * Dependencies:
 *
 *      menu_controller.h
 *              |
 *              |
 *              +---- menu_items.h
 *
 *
 *      menu_controller.c
 *              |
 *              |
 *              +---- menu_edit.h
 *
 *
 * No dependency:
 *
 *      - lcd_display
 *      - lcd_i2c
 *      - button driver
 *      - HAL
 *
 *
 * Build Compatibility:
 *
 *      STM32F103C8T6
 *
 *      STM32 HAL
 *
 *      GCC ARM Embedded
 *
 *
 * Version:
 *
 *      menu_controller.c
 *
 *      Clean Final v1.1.1
 *
 * ============================================================================
 */
