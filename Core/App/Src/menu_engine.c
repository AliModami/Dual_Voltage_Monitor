/******************************************************************************
 *
 * @file    menu_engine.c
 *
 * @brief   Runtime Menu Navigation Engine
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU:
 *
 *      STM32F103C8T6
 *
 *------------------------------------------------------------------------------
 *
 * Framework:
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This module implements the runtime menu navigation engine.
 *
 *      Responsibilities:
 *
 *          - Menu initialization
 *          - Page navigation
 *          - Item selection
 *          - Event processing
 *          - Edit mode management
 *          - Renderer synchronization
 *
 *------------------------------------------------------------------------------
 *
 * Design Principles:
 *
 *      1. Runtime state is separated from menu database.
 *
 *      2. No hardware dependency exists here.
 *
 *      3. Renderer communicates only through refresh flags.
 *
 *      4. Application functionality is accessed through callbacks.
 *
 *------------------------------------------------------------------------------
 *
 * Dependencies:
 *
 *      menu_engine.h
 *      menu_types.h
 *      menu_items.h
 *
 *------------------------------------------------------------------------------
 *
 * Author:
 *
 *      Ali Modami
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      2.1.0
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <menu_items_old.h>
#include "menu_engine.h"




/******************************************************************************
 * Private Runtime Object
 ******************************************************************************/

/*
 * Single static runtime instance.
 *
 * Dynamic memory allocation is intentionally avoided.
 */
static MenuEngine_t g_menuEngine;



/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void MenuEngine_OpenPage(
        MenuPage_t *page);


//static void MenuEngine_SelectItem(
//        MenuItem_t *item);


static void MenuEngine_ExecuteAction(
        MenuItem_t *item);


static void MenuEngine_EnterEditMode(void);


static void MenuEngine_LeaveEditMode(void);


static void MenuEngine_InternalRefresh(void);



/******************************************************************************
 * Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initializes menu engine.
 *
 * @details
 *
 *      Initialization order:
 *
 *          1. Create static menu database.
 *          2. Reset runtime engine.
 *
 ******************************************************************************/

void MenuEngine_Init(void)
{

    MenuItems_Init();


    MenuEngine_Reset();

}



/******************************************************************************
 * Reset Runtime State
 ******************************************************************************/

/**
 * @brief
 *      Resets menu engine runtime state.
 *
 ******************************************************************************/

void MenuEngine_Reset(void)
{

    /*
     * Select root menu page.
     */

    g_menuEngine.currentPage =
            MenuItems_GetRootPage();



    if(g_menuEngine.currentPage != NULL)
    {

        g_menuEngine.currentItem =
                g_menuEngine.currentPage->firstItem;

    }
    else
    {

        g_menuEngine.currentItem =
                NULL;

    }



    /*
     * Default runtime state.
     */

    g_menuEngine.state =
            MENU_STATE_IDLE;



    g_menuEngine.editMode =
            false;



    /*
     * Force renderer update.
     */

    g_menuEngine.pageChanged =
            true;


    g_menuEngine.selectionChanged =
            true;



    g_menuEngine.lastEvent =
            MENU_EVENT_NONE;

}



/******************************************************************************
 * Runtime Access
 ******************************************************************************/

/**
 * @brief
 *      Returns internal engine instance.
 *
 ******************************************************************************/

MenuEngine_t *MenuEngine_GetInstance(void)
{

    return &g_menuEngine;

}



/**
 * @brief
 *      Returns current menu page.
 *
 ******************************************************************************/

MenuPage_t *MenuEngine_GetCurrentPage(void)
{

    return g_menuEngine.currentPage;

}



/**
 * @brief
 *      Returns selected menu item.
 *
 ******************************************************************************/

MenuItem_t *MenuEngine_GetSelectedItem(void)
{

    return g_menuEngine.currentItem;

}



/******************************************************************************
 * Navigation Up
 ******************************************************************************/

