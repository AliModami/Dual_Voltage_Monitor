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
 *      This module implements runtime control of the page based
 *      menu system.
 *
 *      Responsibilities:
 *
 *      - Maintain current page.
 *      - Maintain selected item.
 *      - Handle UP / DOWN navigation.
 *      - Handle ENTER / BACK operations.
 *      - Execute menu actions.
 *      - Manage page transitions.
 *
 *      This module has no dependency on:
 *
 *      - LCD driver.
 *      - Button driver.
 *      - Hardware layer.
 *
 *****************************************************************************/

#include <stddef.h>
#include "menu_controller.h"



/*
 * Maximum supported menu depth.
 *
 * Used to store cursor position while
 * entering child pages.
 */
#define MENU_MAX_DEPTH      8U



/*
 * Internal page history entry.
 *
 * Stores previous page and cursor position.
 */
typedef struct
{
    MenuPageId_t page;

    uint8_t selected_item;

} MenuHistoryEntry_t;



/*
 * Runtime controller state.
 */
static MenuControllerState_t menu_state;



/*
 * Parent page history stack.
 */
static MenuHistoryEntry_t menu_history[MENU_MAX_DEPTH];



/*
 * Current history depth.
 */
static uint8_t menu_depth = 0U;



/******************************************************************************
 *
 * Internal Functions
 *
 *****************************************************************************/



/*
 * Get current page object.
 */
static const MenuPage_t *MenuController_GetPage(void)
{
    return MenuItems_GetPage(menu_state.current_page);
}



/*
 * Validate current selection.
 *
 * Prevents invalid array access
 * after page changes.
 */
static void MenuController_ValidateSelection(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();



    if (page == NULL)
    {
        menu_state.selected_item = 0U;
        return;
    }



    if (page->item_count == 0U)
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
 * Push current page state into history.
 */
static void MenuController_PushHistory(void)
{
    if (menu_depth < MENU_MAX_DEPTH)
    {
        menu_history[menu_depth].page =
                menu_state.current_page;


        menu_history[menu_depth].selected_item =
                menu_state.selected_item;


        menu_depth++;
    }
}



/*
 * Restore previous page state.
 */
static void MenuController_PopHistory(void)
{
    if (menu_depth > 0U)
    {
        menu_depth--;


        menu_state.current_page =
                menu_history[menu_depth].page;


        menu_state.selected_item =
                menu_history[menu_depth].selected_item;
    }
}

/******************************************************************************
*
* Initialization and Navigation
*
*****************************************************************************/


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
*/
void MenuController_Init(void)
{
   menu_state.current_page = MENU_PAGE_MAIN_0;


   menu_state.selected_item = 0U;


   menu_depth = 0U;
}






/*
 * Move cursor up.
 *
 * Rules:
 *
 * - Move inside current page.
 * - If cursor is already at first item,
 *   move to previous page if available.
 *
 */
void MenuController_MoveUp(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if (page == NULL)
    {
        return;
    }


    if (menu_state.selected_item > 0U)
    {
        menu_state.selected_item--;
    }
    else
    {
        if (page->previous_page != MENU_INVALID_PAGE)
        {
            menu_state.current_page = page->previous_page;


            page = MenuController_GetPage();


            if (page != NULL && page->item_count > 0U)
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
 * Rules:
 *
 * - Move inside current page.
 * - If cursor is already at last item,
 *   move to next page if available.
 *
 */
void MenuController_MoveDown(void)
{
    const MenuPage_t *page;


    page = MenuController_GetPage();


    if (page == NULL)
    {
        return;
    }


    if (page->item_count == 0U)
    {
        return;
    }


    if (menu_state.selected_item < (page->item_count - 1U))
    {
        menu_state.selected_item++;
    }
    else
    {
        if (page->next_page != MENU_INVALID_PAGE)
        {
            menu_state.current_page = page->next_page;


            menu_state.selected_item = 0U;
        }
    }
}




/*
* Move to next page.
*
* Used for page based navigation.
*/
void MenuController_NextPage(void)
{
   const MenuPage_t *page;


   page = MenuController_GetPage();



   if (page == NULL)
   {
       return;
   }



   if (page->next_page != MENU_INVALID_PAGE)
   {
       menu_state.current_page =
               page->next_page;


       menu_state.selected_item = 0U;
   }
}



/*
* Move to previous page.
*/
void MenuController_PreviousPage(void)
{
   const MenuPage_t *page;


   page = MenuController_GetPage();



   if (page == NULL)
   {
       return;
   }



   if (page->previous_page != MENU_INVALID_PAGE)
   {
       menu_state.current_page =
               page->previous_page;


       menu_state.selected_item = 0U;
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
* MENU_ITEM_SUBMENU:
*
*      Open child page.
*
*
* MENU_ITEM_ACTION:
*
*      Execute callback.
*/
void MenuController_Enter(void)
{
   const MenuPage_t *page;

   const MenuItem_t *item;



   page = MenuController_GetPage();



   if (page == NULL)
   {
       return;
   }



   MenuController_ValidateSelection();



   if (page->item_count == 0U)
   {
       return;
   }



   item = &page->items[menu_state.selected_item];



   if (item->type == MENU_ITEM_SUBMENU)
   {
       if (item->child_page != MENU_INVALID_PAGE)
       {
           MenuController_PushHistory();


           menu_state.current_page =
                   item->child_page;


           menu_state.selected_item = 0U;
       }
   }


   else if (item->type == MENU_ITEM_ACTION)
   {
       if (item->action != NULL)
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
 * Behavior:
 *
 * - Return to previous page.
 * - Restore previous cursor position.
 */
void MenuController_Back(void)
{
    /*
     * If history exists,
     * restore previous menu state.
     */
    if (menu_depth > 0U)
    {
        MenuController_PopHistory();
    }
    else
    {
        /*
         * Already at root menu.
         * No operation required.
         */
        menu_state.current_page =
                MENU_PAGE_MAIN_0;


        menu_state.selected_item = 0U;
    }
}



/******************************************************************************
 *
 * Getter Functions
 *
 *****************************************************************************/


/*
 * Get current page.
 *
 * Used by renderer.
 */
const MenuPage_t *MenuController_GetCurrentPage(void)
{
    return MenuController_GetPage();
}



/*
 * Get selected item index.
 *
 * Used by renderer for cursor drawing.
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



    if (page == NULL)
    {
        return NULL;
    }



    if (page->item_count == 0U)
    {
        return NULL;
    }



    if (menu_state.selected_item >= page->item_count)
    {
        return NULL;
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
 * Implementation summary:
 *
 *      - Page based navigation implemented.
 *      - Cursor movement without wrap-around.
 *      - Child page entry supported.
 *      - Parent page restore supported.
 *      - Cursor position history supported.
 *      - Action callbacks supported.
 *      - No LCD dependency.
 *      - No button dependency.
 *
 *------------------------------------------------------------------------------
 *
 * Notes:
 *
 *      This module requires:
 *
 *      menu_controller.h
 *      menu_items.h
 *
 *      The menu tree must be provided by:
 *
 *      menu_items.c
 *
 *****************************************************************************/


/*
 * End of menu_controller.c
 */
