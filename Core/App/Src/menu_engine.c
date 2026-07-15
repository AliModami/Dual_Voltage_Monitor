/******************************************************************************
 *
 * File Name :
 *
 *      menu_engine.c
 *
 *------------------------------------------------------------------------------
 *
 * Project :
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU :
 *
 *      STM32F103C8T6
 *
 *------------------------------------------------------------------------------
 *
 * Framework :
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Description
 *
 *      Menu Navigation Engine
 *
 *------------------------------------------------------------------------------
 *
 * Overview
 *
 *      This module implements the runtime navigation engine of the
 *      menu framework.
 *
 *      Responsibilities:
 *
 *          • Navigation between menu items
 *          • Page switching
 *          • Submenu navigation
 *          • Edit mode management
 *          • Event processing
 *          • Callback execution
 *          • Renderer refresh notification
 *
 *      This module never:
 *
 *          • Draws anything on the LCD
 *          • Reads push buttons
 *          • Creates menu objects
 *          • Stores application settings
 *
 *------------------------------------------------------------------------------
 *
 * Architecture
 *
 *                  Button Driver
 *                        │
 *                        ▼
 *                  MenuEvent_t
 *                        │
 *                        ▼
 *                +---------------+
 *                | Menu Engine   |
 *                +---------------+
 *                 │      │      │
 *                 │      │      │
 *                 ▼      ▼      ▼
 *             Menu DB  Renderer  Actions
 *
 *------------------------------------------------------------------------------
 *
 * Dependencies
 *
 *      menu_engine.h
 *      menu_items.h
 *      menu_types.h
 *
 *------------------------------------------------------------------------------
 *
 * Author :
 *
 *      Ali Modami
 *
 *------------------------------------------------------------------------------
 *
 * Version :
 *
 *      2.0.0
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Includes
 *
 ******************************************************************************/

#include "menu_engine.h"
#include "menu_items.h"
#include <stddef.h>


/******************************************************************************
 *
 * Private Runtime Object
 *
 ******************************************************************************/

/*
 * Single runtime instance of the Menu Engine.
 *
 * The engine never allocates memory dynamically.
 * This object exists during the entire lifetime of
 * the application.
 */

static MenuEngine_t g_menuEngine;



/******************************************************************************
 *
 * Private Function Prototypes
 *
 ******************************************************************************/

/*
 * Internal navigation helpers.
 */

static void MenuEngine_EnterSubMenu(void);

static void MenuEngine_SelectFirstItem(MenuPage_t *page);

static void MenuEngine_SelectLastItem(MenuPage_t *page);

static void MenuEngine_UpdateCurrentItem(void);

static void MenuEngine_ExecuteCallback(MenuItem_t *item);

static void MenuEngine_RequestFullRefresh(void);

static void MenuEngine_RequestCursorRefresh(void);


/******************************************************************************
 *
 * Design Notes
 *
 ******************************************************************************

The Menu Engine operates as a simple state machine.

Each incoming MenuEvent_t is interpreted according to the current
runtime state.

Typical execution flow:

        Button Driver
              │
              ▼
        MenuEvent_t
              │
              ▼
    MenuEngine_ProcessEvent()
              │
              ▼
      State Decision Logic
              │
      ┌───────┼────────┐
      │       │        │
      ▼       ▼        ▼
 Navigation  Edit   Callback
              │
              ▼
      Refresh Request
              │
              ▼
      Menu Renderer

The engine owns only the runtime state.

The complete menu database remains static and is provided by
menu_items.c.

******************************************************************************/



/******************************************************************************
 *
 * Private Helper Functions
 *
 ******************************************************************************/

/**
 * @brief
 *      Select the first item of a page.
 *
 * @param page
 *      Target page.
 *
 * @details
 *      Updates both the page object and the runtime engine object.
 */

static void MenuEngine_SelectFirstItem(MenuPage_t *page)
{

    if (page == NULL)
    {
        return;
    }

    page->selectedItem = page->firstItem;

    g_menuEngine.currentItem = page->selectedItem;

}