/**
 * @brief
 *      Moves cursor to previous item.
 *
 ******************************************************************************/

void MenuEngine_MoveUp(void)
{

    /*
     * Navigation is disabled during editing.
     */

    if(g_menuEngine.editMode)
    {
        return;
    }



    if(g_menuEngine.currentItem == NULL)
    {
        return;
    }



    if(g_menuEngine.currentItem->previous == NULL)
    {
        return;
    }



    g_menuEngine.currentItem =
            g_menuEngine.currentItem->previous;



    if(g_menuEngine.currentPage != NULL)
    {

        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentItem;


        if(g_menuEngine.currentPage->selectedIndex > 0U)
        {

            g_menuEngine.currentPage->selectedIndex--;

        }

    }



    MenuEngine_RequestCursorRefresh();

}



/******************************************************************************
 * Navigation Down
 ******************************************************************************/

/**
 * @brief
 *      Moves cursor to next item.
 *
 ******************************************************************************/

void MenuEngine_MoveDown(void)
{

    /*
     * Navigation is disabled during editing.
     */

    if(g_menuEngine.editMode)
    {
        return;
    }



    if(g_menuEngine.currentItem == NULL)
    {
        return;
    }



    if(g_menuEngine.currentItem->next == NULL)
    {
        return;
    }



    g_menuEngine.currentItem =
            g_menuEngine.currentItem->next;



    if(g_menuEngine.currentPage != NULL)
    {

        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentItem;


        g_menuEngine.currentPage->selectedIndex++;

    }



    MenuEngine_RequestCursorRefresh();

}



/******************************************************************************
 *
 * End Of Part 1/6
 *
 ******************************************************************************/
/******************************************************************************
 * Execute Selected Item
 ******************************************************************************/

/**
 * @brief
 *      Executes currently selected menu item.
 *
 * @details
 *
 *      The execution flow depends on item type:
 *
 *          SUBMENU:
 *              Opens child page.
 *
 *          EDIT:
 *              Enters edit mode.
 *
 *          ACTION:
 *              Executes callback.
 *
 ******************************************************************************/

void MenuEngine_ExecuteSelectedItem(void)
{

    MenuItem_t *item =
            g_menuEngine.currentItem;



    if(item == NULL)
    {
        return;
    }



    /*
     * Notify selection callback.
     */

    if(item->onSelect != NULL)
    {

        item->onSelect(item);

    }



    switch(item->type)
    {

        /**************************************************************
         * Open Submenu Page
         **************************************************************/

        case MENU_ITEM_SUBMENU:

            if(item->childPage != NULL)
            {

                MenuEngine_OpenPage(
                        item->childPage);

            }

            break;



        /**************************************************************
         * Editable Item
         *
         * New architecture:
         *
         * All editable items use MENU_ITEM_EDIT.
         *
         * The real data type is stored in:
         *
         *      item->dataType
         *
         **************************************************************/

        case MENU_ITEM_EDIT:

            MenuEngine_EnterEditMode();

            break;



        /**************************************************************
         * Action Item
         **************************************************************/

        case MENU_ITEM_ACTION:

            MenuEngine_ExecuteAction(item);

            break;



        /**************************************************************
         * Information Items
         **************************************************************/

        case MENU_ITEM_INFO:

        default:

            break;

    }

}




/******************************************************************************
 * Return To Previous Page
 ******************************************************************************/

/**
 * @brief
 *      Handles BACK button operation.
 *
 ******************************************************************************/

void MenuEngine_Back(void)
{

    /*
     * If editing,
     * BACK exits edit mode first.
     */

    if(g_menuEngine.editMode)
    {

        MenuEngine_LeaveEditMode();

        return;

    }



    if(g_menuEngine.currentPage == NULL)
    {
        return;
    }



    /*
     * Root page cannot go backward.
     */

    if(g_menuEngine.currentPage->parentPage == NULL)
    {
        return;
    }



    MenuEngine_OpenPage(
            g_menuEngine.currentPage->parentPage);

}





