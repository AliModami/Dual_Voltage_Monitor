/******************************************************************************
 * @file    menu_engine.c
 * @brief   Menu Navigation Engine Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file implements the menu navigation engine.
 *
 *      The menu engine manages:
 *
 *          - Current menu state.
 *          - Selected item.
 *          - Scrolling position.
 *          - Enter and back navigation.
 *          - Menu action execution.
 *
 *
 *      Architecture:
 *
 *
 *              Button Input
 *
 *                    |
 *                    v
 *
 *             menu_controller
 *
 *                    |
 *                    v
 *
 *             menu_engine.c
 *
 *                    |
 *          +---------+---------+
 *          |                   |
 *          v                   v
 *
 *     menu_data.c       menu_renderer.c
 *
 *
 *
 *      This module does NOT:
 *
 *          - Access GPIO.
 *          - Access LCD.
 *          - Draw graphics.
 *          - Manage hardware.
 *
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      1. Menu data is read-only.
 *
 *      2. Navigation state is maintained internally.
 *
 *      3. Renderer receives only state information.
 *
 *      4. Actions are executed through callbacks.
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



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_engine.h"

#include "menu_actions.h"

#include <stddef.h>





/******************************************************************************
 *                         Private Constants
 ******************************************************************************/

/*
 * Number of visible menu items on LCD.
 *
 * LCD layout:
 *
 *      Row 0 : Title
 *      Row 1 : Item
 *      Row 2 : Item
 *      Row 3 : Item
 *
 */

#define MENU_ENGINE_VISIBLE_ITEMS       (3U)





/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * Current navigation state.
 *
 * This structure contains:
 *
 *      current_menu
 *      selected_index
 *      top_index
 *
 */

static MenuState_t menu_state;



/*
 * Indicates that menu content changed.
 *
 * Renderer uses this flag to decide
 * whether LCD update is required.
 *
 */

static bool menu_changed = false;





/******************************************************************************
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Update scrolling position.
 *
 * @details
 *
 *      When selected item moves outside
 *      visible LCD area, the top index
 *      is adjusted.
 *
 */
static void MenuEngine_UpdateScroll(void)
{

    /*
     * Selected item moved below
     * visible area.
     */

    if(menu_state.selected_index >=
       (menu_state.top_index +
        MENU_ENGINE_VISIBLE_ITEMS))
    {

        menu_state.top_index =
                menu_state.selected_index -
                MENU_ENGINE_VISIBLE_ITEMS +
                1U;

    }



    /*
     * Selected item moved above
     * visible area.
     */

    if(menu_state.selected_index <
       menu_state.top_index)
    {

        menu_state.top_index =
                menu_state.selected_index;

    }

}





/**
 * @brief
 *      Mark menu as changed.
 *
 * @details
 *
 *      Renderer checks this flag.
 *
 */
static void MenuEngine_SetChanged(void)
{
    menu_changed = true;
}





/**
 * @brief
 *      Reset selection when entering menu.
 *
 * @param menu
 *
 *      New active menu.
 *
 */
static void MenuEngine_LoadMenu(
        const Menu_t *menu)
{

    if(menu == NULL)
    {
        return;
    }



    menu_state.current_menu =
            menu;


    menu_state.selected_index =
            0U;


    menu_state.top_index =
            0U;


    MenuEngine_SetChanged();

}





/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu engine.
 */
void MenuEngine_Init(void)
{

    /*
     * Start from root menu.
     */

    MenuEngine_LoadMenu(
            MenuData_GetRoot());

}


/******************************************************************************
 *                         Navigation Functions
 ******************************************************************************/

/**
 * @brief
 *      Move selection to next menu item.
 *
 * @details
 *
 *      Moves cursor down inside current menu.
 *
 *      When the last item is reached,
 *      selection wraps to the first item.
 *
 */
void MenuEngine_MoveNext(void)
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



    /*
     * Move to next item.
     *
     * Wrap around after last item.
     */

    if(menu_state.selected_index <
       (item_count - 1U))
    {

        menu_state.selected_index++;

    }
    else
    {

        menu_state.selected_index = 0U;

    }



    MenuEngine_UpdateScroll();


    MenuEngine_SetChanged();

}





/**
 * @brief
 *      Move selection to previous menu item.
 *
 * @details
 *
 *      Moves cursor upward.
 *
 *      When first item is reached,
 *      selection wraps to the last item.
 *
 */