/**
 * @brief
 *      Select the last item of a page.
 *
 * @param page
 *      Target page.
 */

static void MenuEngine_SelectLastItem(MenuPage_t *page)
{

    if (page == NULL)
    {
        return;
    }

    page->selectedItem = page->lastItem;

    g_menuEngine.currentItem = page->selectedItem;

}


/**
 * @brief
 *      Synchronize the runtime engine with the active page.
 *
 * @details
 *      Ensures that the runtime pointer always references the
 *      currently selected item of the active page.
 */

static void MenuEngine_UpdateCurrentItem(void)
{

    if (g_menuEngine.currentPage == NULL)
    {
        g_menuEngine.currentItem = NULL;
        return;
    }

    g_menuEngine.currentItem =
            g_menuEngine.currentPage->selectedItem;

}



/**
 * @brief
 *      Execute the callback attached to a menu item.
 *
 * @param item
 *      Target menu item.
 *
 * @details
 *      If no callback is attached, this function simply returns.
 */

static void MenuEngine_ExecuteCallback(MenuItem_t *item)
{

    if (item == NULL)
    {
        return;
    }

    if (item->enterCallback == NULL)
    {
        return;
    }

    item->enterCallback(item);

}



/******************************************************************************
 *
 * Renderer Refresh Helpers
 *
 ******************************************************************************/

/**
 * @brief
 *      Request a complete renderer refresh.
 */

static void MenuEngine_RequestFullRefresh(void)
{

    g_menuEngine.pageChanged = true;

    g_menuEngine.selectionChanged = false;

}



/**
 * @brief
 *      Request only a cursor refresh.
 */

static void MenuEngine_RequestCursorRefresh(void)
{

    g_menuEngine.selectionChanged = true;

}

/******************************************************************************
 *
 * MenuEngine_Init()
 *
 ******************************************************************************/

/**
 * @brief
 *      Initialize the Menu Engine.
 *
 * @details
 *      Initializes the runtime state and prepares the navigation
 *      engine for normal operation.
 *
 *      The menu database must already be initialized before this
 *      function is called.
 */

void MenuEngine_Init(void)
{

    /*
     * Start from the root page.
     */

    g_menuEngine.currentPage =
            Menu_GetMainPage();


    /*
     * Select the first item.
     */

    MenuEngine_SelectFirstItem(
            g_menuEngine.currentPage);


    /*
     * Initial engine state.
     */

    g_menuEngine.state =
            MENU_STATE_NAVIGATION;


    /*
     * Edit mode is disabled.
     */

    g_menuEngine.editMode =
            false;


    /*
     * Request a complete screen redraw.
     */

    g_menuEngine.pageChanged =
            true;

    g_menuEngine.selectionChanged =
            false;

}



/******************************************************************************
 *
 * Runtime Access
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the runtime engine object.
 */

MenuEngine_t *MenuEngine_GetInstance(void)
{

    return &g_menuEngine;

}



/**
 * @brief
 *      Return the active page.
 */

MenuPage_t *MenuEngine_GetCurrentPage(void)
{

    return g_menuEngine.currentPage;

}



/**
 * @brief
 *      Return the currently selected item.
 */

MenuItem_t *MenuEngine_GetSelectedItem(void)
{

    return g_menuEngine.currentItem;

}

/******************************************************************************
 *
 * Engine State Access
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the current Menu Engine state.
 */

MenuState_t MenuEngine_GetState(void)
{

    return g_menuEngine.state;

}



/**
 * @brief
 *      Change the current Menu Engine state.
 *
 * @param state
 *      New engine state.
 */

void MenuEngine_SetState(MenuState_t state)
{

    g_menuEngine.state = state;

}



/******************************************************************************
 *
 * Edit Mode
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the current edit mode state.
 */

bool MenuEngine_IsEditMode(void)
{

    return g_menuEngine.editMode;

}



/**
 * @brief
 *      Enable or disable edit mode.
 *
 * @param enable
 *      Desired edit mode state.
 */