/******************************************************************************
 * Event Dispatcher
 ******************************************************************************/

/**
 * @brief
 *      Processes external menu events.
 *
 * @param event
 *      Menu input event.
 *
 ******************************************************************************/

void MenuEngine_ProcessEvent(
        MenuEvent_t event)
{

    g_menuEngine.lastEvent =
            event;



    switch(event)
    {

        case MENU_EVENT_UP:

            MenuEngine_MoveUp();

            break;



        case MENU_EVENT_DOWN:

            MenuEngine_MoveDown();

            break;



        case MENU_EVENT_ENTER:

            MenuEngine_ExecuteSelectedItem();

            break;



        case MENU_EVENT_BACK:

            MenuEngine_Back();

            break;



        default:

            break;

    }

}





/******************************************************************************
 * Page Navigation Helper
 ******************************************************************************/

/**
 * @brief
 *      Opens a menu page.
 *
 * @param page
 *      Destination page.
 *
 ******************************************************************************/

static void MenuEngine_OpenPage(
        MenuPage_t *page)
{

    if(page == NULL)
    {
        return;
    }



    /*
     * Notify current page before exit.
     */

    if(g_menuEngine.currentPage != NULL)
    {

        if(g_menuEngine.currentPage->onExit != NULL)
        {

            g_menuEngine.currentPage->onExit(
                    g_menuEngine.currentPage);

        }

    }



    /*
     * Change active page.
     */

    g_menuEngine.currentPage =
            page;



    /*
     * Restore previous selection.
     */

    g_menuEngine.currentItem =
            page->selectedItem;



    /*
     * First visit to page.
     */

    if(g_menuEngine.currentItem == NULL)
    {

        g_menuEngine.currentItem =
                page->firstItem;


        page->selectedItem =
                g_menuEngine.currentItem;


        page->selectedIndex =
                0U;

    }



    /*
     * Notify new page.
     */

    if(page->onEnter != NULL)
    {

        page->onEnter(page);

    }



    MenuEngine_RequestFullRefresh();

}





/******************************************************************************
 *
 * End Of Part 2/6
 *
 ******************************************************************************/
/******************************************************************************
 * Select Item Helper
 ******************************************************************************/

/**
 * @brief
 *      Selects a menu item manually.
 *
 * @param item
 *      Target menu item.
 *
 * @details
 *
 *      This function is mainly used by:
 *
 *          - Programmatic navigation
 *          - Service functions
 *          - Menu synchronization
 *
 ******************************************************************************/

//static void MenuEngine_SelectItem(
//        MenuItem_t *item)
//{
//
//    if(item == NULL)
//    {
//        return;
//    }
//
//
//
//    g_menuEngine.currentItem =
//            item;
//
//
//
//    if(g_menuEngine.currentPage != NULL)
//    {
//
//        g_menuEngine.currentPage->selectedItem =
//                item;
//
//    }
//
//
//
//    MenuEngine_RequestCursorRefresh();
//
//}





/******************************************************************************
 * Execute Action Item
 ******************************************************************************/

/**
 * @brief
 *      Executes action callback of menu item.
 *
 ******************************************************************************/

static void MenuEngine_ExecuteAction(
        MenuItem_t *item)
{

    if(item == NULL)
    {
        return;
    }



    /*
     * New callback interface.
     */

    if(item->onEnter != NULL)
    {

        item->onEnter(item);

    }



    /*
     * Compatibility callback.
     *
     * Existing old menu items may still
     * use this callback.
     *
     * The new menu framework passes
     * the item pointer.
     */

    if(item->enterCallback != NULL)
    {

        item->enterCallback(item);

    }



    /*
     * Optional exit callback.
     */

    if(item->onExit != NULL)
    {

        item->onExit(item);

    }

}





/******************************************************************************
 * Edit Mode Management
 ******************************************************************************/

