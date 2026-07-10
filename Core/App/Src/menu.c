/******************************************************************************
 * @file    menu.c
 * @brief   Menu Controller Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module implements the menu controller layer.
 *
 *
 *      The responsibility of this module is:
 *
 *          - Keep runtime menu state.
 *          - Receive button commands.
 *          - Navigate inside menu tree.
 *          - Execute selected menu actions.
 *          - Provide information to renderer.
 *
 *
 *      This module DOES NOT:
 *
 *          - Define menu structure.
 *          - Store menu items.
 *          - Access LCD hardware.
 *          - Access GPIO.
 *          - Access ADC.
 *          - Access UART.
 *
 *
 *-----------------------------------------------------------------------------
 *
 * Architecture:
 *
 *
 *              button_app
 *                  |
 *                  v
 *              menu.c
 *                  |
 *        +---------+----------+
 *        |                    |
 *        v                    v
 *   menu_data.c        menu_renderer.c
 *
 *
 *
 *      menu_data.c
 *
 *          Owns:
 *
 *              - Menu tree
 *              - Menu items
 *              - Text
 *              - Actions
 *
 *
 *      menu.c
 *
 *          Owns:
 *
 *              - Current position
 *              - Selection
 *              - Navigation state
 *
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      2.0.0
 *
 *
 * Change History :
 *
 *      2.0.0
 *
 *          - Migrated from old linked-list menu.
 *
 *          - Removed menu tree definition.
 *
 *          - Added compatibility with Plan A architecture.
 *
 ******************************************************************************/




/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu.h"

#include "menu_data.h"
#include "menu_actions.h"

#include <stddef.h>




/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/**
 * @brief
 *      Current application mode.
 *
 *      This value represents the active
 *      application screen.
 */
static Menu_Mode_t current_mode = MENU_MODE;



/**
 * @brief
 *      Runtime navigation state.
 *
 *      This structure contains:
 *
 *          - Current menu
 *          - Selected item
 *          - Scroll position
 */
static MenuState_t menu_state;



/**
 * @brief
 *      Refresh request flag.
 *
 *      Renderer checks this flag
 *      before redrawing LCD.
 */
static bool refresh_required = false;




/******************************************************************************
 *                         Private Function Prototypes
 ******************************************************************************/

/**
 * @brief
 *      Move selection upward.
 */
static void Menu_MoveUp(void);



/**
 * @brief
 *      Move selection downward.
 */
static void Menu_MoveDown(void);



/**
 * @brief
 *      Execute selected menu item.
 */
static void Menu_ExecuteSelected(void);



/**
 * @brief
 *      Return to previous menu.
 */
static void Menu_ReturnBack(void);




/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu controller.
 *
 * @details
 *
 *      This function does not create menus.
 *
 *      Menu structure is already created
 *      inside menu_data.c.
 *
 */
void Menu_Init(void)
{

    const Menu_t *root_menu;



    /*
     * Get root menu from data layer.
     */
    root_menu = MenuData_GetRoot();



    if(root_menu == NULL)
    {
        return;
    }



    /*
     * Initialize runtime state.
     */
    menu_state.current_menu = root_menu;


    /*
     * First item is selected.
     */
    menu_state.selected_index = 0U;


    /*
     * First visible item starts
     * from beginning.
     */
    menu_state.top_index = 0U;



    /*
     * Default application mode.
     */
    current_mode = MENU_MODE;



    /*
     * Force first LCD rendering.
     */
    refresh_required = true;

}




/**
 * @brief
 *      Periodic menu task.
 *
 * @details
 *
 *      Reserved for future features:
 *
 *          - Long press handling.
 *          - Automatic scrolling.
 *          - Menu timeout.
 *
 */
void Menu_Task(void)
{

    /*
     * Currently no periodic processing
     * is required.
     */

}





/**
 * @brief
 *      Process button command.
 *
 * @param command
 *      Button application command.
 */
void Menu_ProcessCommand(
        Button_AppCommand_t command)
{

    switch(command)
    {

        case BUTTON_CMD_UP:

            Menu_MoveUp();

            break;



        case BUTTON_CMD_DOWN:

            Menu_MoveDown();

            break;



        case BUTTON_CMD_ENTER:

            Menu_ExecuteSelected();

            break;



        case BUTTON_CMD_BACK:

            Menu_ReturnBack();

            break;



        default:

            break;

    }

}