void MenuEngine_SetEditMode(bool enable)
{

    g_menuEngine.editMode = enable;

}



/******************************************************************************
 *
 * Refresh Flag Access
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the page refresh request flag.
 */

bool MenuEngine_IsPageChanged(void)
{

    return g_menuEngine.pageChanged;

}



/**
 * @brief
 *      Clear the page refresh request.
 */

void MenuEngine_ClearPageChanged(void)
{

    g_menuEngine.pageChanged = false;

}

/**
 * @brief
 *      Return the cursor refresh request flag.
 */

bool MenuEngine_IsSelectionChanged(void)
{

    return g_menuEngine.selectionChanged;

}



/**
 * @brief
 *      Clear the cursor refresh request.
 */

void MenuEngine_ClearSelectionChanged(void)
{

    g_menuEngine.selectionChanged = false;

}



/******************************************************************************
 *
 * Refresh Request Interface
 *
 ******************************************************************************/

/**
 * @brief
 *      Request a complete page redraw.
 */

void MenuEngine_RequestPageRefresh(void)
{

    MenuEngine_RequestFullRefresh();

}



/**
 * @brief
 *      Request a cursor-only refresh.
 */

void MenuEngine_RequestSelectionRefresh(void)
{

    MenuEngine_RequestCursorRefresh();

}



/******************************************************************************
 *
 * Navigation Information
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the identifier of the current page.
 */

MenuPageId_t MenuEngine_GetCurrentPageId(void)
{

    if (g_menuEngine.currentPage == NULL)
    {
        return MENU_PAGE_MAIN;
    }

    return g_menuEngine.currentPage->id;

}



/**
 * @brief
 *      Return the identifier of the selected menu item.
 */

MenuItemId_t MenuEngine_GetCurrentItemId(void)
{

    if (g_menuEngine.currentItem == NULL)
    {
        return MENU_ITEM_ID_COUNT;
    }

    return g_menuEngine.currentItem->id;

}


/******************************************************************************
 *
 * Page Information
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the number of items in the active page.
 */

uint8_t MenuEngine_GetItemCount(void)
{

    if (g_menuEngine.currentPage == NULL)
    {
        return 0U;
    }

    return g_menuEngine.currentPage->itemCount;

}



/**
 * @brief
 *      Determine whether the active page is the root page.
 */

bool MenuEngine_IsRootPage(void)
{

    if (g_menuEngine.currentPage == NULL)
    {
        return true;
    }

    return (g_menuEngine.currentPage->parentPage == NULL);

}



/******************************************************************************
 *
 * Reset
 *
 ******************************************************************************/

/**
 * @brief
 *      Restore the Menu Engine to its initial runtime state.
 */

void MenuEngine_Reset(void)
{

    g_menuEngine.currentPage =
            Menu_GetMainPage();

    MenuEngine_SelectFirstItem(
            g_menuEngine.currentPage);

    g_menuEngine.state =
            MENU_STATE_NAVIGATION;

    g_menuEngine.editMode =
            false;

    MenuEngine_RequestFullRefresh();

}



/******************************************************************************
 *
 * Cursor Navigation
 *
 ******************************************************************************/
/******************************************************************************
 *
 * MenuEngine_MoveUp()
 *
 ******************************************************************************/

/**
 * @brief
 *      Move the cursor to the previous menu item.
 *
 * @details
 *      If the currently selected item has a valid previous pointer,
 *      the selection is moved upward. Otherwise, the selection
 *      remains unchanged.
 */

void MenuEngine_MoveUp(void)
{

    if (g_menuEngine.currentItem == NULL)
    {
        return;
    }

    if (g_menuEngine.currentItem->previous == NULL)
    {
        return;
    }

    g_menuEngine.currentItem =
            g_menuEngine.currentItem->previous;

    g_menuEngine.currentPage->selectedItem =
            g_menuEngine.currentItem;

    MenuEngine_RequestCursorRefresh();

}