void MenuEngine_MovePrevious(void)
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



    /*
     * Move to previous item.
     */

    if(menu_state.selected_index > 0U)
    {

        menu_state.selected_index--;

    }
    else
    {

        menu_state.selected_index =
                item_count - 1U;

    }



    MenuEngine_UpdateScroll();


    MenuEngine_SetChanged();

}





/**
 * @brief
 *      Enter selected menu item.
 *
 * @details
 *
 *      Two possible cases:
 *
 *      1)
 *
 *          Item contains submenu:
 *
 *              Enter submenu.
 *
 *
 *      2)
 *
 *          Item contains action:
 *
 *              Execute callback.
 *
 */
void MenuEngine_Enter(void)
{

    const MenuItem_t *item;



    item =
        MenuEngine_GetSelectedItem();



    if(item == NULL)
    {
        return;
    }



    /*
     * Case 1:
     *
     * Selected item contains submenu.
     */

    if(item->type == MENU_ITEM_SUBMENU)
    {

        if(item->submenu != NULL)
        {

            MenuEngine_LoadMenu(
                    item->submenu);

        }

    }



    /*
     * Case 2:
     *
     * Selected item has action.
     */

    else
    {

        if(item->action != NULL)
        {

            item->action();

        }

    }

}





/**
 * @brief
 *      Return to parent menu.
 *
 * @details
 *
 *      If current menu has parent,
 *      engine returns to it.
 *
 */
void MenuEngine_Back(void)
{

    const Menu_t *parent;



    if(menu_state.current_menu == NULL)
    {
        return;
    }



    parent =
        menu_state.current_menu->parent;



    if(parent != NULL)
    {

        MenuEngine_LoadMenu(parent);

    }

}

/******************************************************************************
 *                         Command Processing
 ******************************************************************************/

/**
 * @brief
 *      Process application button command.
 *
 * @details
 *
 *      This function receives logical commands
 *      from button application layer.
 *
 *
 *      Hardware details are already removed.
 *
 *      Example:
 *
 *          Physical DOWN button
 *
 *                  |
 *                  v
 *
 *          Button Driver
 *
 *                  |
 *                  v
 *
 *          Button Application
 *
 *                  |
 *                  v
 *
 *          MenuEngine_ProcessCommand()
 *
 *
 * @param command
 *
 *      Application button command.
 *
 */
void MenuEngine_ProcessCommand(
        Button_AppCommand_t command)
{

    switch(command)
    {

        case BUTTON_APP_CMD_UP:

            MenuEngine_MovePrevious();

            break;



        case BUTTON_APP_CMD_DOWN:

            MenuEngine_MoveNext();

            break;



        case BUTTON_APP_CMD_ENTER:

            MenuEngine_Enter();

            break;



        case BUTTON_APP_CMD_BACK:

            MenuEngine_Back();

            break;



        case BUTTON_APP_CMD_NONE:

        default:

            /*
             * No action required.
             */

            break;

    }

}





/******************************************************************************
 *                         State Access Functions
 ******************************************************************************/

/**
 * @brief
 *      Get current active menu.
 *
 * @return
 *
 *      Pointer to active menu.
 *
 */
const Menu_t *MenuEngine_GetCurrentMenu(void)
{

    return menu_state.current_menu;

}





/**
 * @brief
 *      Get selected item index.
 *
 * @return
 *
 *      Current selected index.
 *
 */
uint8_t MenuEngine_GetSelectedIndex(void)
{

    return menu_state.selected_index;

}





/**
 * @brief
 *      Get currently selected menu item.
 *
 * @return
 *
 *      Pointer to selected item.
 *
 */
const MenuItem_t *MenuEngine_GetSelectedItem(void)
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
 *      Get first visible item index.
 *
 * @return
 *
 *      Top display index.
 *
 */
uint8_t MenuEngine_GetTopIndex(void)
{

    return menu_state.top_index;

}





/**
 * @brief
 *      Check menu modification status.
 *
 * @return
 *
 *      true:
 *          Renderer should refresh LCD.
 *
 *      false:
 *          No refresh needed.
 *
 */
bool MenuEngine_IsChanged(void)
{

    return menu_changed;

}





/**
 * @brief
 *      Clear menu change flag.
 *
 * @details
 *
 *      Renderer calls this function after
 *      successful LCD update.
 *
 */
void MenuEngine_ClearChangedFlag(void)
{

    menu_changed = false;

}





/******************************************************************************
 *                              End Of File
 ******************************************************************************/