/******************************************************************************
 *                         Private Navigation Functions
 ******************************************************************************/


/**
 * @brief
 *      Move selection upward.
 *
 * @details
 *
 *      The selected index is decreased
 *      until the first item is reached.
 *
 *      Circular navigation is intentionally
 *      disabled.
 *
 */
static void Menu_MoveUp(void)
{

    if(menu_state.current_menu == NULL)
    {
        return;
    }



    if(menu_state.selected_index > 0U)
    {
        menu_state.selected_index--;

        /*
         * Update visible window.
         */
        if(menu_state.selected_index <
           menu_state.top_index)
        {
            menu_state.top_index =
                    menu_state.selected_index;
        }


        refresh_required = true;
    }

}





/**
 * @brief
 *      Move selection downward.
 *
 * @details
 *
 *      Navigation stops at the last
 *      available item.
 *
 */
static void Menu_MoveDown(void)
{

    uint8_t item_count;



    if(menu_state.current_menu == NULL)
    {
        return;
    }



    item_count =
        MenuData_GetItemCount(
                menu_state.current_menu);



    if(item_count == 0U)
    {
        return;
    }



    if(menu_state.selected_index <
       (item_count - 1U))
    {

        menu_state.selected_index++;



        /*
         * Check scrolling window.
         *
         * LCD shows only limited rows.
         */
        if(menu_state.selected_index >=
           (menu_state.top_index +
            MENU_VISIBLE_ITEMS))
        {

            menu_state.top_index++;

        }



        refresh_required = true;

    }

}







/**
 * @brief
 *      Execute current selected item.
 *
 * @details
 *
 *      Two possibilities exist:
 *
 *
 *      1)
 *          Item has submenu.
 *
 *          Enter submenu.
 *
 *
 *      2)
 *          Item has action.
 *
 *          Execute callback.
 *
 */
static void Menu_ExecuteSelected(void)
{

    const MenuItem_t *item;



    if(menu_state.current_menu == NULL)
    {
        return;
    }



    item =
        MenuData_GetItem(
            menu_state.current_menu,
            menu_state.selected_index);



    if(item == NULL)
    {
        return;
    }



    /*
     * Case 1:
     *
     * Item opens submenu.
     */
    if(item->type == MENU_ITEM_SUBMENU)
    {

        if(item->submenu != NULL)
        {

            menu_state.current_menu =
                    item->submenu;


            menu_state.selected_index = 0U;


            menu_state.top_index = 0U;


            refresh_required = true;

        }


        return;
    }




    /*
     * Case 2:
     *
     * Execute application action.
     */
    if(item->action != NULL)
    {

        item->action();


        refresh_required = true;

    }

}







/**
 * @brief
 *      Return to parent menu.
 *
 * @details
 *
 *      If current menu has parent,
 *      navigation returns one level upward.
 *
 */
static void Menu_ReturnBack(void)
{

    if(menu_state.current_menu == NULL)
    {
        return;
    }



    if(menu_state.current_menu->parent != NULL)
    {

        menu_state.current_menu =
                menu_state.current_menu->parent;



        menu_state.selected_index = 0U;



        menu_state.top_index = 0U;



        current_mode = MENU_MODE;



        refresh_required = true;

    }

}







/******************************************************************************
 *                         Public State Access Functions
 ******************************************************************************/


/**
 * @brief
 *      Get current application mode.
 *
 * @return
 *      Current mode.
 */
Menu_Mode_t Menu_GetMode(void)
{
    return current_mode;
}







/**
 * @brief
 *      Get current menu state.
 *
 * @details
 *
 *      Renderer uses this information
 *      to draw menu.
 *
 *      Returned pointer must not be modified.
 *
 */
const MenuState_t *Menu_GetState(void)
{
    return &menu_state;
}







/**
 * @brief
 *      Request renderer update.
 *
 */
void Menu_RequestRefresh(void)
{

    refresh_required = true;

}







/**
 * @brief
 *      Check refresh status.
 *
 * @return
 *
 *      true:
 *          Renderer should redraw.
 *
 */
