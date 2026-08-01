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
 *      Dual Voltage Monitor - Page Based Menu
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This module implements runtime control of
 *      the page based menu system.
 *
 *
 *      Responsibilities:
 *
 *      - Maintain current page.
 *      - Maintain selected item.
 *      - Preserve navigation context.
 *      - Handle UP / DOWN navigation.
 *      - Handle ENTER operation.
 *      - Handle BACK operation.
 *
 *
 *      Navigation model:
 *
 *      - Child navigation:
 *              child_page
 *
 *      - Page navigation:
 *              next_page / previous_page
 *
 *      - Context restore:
 *              stored page + cursor position
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


/******************************************************************************
 *                              Include Files
 *****************************************************************************/


#include <stddef.h>


#include "menu_controller.h"





/******************************************************************************
 *                              Private Variables
 *****************************************************************************/


/*
 * Runtime menu state.
 *
 * Contains:
 *
 *      - Current active page.
 *      - Current selected item.
 *      - Previous navigation context.
 *
 */
static MenuControllerState_t menu_state;





/******************************************************************************
 *                         Private Function Prototypes
 *****************************************************************************/


/*
 * Get current page object.
 *
 */
static const MenuPage_t *MenuController_GetPage(void);





/*
 * Validate selected item.
 *
 */
static void MenuController_ValidateSelection(void);





/*
 * Save current navigation context.
 *
 * Used before:
 *
 *      - ENTER child page
 *      - NEXT page transition
 *      - PREVIOUS page transition
 *
 */
static void MenuController_SaveContext(void);





/******************************************************************************
 *                         Private Functions
 *****************************************************************************/


/*
 * Get current page object.
 *
 */
static const MenuPage_t *MenuController_GetPage(void)
{

    return MenuItems_GetPage(
            menu_state.current_page);

}







/*
 * Validate current selection.
 *
 */