/******************************************************************************
 *
 * MenuEngine_MoveDown()
 *
 ******************************************************************************/

/**
 * @brief
 *      Move the cursor to the next menu item.
 *
 * @details
 *      If the currently selected item has a valid next pointer,
 *      the selection is moved downward. Otherwise, the selection
 *      remains unchanged.
 */

void MenuEngine_MoveDown(void)
{

    if (g_menuEngine.currentItem == NULL)
    {
        return;
    }

    if (g_menuEngine.currentItem->next == NULL)
    {
        return;
    }

    g_menuEngine.currentItem =
            g_menuEngine.currentItem->next;

    g_menuEngine.currentPage->selectedItem =
            g_menuEngine.currentItem;

    MenuEngine_RequestCursorRefresh();

}


/******************************************************************************
 *
 * Submenu Navigation
 *
 ******************************************************************************/

/**
 * @brief
 *      Enter the child page of the selected menu item.
 *
 * @details
 *      If the selected item owns a valid child page, the Menu Engine
 *      activates that page and selects its first menu item.
 */

static void MenuEngine_EnterSubMenu(void)
{

    if (g_menuEngine.currentItem == NULL)
    {
        return;
    }

    if (g_menuEngine.currentItem->childPage == NULL)
    {
        return;
    }

    g_menuEngine.currentPage =
            g_menuEngine.currentItem->childPage;

    MenuEngine_SelectFirstItem(
            g_menuEngine.currentPage);

    g_menuEngine.state =
            MENU_STATE_NAVIGATION;

    MenuEngine_RequestFullRefresh();

}



/******************************************************************************
 *
 * Return To Parent Page
 *
 ******************************************************************************/

/**
 * @brief
 *      Return to the parent page.
 */

void MenuEngine_Back(void)
{

    if (g_menuEngine.currentPage == NULL)
    {
        return;
    }

    if (g_menuEngine.currentPage->parentPage == NULL)
    {
        return;
    }

    g_menuEngine.currentPage =
            g_menuEngine.currentPage->parentPage;

    MenuEngine_UpdateCurrentItem();

    MenuEngine_RequestFullRefresh();

}

/******************************************************************************
 *
 * MenuEngine_ExecuteSelectedItem()
 *
 ******************************************************************************/

/**
 * @brief
 *      Execute the currently selected menu item.
 *
 * @details
 *      The action depends on the MenuItemType.
 */