/**
 * @brief
 *      Enters menu edit mode.
 *
 * @details
 *
 *      Editing is separated from navigation.
 *      While editing, UP/DOWN navigation is disabled.
 *
 ******************************************************************************/

static void MenuEngine_EnterEditMode(void)
{

    if(g_menuEngine.editMode == true)
    {
        return;
    }



    g_menuEngine.editMode =
            true;



    g_menuEngine.state =
            MENU_STATE_EDIT;



    g_menuEngine.statistics.editCount++;



    MenuEngine_RequestFullRefresh();

}





/**
 * @brief
 *      Leaves menu edit mode.
 *
 ******************************************************************************/

static void MenuEngine_LeaveEditMode(void)
{

    if(g_menuEngine.editMode == false)
    {
        return;
    }



    g_menuEngine.editMode =
            false;



    g_menuEngine.state =
            MENU_STATE_IDLE;



    MenuEngine_RequestFullRefresh();

}





/**
 * @brief
 *      Returns edit mode status.
 *
 ******************************************************************************/

bool MenuEngine_IsEditMode(void)
{

    return g_menuEngine.editMode;

}





/**
 * @brief
 *      Enables or disables edit mode.
 *
 ******************************************************************************/

void MenuEngine_SetEditMode(
        bool enable)
{

    if(enable)
    {

        MenuEngine_EnterEditMode();

    }
    else
    {

        MenuEngine_LeaveEditMode();

    }

}





/******************************************************************************
 * Engine State Access
 ******************************************************************************/

/**
 * @brief
 *      Gets current engine state.
 *
 ******************************************************************************/

MenuState_t MenuEngine_GetState(void)
{

    return g_menuEngine.state;

}





/**
 * @brief
 *      Sets engine state.
 *
 ******************************************************************************/

void MenuEngine_SetState(
        MenuState_t state)
{

    g_menuEngine.state =
            state;

}




/******************************************************************************
 *
 * End Of Part 3/6
 *
 ******************************************************************************/
/******************************************************************************
 * Refresh Management
 ******************************************************************************/

/**
 * @brief
 *      Requests page redraw.
 *
 * @details
 *
 *      Renderer checks this flag and redraws
 *      the complete page when required.
 *
 ******************************************************************************/

void MenuEngine_RequestPageRefresh(void)
{

    g_menuEngine.pageChanged =
            true;

}




/**
 * @brief
 *      Requests selection/cursor redraw.
 *
 * @details
 *
 *      Used when only cursor position changes.
 *
 ******************************************************************************/

void MenuEngine_RequestSelectionRefresh(void)
{

    g_menuEngine.selectionChanged =
            true;

}





/**
 * @brief
 *      Requests complete menu refresh.
 *
 ******************************************************************************/

void MenuEngine_RequestFullRefresh(void)
{

    MenuEngine_RequestPageRefresh();


    MenuEngine_RequestSelectionRefresh();

}





/**
 * @brief
 *      Requests cursor-only refresh.
 *
 ******************************************************************************/

void MenuEngine_RequestCursorRefresh(void)
{

    MenuEngine_RequestSelectionRefresh();

}





/**
 * @brief
 *      Checks page refresh flag.
 *
 ******************************************************************************/

bool MenuEngine_IsPageChanged(void)
{

    return g_menuEngine.pageChanged;

}





/**
 * @brief
 *      Clears page refresh flag.
 *
 ******************************************************************************/

void MenuEngine_ClearPageChanged(void)
{

    g_menuEngine.pageChanged =
            false;

}





/**
 * @brief
 *      Checks selection refresh flag.
 *
 ******************************************************************************/

bool MenuEngine_IsSelectionChanged(void)
{

    return g_menuEngine.selectionChanged;

}





/**
 * @brief
 *      Clears selection refresh flag.
 *
 ******************************************************************************/