bool Menu_IsRefreshRequired(void)
{

    return refresh_required;

}







/**
 * @brief
 *      Clear refresh flag.
 *
 */
void Menu_ClearRefreshRequest(void)
{

    refresh_required = false;

}

/******************************************************************************
*                         Renderer Interface
******************************************************************************/

/**
* @brief
*      Get currently selected menu item.
*
* @details
*
*      Renderer uses this function
*      to identify highlighted item.
*
*      Returned pointer is read-only.
*
* @return
*      Selected menu item.
*
*/
const MenuItem_t *Menu_GetSelectedItem(void)
{

   if(menu_state.current_menu == NULL)
   {
       return NULL;
   }



   return MenuData_GetItem(
           menu_state.current_menu,
           menu_state.selected_index);

}






/**
* @brief
*      Get current menu object.
*
* @details
*
*      Renderer uses this function
*      for drawing visible items.
*
* @return
*      Current menu pointer.
*
*/
const Menu_t *Menu_GetCurrentMenu(void)
{

   return menu_state.current_menu;

}






/**
* @brief
*      Get current menu title.
*
* @details
*
*      LCD first row is reserved
*      for menu title.
*
* @return
*      Title string.
*
*/
const char *Menu_GetCurrentTitle(void)
{

   if(menu_state.current_menu == NULL)
   {
       return "";
   }



   return menu_state.current_menu->title;

}






/**
* @brief
*      Get visible start index.
*
* @details
*
*      Renderer uses this value
*      for scrolling.
*
* @return
*      First visible item index.
*
*/
uint8_t Menu_GetTopIndex(void)
{

   return menu_state.top_index;

}






/**
* @brief
*      Get selected item index.
*
* @return
*      Current selection index.
*
*/
uint8_t Menu_GetSelectedIndex(void)
{

   return menu_state.selected_index;

}






/**
* @brief
*      Calculate LCD row for item.
*
* @details
*
*      LCD layout:
*
*
*          Row 0
*          +----------------+
*          | Menu Title     |
*          +----------------+
*
*          Row 1
*          | Item 1         |
*
*          Row 2
*          | > Item 2       |
*
*          Row 3
*          | Item 3         |
*
*
*      Selected item is always
*      displayed on the middle row
*      when possible.
*
* @param item_index
*      Menu item index.
*
* @return
*      LCD row number.
*
*/
uint8_t Menu_GetItemDisplayRow(
       uint8_t item_index)
{

   uint8_t relative_position;



   if(item_index < menu_state.top_index)
   {
       return 0U;
   }



   relative_position =
           item_index -
           menu_state.top_index;



   /*
    * Title occupies row zero.
    */
   return MENU_FIRST_ITEM_ROW +
          relative_position;

}






/**
* @brief
*      Get number of visible menu items.
*
* @details
*
*      Prevents renderer from
*      drawing outside LCD area.
*
* @return
*      Visible item count.
*
*/
uint8_t Menu_GetVisibleCount(void)
{

   uint8_t remaining_items;



   if(menu_state.current_menu == NULL)
   {
       return 0U;
   }



   if(menu_state.top_index >=
      menu_state.current_menu->item_count)
   {
       return 0U;
   }



   remaining_items =
       menu_state.current_menu->item_count -
       menu_state.top_index;



   if(remaining_items > MENU_VISIBLE_ITEMS)
   {
       return MENU_VISIBLE_ITEMS;
   }


   return remaining_items;

}






/******************************************************************************
*                              End Of File
******************************************************************************/

/*
*
* Version 2.0.0
*
* Plan A migration completed.
*
*
* Changes:
*
*      - Removed old linked-list menu ownership.
*
*      - Moved menu tree to menu_data.c.
*
*      - Added separated controller layer.
*
*      - Added renderer interface.
*
*      - Added scroll support.
*
*      - Added read-only state access.
*
*
*
* Architecture result:
*
*
*
*              button_app
*                   |
*                   v
*                menu.c
*                   |
*          +--------+--------+
*          |                 |
*          v                 v
*
*      menu_data.c     menu_renderer.c
*
*
*
******************************************************************************/