void MenuEngine_ExecuteSelectedItem(void)
{

    if (g_menuEngine.currentItem == NULL)
    {
        return;
    }

    switch (g_menuEngine.currentItem->type)
    {

        /******************************************************************
         * Sub Menu
         ******************************************************************/

        case MENU_ITEM_SUBMENU:

            MenuEngine_EnterSubMenu();

            break;



        /******************************************************************
         * Editable Value
         ******************************************************************/

        case MENU_ITEM_EDIT:

        case MENU_ITEM_VALUE:

            MenuEngine_SetEditMode(true);

            g_menuEngine.state =
                    MENU_STATE_EDIT;

            MenuEngine_RequestCursorRefresh();

            break;



        /******************************************************************
         * Execute Callback
         ******************************************************************/

        case MENU_ITEM_ACTION:

            g_menuEngine.state =
                    MENU_STATE_ACTION;

            MenuEngine_ExecuteCallback(
                    g_menuEngine.currentItem);

            break;



        /******************************************************************
         * Information Screen
         ******************************************************************/

        case MENU_ITEM_INFO:

            MenuEngine_ExecuteCallback(
                    g_menuEngine.currentItem);

            break;
            /******************************************************************
             * Normal Item
             ******************************************************************/

            case MENU_ITEM_NORMAL:

            default:

                MenuEngine_ExecuteCallback(
                        g_menuEngine.currentItem);

                break;

        }

    }



    /******************************************************************************
     *
     * MenuEngine_ProcessEvent()
     *
     ******************************************************************************/

    /**
     * @brief
     *      Process a menu event.
     *
     * @param event
     *      Incoming menu event generated by the input layer.
     *
     * @details
     *      This function is the central entry point of the Menu Engine.
     *
     *      Every button press is translated into a MenuEvent_t and routed
     *      through this state machine.
     */

    void MenuEngine_ProcessEvent(MenuEvent_t event)
    {

        switch (event)
        {

            /******************************************************************
             * No Event
             ******************************************************************/

            case MENU_EVENT_NONE:

                return;



            /******************************************************************
             * Cursor Up
             ******************************************************************/

            case MENU_EVENT_UP:

                if (g_menuEngine.editMode == false)
                {
                    MenuEngine_MoveUp();
                }

                break;



            /******************************************************************
             * Cursor Down
             ******************************************************************/

            case MENU_EVENT_DOWN:

                if (g_menuEngine.editMode == false)
                {
                    MenuEngine_MoveDown();
                }

                break;

                /******************************************************************
                         * Enter / Select
                         ******************************************************************/

                        case MENU_EVENT_ENTER:

                            MenuEngine_ExecuteSelectedItem();

                            break;



                        /******************************************************************
                         * Back Navigation
                         ******************************************************************/

                        case MENU_EVENT_BACK:

                            if (g_menuEngine.editMode == true)
                            {
                                /*
                                 * Leaving edit mode does not change the stored value.
                                 * The application layer is responsible for commit or
                                 * rollback decisions.
                                 */

                                MenuEngine_SetEditMode(false);

                                g_menuEngine.state =
                                        MENU_STATE_NAVIGATION;

                                MenuEngine_RequestCursorRefresh();
                            }
                            else
                            {
                                MenuEngine_Back();
                            }

                            break;



                        default:

                            break;

                    }

                }



                /******************************************************************************
                 *
                 * Internal Consistency Helpers
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Verify that the runtime engine contains valid pointers.
                 *
                 * @details
                 *      This function is intended for debugging and future diagnostic
                 *      extensions.
                 */

                static void MenuEngine_ValidateRuntime(void)
                {

                    if (g_menuEngine.currentPage == NULL)
                    {
                        g_menuEngine.currentItem = NULL;
                        return;
                    }


                    if (g_menuEngine.currentPage->selectedItem == NULL)
                    {
                        g_menuEngine.currentItem = NULL;
                        return;
                    }


                    g_menuEngine.currentItem =
                            g_menuEngine.currentPage->selectedItem;

                }

                /******************************************************************************
                 *
                 * Future Extension Point
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Placeholder for future periodic engine update.
                 *
                 * @details
                 *      Currently the Menu Engine is event driven.
                 *
                 *      If future requirements introduce:
                 *
                 *          • Timeout handling
                 *          • Automatic screen exit
                 *          • Message duration control
                 *          • Long press processing
                 *
                 *      this function will become the centralized location for
                 *      time-based engine processing.
                 */

                void MenuEngine_Update(void)
                {

                    /*
                     * Reserved for future implementation.
                     *
                     * Intentionally empty.
                     */

                }



                /******************************************************************************
                 *
                 * Debug Support
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Return current engine status information.
                 *
                 * @details
                 *      This function provides a simple diagnostic interface for
                 *      debugging and development tools.
                 *
                 *      The function does not modify the engine state.
                 */

                void MenuEngine_DebugRefresh(void)
                {

                    MenuEngine_ValidateRuntime();

                }

                /******************************************************************************
                 *
                 * Private State Handling
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Handle transition from edit mode to navigation mode.
                 *
                 * @details
                 *      This helper keeps state transitions centralized.
                 *
                 *      The actual value modification is intentionally handled by
                 *      the application layer because Menu Engine does not know the
                 *      meaning or format of editable values.
                 */

                static void MenuEngine_ExitEditMode(void)
                {

                    g_menuEngine.editMode = false;


                    g_menuEngine.state =
                            MENU_STATE_NAVIGATION;


                    MenuEngine_RequestCursorRefresh();

                }



                /******************************************************************************
                 *
                 * Action State Handling
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Complete an action state operation.
                 *
                 * @details
                 *      After a callback execution, the engine returns to normal
                 *      navigation unless the application changes the state.
                 */

                static void MenuEngine_CompleteAction(void)
                {

                    if (g_menuEngine.state != MENU_STATE_ACTION)
                    {
                        return;
                    }


                    g_menuEngine.state =
                            MENU_STATE_NAVIGATION;


                    MenuEngine_RequestFullRefresh();

                }



                /******************************************************************************
                 *
                 * Message State Handling
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Enter message display state.
                 *
                 * @param
                 *      None.
                 *
                 * @details
                 *      This function is reserved for future implementation of
                 *      temporary messages such as:
                 *
                 *          Saved
                 *          Error
                 *          Completed
                 */

                static void MenuEngine_ShowMessage(void)
                {

                    g_menuEngine.state =
                            MENU_STATE_MESSAGE;


                    MenuEngine_RequestFullRefresh();

                }

                /******************************************************************************
                 *
                 * Engine State Recovery
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Restore engine state after temporary operations.
                 *
                 * @details
                 *      Temporary states such as ACTION and MESSAGE must finally
                 *      return to the normal navigation state.
                 *
                 *      This helper keeps state recovery logic in one location.
                 */

                static void MenuEngine_RestoreNavigationState(void)
                {

                    g_menuEngine.state =
                            MENU_STATE_NAVIGATION;


                    g_menuEngine.editMode =
                            false;


                    MenuEngine_RequestFullRefresh();

                }



                /******************************************************************************
                 *
                 * Internal Page Switching
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Activate a specified menu page.
                 *
                 * @param page
                 *      Target menu page.
                 *
                 * @details
                 *      This function is used internally by the engine when changing
                 *      the active navigation context.
                 */

                static void MenuEngine_SetCurrentPage(MenuPage_t *page)
                {

                    if (page == NULL)
                    {
                        return;
                    }


                    g_menuEngine.currentPage =
                            page;


                    MenuEngine_SelectFirstItem(page);


                    g_menuEngine.state =
                            MENU_STATE_NAVIGATION;


                    MenuEngine_RequestFullRefresh();

                }



                /******************************************************************************
                 *
                 * Final Runtime Synchronization
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Synchronize all runtime pointers before leaving the module.
                 *
                 * @details
                 *      Ensures that:
                 *
                 *          currentPage
                 *          currentItem
                 *          selectedItem
                 *
                 *      are always aligned.
                 */

                static void MenuEngine_Synchronize(void)
                {

                    if (g_menuEngine.currentPage == NULL)
                    {
                        g_menuEngine.currentItem = NULL;
                        return;
                    }


                    g_menuEngine.currentItem =
                            g_menuEngine.currentPage->selectedItem;

                }

                /******************************************************************************
                 *
                 * Module Finalization
                 *
                 ******************************************************************************/

                /**
                 * @brief
                 *      Final internal consistency check.
                 *
                 * @details
                 *      This function is intentionally kept private.
                 *
                 *      It provides a single place for future safety checks before
                 *      extending the engine with more complex features.
                 */

                static void MenuEngine_FinalCheck(void)
                {

                    MenuEngine_Synchronize();

                }



                /******************************************************************************
                 *
                 * End Of Module
                 *
                 ******************************************************************************/

                /*
                 * Notes:
                 *
                 * The Menu Engine is now fully separated from:
                 *
                 *      - Hardware drivers
                 *      - LCD rendering
                 *      - Button scanning
                 *      - Application settings
                 *
                 * Future development should extend the engine only by:
                 *
                 *      1. Adding new MenuEvent_t values.
                 *
                 *      2. Adding new MenuState_t states.
                 *
                 *      3. Extending callbacks in the application layer.
                 *
                 *      4. Extending renderer support.
                 *
                 * The navigation core must remain hardware independent.
                 */



                /******************************************************************************
                 *
                 * End Of File
                 *
                 ******************************************************************************/