static void MenuController_ValidateSelection(void)
{

    const MenuPage_t *page;



    page = MenuController_GetPage();



    if(page == NULL)
    {
        menu_state.selected_item = 0U;
        return;
    }





    if(page->item_count == 0U)
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
 * Save current navigation context.
 *
 * This function stores:
 *
 *      Current page
 *      Current cursor position
 *
 * before leaving current page.
 *
 */
static void MenuController_SaveContext(void)
{

    menu_state.parent_page =
            menu_state.current_page;



    menu_state.parent_selected_item =
            menu_state.selected_item;

}






/******************************************************************************
 *
 * Initialization
 *
 *****************************************************************************/


/*
 * Initialize menu controller.
 *
 */
void MenuController_Init(void)
{

    menu_state.current_page =
            MENU_PAGE_MAIN_0;



    menu_state.selected_item =
            0U;



    menu_state.parent_page =
            MENU_INVALID_PAGE;



    menu_state.parent_selected_item =
            0U;


}

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
 *      - Move to previous page if available.
 *
 */
void MenuController_MoveUp(void)
{

    const MenuPage_t *page;



    page = MenuController_GetPage();



    if(page == NULL)
    {
        return;
    }





    /*
     * Move inside current page.
     */
    if(menu_state.selected_item > 0U)
    {

        menu_state.selected_item--;

    }





    /*
     * First item reached.
     *
     * Move to previous page.
     */
    else
    {

        if(page->previous_page != MENU_INVALID_PAGE)
        {

            /*
             * Save current page context.
             */
            MenuController_SaveContext();




            /*
             * Change page.
             */
            menu_state.current_page =
                    page->previous_page;




            /*
             * Previous page starts
             * from last item.
             */
            page = MenuController_GetPage();



            if(page != NULL &&
               page->item_count > 0U)
            {

                menu_state.selected_item =
                        page->item_count - 1U;

            }
            else
            {

                menu_state.selected_item = 0U;

            }

        }

    }


}








/*
 * Move cursor down.
 *
 *
 * Rules:
 *
 *      - Increase selected item.
 *      - Stop at last item.
 *      - Move to next page if available.
 *
 */
void MenuController_MoveDown(void)
{

    const MenuPage_t *page;



    page = MenuController_GetPage();



    if(page == NULL)
    {
        return;
    }





    if(page->item_count == 0U)
    {
        return;
    }





    /*
     * Move inside current page.
     */
    if(menu_state.selected_item <
       (page->item_count - 1U))
    {

        menu_state.selected_item++;

    }





    /*
     * Last item reached.
     *
     * Move to next page.
     */
    else
    {

        if(page->next_page != MENU_INVALID_PAGE)
        {

            /*
             * Save current cursor
             * before leaving page.
             */
            MenuController_SaveContext();





            /*
             * Change to next page.
             */
            menu_state.current_page =
                    page->next_page;




            /*
             * New page starts
             * from first item.
             */
            menu_state.selected_item =
                    0U;

        }

    }


}






/******************************************************************************
 *
 * ENTER Operation
 *
 *****************************************************************************/


/*
 * ENTER button handler.
 *
 *
 * MENU_ITEM_SUBMENU:
 *
 *      Save current context.
 *      Open child page.
 *
 *
 * MENU_ITEM_ACTION:
 *
 *      Execute callback.
 *
 */
void MenuController_Enter(void)
{

    const MenuPage_t *page;

    const MenuItem_t *item;





    page = MenuController_GetPage();



    if(page == NULL)
    {
        return;
    }





    MenuController_ValidateSelection();





    if(page->item_count == 0U)
    {
        return;
    }





    item = &page->items[
            menu_state.selected_item];





    /*
     * Open child page.
     */
    if(item->type == MENU_ITEM_SUBMENU)
    {

        if(item->child_page != MENU_INVALID_PAGE)
        {

            /*
             * Store parent context.
             */
            MenuController_SaveContext();





            /*
             * Enter child page.
             */
            menu_state.current_page =
                    item->child_page;




            /*
             * Child page starts
             * from first item.
             */
            menu_state.selected_item =
                    0U;

        }

    }





    /*
     * Execute action.
     */

    else if(item->type == MENU_ITEM_ACTION)
    {

        /*
         * Preserve cursor position
         * before entering action/edit mode.
         */
        menu_state.parent_selected_item =
                menu_state.selected_item;


        if(item->action != NULL)
        {

            item->action();

        }

    }



//    else if(item->type == MENU_ITEM_ACTION)
//    {
//
//        if(item->action != NULL)
//        {
//
//            item->action();
//
//        }
//
//    }


}

/******************************************************************************
 *
 * BACK Operation
 *
 *****************************************************************************/


/*
 * BACK button handler.
 *
 *
 * Behavior:
 *
 *      - Return to stored previous page.
 *      - Restore previous cursor position.
 *
 *
 * Works with:
 *
 *      - Child pages.
 *      - Page based navigation.
 *
 *
 */
void MenuController_Back(void)
{

    const MenuPage_t *page;



    page = MenuController_GetPage();



    if(page == NULL)
    {
        return;
    }





    if(menu_state.parent_page != MENU_INVALID_PAGE)
    {

        /*
         * Return to stored page.
         */
        menu_state.current_page =
                menu_state.parent_page;





        /*
         * Restore cursor.
         */
        menu_state.selected_item =
                menu_state.parent_selected_item;





        /*
         * Clear stored context.
         */
        menu_state.parent_page =
                MENU_INVALID_PAGE;



        menu_state.parent_selected_item =
                0U;


    }
    else
    {

        /*
         * Fallback:
         *
         * Use page parent relation.
         *
         * This protects against
         * invalid navigation state.
         */
        if(page->parent_page != MENU_INVALID_PAGE)
        {

            menu_state.current_page =
                    page->parent_page;



            menu_state.selected_item =
                    0U;

        }

    }


}







/******************************************************************************
 *
 * Getter Functions
 *
 *****************************************************************************/


/*
 * Get current active page.
 *
 */
const MenuPage_t *MenuController_GetCurrentPage(void)
{

    return MenuController_GetPage();

}







/*
 * Get selected item index.
 *
 */
uint8_t MenuController_GetSelectedIndex(void)
{

    return menu_state.selected_item;

}







/*
 * Get selected menu item.
 *
 */
const MenuItem_t *MenuController_GetSelectedItem(void)
{

    const MenuPage_t *page;



    page = MenuController_GetPage();





    if(page == NULL)
    {
        return NULL;
    }





    if(page->item_count == 0U)
    {
        return NULL;
    }





    if(menu_state.selected_item >=
       page->item_count)
    {
        return NULL;
    }





    return &page->items[
            menu_state.selected_item];

}





/*
 * Get current page identifier.
 *
 */
MenuPageId_t MenuController_GetCurrentPageId(void)
{

    return menu_state.current_page;

}

/******************************************************************************
 *
 * Design Verification
 *
 *****************************************************************************/


/*
 *
 * Compatibility checklist:
 *
 *
 *  [OK] No history stack.
 *
 *  [OK] No dynamic memory.
 *
 *  [OK] No LCD dependency.
 *
 *  [OK] No button dependency.
 *
 *  [OK] Uses menu_items page model.
 *
 *  [OK] ENTER uses child_page.
 *
 *  [OK] BACK restores previous cursor context.
 *
 *  [OK] next_page transition preserves context.
 *
 *  [OK] previous_page transition preserves context.
 *
 *  [OK] Renderer API preserved.
 *
 *
 */







/******************************************************************************
 *
 * Navigation Architecture
 *
 *****************************************************************************/


/*
 *
 *
 *              menu_items.c
 *
 *                    |
 *                    v
 *
 *          menu_controller.c
 *
 *                    |
 *                    v
 *
 *          menu_renderer.c
 *
 *                    |
 *                    v
 *
 *             lcd_display.c
 *
 *                    |
 *                    v
 *
 *              lcd_i2c.c
 *
 *
 *
 *
 * Navigation Context Flow:
 *
 *
 *
 *      Current Page
 *
 *          |
 *          |
 *          |  ENTER / NEXT PAGE
 *          |
 *          v
 *
 *      Store:
 *
 *          page
 *          cursor
 *
 *          |
 *          v
 *
 *      New Page
 *
 *
 *
 *      BACK
 *
 *          |
 *          v
 *
 *      Restore:
 *
 *          previous page
 *          previous cursor
 *
 *
 *
 */







/******************************************************************************
 *
 * Version Information
 *
 *****************************************************************************/


/*
 *
 * File:
 *
 *      menu_controller.c
 *
 *
 * Version:
 *
 *      Clean Final v2.3.2
 *
 *
 * Changes:
 *
 *      v2.3.2
 *
 *          - Fixed cursor restore after page navigation.
 *
 *          - Added context save before next_page transition.
 *
 *          - Added context save before previous_page transition.
 *
 *          - Improved BACK restoration behavior.
 *
 *          - Preserved child page navigation.
 *
 *          - Preserved Page Based Menu architecture.
 *
 *
 *
 * Compatible With:
 *
 *      menu_controller.h v2.3.0
 *
 *      menu_items.c
 *
 *      menu_renderer.c
 *
 *      lcd_display.c
 *
 *
 *****************************************************************************/







/******************************************************************************
 *
 *                              END OF FILE
 *
 *****************************************************************************/


/*
 * End of menu_controller.c
 */