void MenuEngine_ClearSelectionChanged(void)
{

    g_menuEngine.selectionChanged =
            false;

}





/******************************************************************************
 * Information Functions
 ******************************************************************************/

/**
 * @brief
 *      Returns current page item count.
 *
 ******************************************************************************/

uint8_t MenuEngine_GetItemCount(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return 0U;
    }



    return (uint8_t)
            g_menuEngine.currentPage->itemCount;

}





/**
 * @brief
 *      Checks if current page is root page.
 *
 ******************************************************************************/

bool MenuEngine_IsRootPage(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return false;
    }



    return
        (g_menuEngine.currentPage->parentPage == NULL);

}





/**
 * @brief
 *      Returns current page ID.
 *
 ******************************************************************************/

uint16_t MenuEngine_GetCurrentPageId(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return 0U;
    }



    return g_menuEngine.currentPage->id;

}





/**
 * @brief
 *      Returns current item ID.
 *
 ******************************************************************************/

uint16_t MenuEngine_GetCurrentItemId(void)
{

    if(g_menuEngine.currentItem == NULL)
    {
        return 0U;
    }



    return g_menuEngine.currentItem->id;

}





/******************************************************************************
 *
 * End Of Part 4/6
 *
 ******************************************************************************/
/******************************************************************************
 * Synchronization And Runtime Service
 ******************************************************************************/

/**
 * @brief
 *      Synchronizes engine state with menu database.
 *
 * @details
 *
 *      This function repairs runtime references
 *      if a page selection becomes invalid.
 *
 ******************************************************************************/

void MenuEngine_DebugRefresh(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return;
    }



    /*
     * Restore default selection
     * when current page lost selection.
     */

    if(g_menuEngine.currentPage->selectedItem == NULL)
    {

        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentPage->firstItem;


        g_menuEngine.currentPage->selectedIndex =
                0U;

    }



    g_menuEngine.currentItem =
            g_menuEngine.currentPage->selectedItem;



    MenuEngine_RequestFullRefresh();

}





/**
 * @brief
 *      Runtime update service.
 *
 * @details
 *
 *      Reserved for future extensions:
 *
 *          - Long key handling
 *          - Timeout processing
 *          - Automatic return
 *          - Message timeout
 *
 ******************************************************************************/

void MenuEngine_Update(void)
{

    /*
     * Currently no periodic task exists.
     */

}





/**
 * @brief
 *      Checks current page validity.
 *
 ******************************************************************************/

bool MenuEngine_HasCurrentPage(void)
{

    return
        (g_menuEngine.currentPage != NULL);

}





/**
 * @brief
 *      Checks current item validity.
 *
 ******************************************************************************/

bool MenuEngine_HasSelectedItem(void)
{

    return
        (g_menuEngine.currentItem != NULL);

}





/**
 * @brief
 *      Checks navigation availability.
 *
 * @return
 *
 *      true:
 *          Navigation allowed.
 *
 *      false:
 *          Engine is editing.
 *
 ******************************************************************************/

bool MenuEngine_IsNavigationEnabled(void)
{

    if(g_menuEngine.editMode)
    {
        return false;
    }



    return true;

}





/******************************************************************************
 * Internal Synchronization
 ******************************************************************************/

/**
 * @brief
 *      Synchronizes internal runtime references.
 *
 ******************************************************************************/

static void MenuEngine_InternalRefresh(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return;
    }



    /*
     * Recover missing current item.
     */

    if(g_menuEngine.currentItem == NULL)
    {

        g_menuEngine.currentItem =
                g_menuEngine.currentPage->selectedItem;

    }



    /*
     * If still invalid,
     * select first item.
     */

    if(g_menuEngine.currentItem == NULL)
    {

        g_menuEngine.currentItem =
                g_menuEngine.currentPage->firstItem;


        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentItem;


        g_menuEngine.currentPage->selectedIndex =
                0U;

    }

}





