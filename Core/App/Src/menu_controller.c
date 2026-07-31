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
 *      This module implements the runtime control
 *      of the page based menu system.
 *
 *
 *      Responsibilities:
 *
 *      - Maintain current page.
 *      - Maintain selected item.
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
 *      - Parent navigation:
 *              parent_page
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
 * This variable contains:
 *
 *      - Current active page
 *      - Current selected item
 *
 */
static MenuControllerState_t menu_state;





/******************************************************************************
 *                         Private Function Prototypes
 *****************************************************************************/


/*
 * Get current page object.
 *
 * Internal helper function.
 *
 */
static const MenuPage_t *MenuController_GetPage(void);




/*
 * Validate selected item.
 *
 * Prevents invalid item access
 * after page changes.
 *
 */
static void MenuController_ValidateSelection(void);





/******************************************************************************
 *                         Private Functions
 *****************************************************************************/


/*
 * Get current page object.
 *
 * Return:
 *
 *      Pointer to MenuPage_t
 *
 *      NULL:
 *          Invalid page
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
 *
 * This function guarantees:
 *
 *      selected_item < item_count
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
 *      MAIN MENU PAGE 0
 *
 *      First item selected
 *
 */
void MenuController_Init(void)
{

    menu_state.current_page =
            MENU_PAGE_MAIN_0;



    menu_state.selected_item =
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
*      - No wrap around.
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
     * Move to previous page if available.
     */
    else
    {
        if(page->previous_page != MENU_INVALID_PAGE)
        {
            menu_state.current_page =
                    page->previous_page;


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
*      - No wrap around.
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
     * Move to next page if available.
     */
    else
    {
        if(page->next_page != MENU_INVALID_PAGE)
        {
            menu_state.current_page =
                    page->next_page;


            menu_state.selected_item = 0U;
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
    * Open child menu page.
    */
   if(item->type == MENU_ITEM_SUBMENU)
   {

       if(item->child_page != MENU_INVALID_PAGE)
       {

           menu_state.current_page =
                   item->child_page;



           /*
            * New page starts
            * from first item.
            */
           menu_state.selected_item =
                   0U;

       }

   }





   /*
    * Execute menu action.
    */
   else if(item->type == MENU_ITEM_ACTION)
   {

       if(item->action != NULL)
       {

           item->action();

       }

   }


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
* New navigation model:
*
*
*      Current Page
*
*             |
*             v
*
*      parent_page
*
*
* No history stack is used.
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





   if(page->parent_page != MENU_INVALID_PAGE)
   {

       menu_state.current_page =
               page->parent_page;



       /*
        * Parent page starts
        * with first item.
        */
       menu_state.selected_item =
               0U;

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
 *
 * Used by:
 *
 *      menu_renderer.c
 *
 *
 * Return:
 *
 *      Pointer to current MenuPage_t
 *
 */
const MenuPage_t *MenuController_GetCurrentPage(void)
{

    return MenuController_GetPage();

}






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
 *
 */
uint8_t MenuController_GetSelectedIndex(void)
{

    return menu_state.selected_item;

}







/*
 * Get selected menu item.
 *
 *
 * Return:
 *
 *      Pointer to selected item
 *
 *
 *      NULL:
 *          Invalid selection
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
 *
 * Return:
 *
 *      Current MenuPageId_t
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
 *  [OK] BACK uses parent_page.
 *
 *  [OK] Renderer API preserved.
 *
 *
 */






/******************************************************************************
 *
 * End Of File
 *
 *****************************************************************************/

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
* Main Changes:
*
*      - Removed History Stack architecture.
*
*      - Removed MENU_MAX_DEPTH dependency.
*
*      - Removed MenuHistoryEntry_t.
*
*      - Removed Push / Pop history functions.
*
*      - Simplified navigation model.
*
*      - Child pages are opened using child_page.
*
*      - BACK navigation uses parent_page.
*
*      - Preserved compatibility with:
*
*              menu_items.h
*              menu_renderer.c
*
*
* Navigation Architecture:
*
*
*
*          menu_items.c
*
*              |
*              v
*
*          menu_controller.c
*
*              |
*              v
*
*          menu_renderer.c
*
*              |
*              v
*
*          lcd_display.c
*
*              |
*              v
*
*          lcd_i2c.c
*
*
*
* Hardware:
*
*      MCU:
*          STM32F103C8T6
*
*
*      Display:
*          HD44780 Character LCD
*
*
*      Interface:
*          PCF8574 I2C Backpack
*
*
*****************************************************************************/


/*
* End of menu_controller.c
*/