/**
 * @brief
 *      Periodic engine service.
 *
 * @details
 *
 *      Call from main loop.
 *
 ******************************************************************************/

void MenuEngine_Service(void)
{

    MenuEngine_InternalRefresh();

}





/******************************************************************************
 *
 * End Of Part 5/6
 *
 ******************************************************************************/
/******************************************************************************
 * Selection Synchronization
 ******************************************************************************/

/**
 * @brief
 *      Synchronizes selected item with current page.
 *
 ******************************************************************************/

void MenuEngine_SynchronizeSelection(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return;
    }



    g_menuEngine.currentItem =
            g_menuEngine.currentPage->selectedItem;



    /*
     * Restore first item if selection is lost.
     */

    if(g_menuEngine.currentItem == NULL)
    {

        g_menuEngine.currentItem =
                g_menuEngine.currentPage->firstItem;


        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentItem;


        g_menuEngine.currentPage->selectedIndex =
                0U;

    }



    MenuEngine_RequestSelectionRefresh();

}





/******************************************************************************
 * Reset Current Page Selection
 ******************************************************************************/

/**
 * @brief
 *      Moves selection to first item of current page.
 *
 ******************************************************************************/

void MenuEngine_ResetCurrentPageSelection(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return;
    }



    g_menuEngine.currentPage->selectedItem =
            g_menuEngine.currentPage->firstItem;



    g_menuEngine.currentPage->selectedIndex =
            0U;



    g_menuEngine.currentItem =
            g_menuEngine.currentPage->selectedItem;



    MenuEngine_RequestFullRefresh();

}





/******************************************************************************
 * Debug Information
 ******************************************************************************/

/**
 * @brief
 *      Returns textual state name.
 *
 ******************************************************************************/

const char *MenuEngine_GetStateName(void)
{

    switch(g_menuEngine.state)
    {

        case MENU_STATE_IDLE:

            return "IDLE";



        case MENU_STATE_NAVIGATION:

            return "NAVIGATION";



        case MENU_STATE_EDIT:

            return "EDIT";



        case MENU_STATE_ACTION:

            return "ACTION";



        case MENU_STATE_MESSAGE:

            return "MESSAGE";



        case MENU_STATE_LOCKED:

            return "LOCKED";



        case MENU_STATE_ERROR:

            return "ERROR";



        default:

            return "UNKNOWN";

    }

}





/******************************************************************************
 * Runtime Validation
 ******************************************************************************/

/**
 * @brief
 *      Validates engine runtime objects.
 *
 ******************************************************************************/

bool MenuEngine_IsValid(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return false;
    }



    if(g_menuEngine.currentItem == NULL)
    {
        return false;
    }



    return true;

}





/******************************************************************************
 * Deinitialization
 ******************************************************************************/

/**
 * @brief
 *      Shuts down menu engine.
 *
 ******************************************************************************/

void MenuEngine_DeInit(void)
{

    g_menuEngine.currentPage =
            NULL;


    g_menuEngine.currentItem =
            NULL;


    g_menuEngine.state =
            MENU_STATE_IDLE;


    g_menuEngine.editMode =
            false;


    g_menuEngine.pageChanged =
            false;


    g_menuEngine.selectionChanged =
            false;


    g_menuEngine.lastEvent =
            MENU_EVENT_NONE;

}





/******************************************************************************
 * Revision History
 *
 ******************************************************************************

Version 2.1.0

Date:
2026-07-16


Changes:


    - Migrated runtime engine to new menu_types.h architecture.


    - Removed obsolete:

          MENU_ITEM_EDIT_BOOLEAN

          MENU_ITEM_EDIT_INTEGER

          MENU_ITEM_EDIT_FLOAT

          MENU_ITEM_EDIT_ENUM


    - Unified editable items under:

          MENU_ITEM_EDIT


    - Fixed callback compatibility.


    - Preserved static allocation model.


    - Preserved renderer separation.


******************************************************************************/




/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
