/******************************************************************************
 *
 * @file    menu_renderer.c
 *
 * @brief   Menu Renderer Implementation
 *          For Dual Voltage Monitor
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
 * Description :
 *
 *      This module converts Menu Engine runtime state into LCD output.
 *
 *      Responsibilities:
 *
 *          - Render page title
 *          - Render visible menu items
 *          - Render selection cursor
 *          - Synchronize LCD with Menu Engine refresh flags
 *
 *      This module does NOT:
 *
 *          - Read buttons
 *          - Change menu navigation
 *          - Modify menu values
 *          - Access application configuration
 *          - Access LCD hardware directly
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "menu_renderer.h"

#include <stdio.h>
#include <string.h>

/******************************************************************************
 * Private Rendestatic void MenuRenderer_ClearUnusedRows(void);rer Object
 ******************************************************************************/

static MenuRenderer_t g_menuRenderer;


/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/



static void MenuRenderer_DrawVisibleItems(void);

static MenuItem_t *MenuRenderer_GetVisibleStartItem(void);

static uint16_t MenuRenderer_GetVisibleStartIndex(void);

static void MenuRenderer_RenderItemLine(
        uint8_t lcdRow,
        MenuItem_t *item,
        bool selected);




/******************************************************************************
 * Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initializes renderer runtime.
 *
 ******************************************************************************/

void MenuRenderer_Init(
        LCD_DisplayHandle_t *display)
{

    memset(
            &g_menuRenderer,
            0,
            sizeof(g_menuRenderer));



    g_menuRenderer.display =
            display;



    g_menuRenderer.state =
            MENU_RENDERER_READY;



    g_menuRenderer.renderedPage =
            NULL;



    g_menuRenderer.renderedSelection =
            0U;



    g_menuRenderer.forceRedraw =
            true;

}



/******************************************************************************
 * Private Reset
 ******************************************************************************/



/******************************************************************************
 * Renderer Instance
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer runtime object.
 *
 ******************************************************************************/

MenuRenderer_t *MenuRenderer_GetInstance(void)
{

    return &g_menuRenderer;

}


/******************************************************************************
 * Visible Window Helper
 ******************************************************************************/

/**
 * @brief
 *      Calculates first visible menu index.
 *
 * @details
 *
 *      LCD:
 *
 *          Row0 -> Title
 *
 *          Row1
 *          Row2
 *          Row3
 *
 ******************************************************************************/

static uint16_t MenuRenderer_GetVisibleStartIndex(void)
{

    MenuPage_t *page;

    uint16_t selected;



    page =
            MenuEngine_GetCurrentPage();

    if(page == NULL)
    {
        return 0U;
    }



    selected =
            page->selectedIndex;



    if(selected < MENU_RENDERER_VISIBLE_ROWS)
    {
        return 0U;
    }



    return
        (selected - MENU_RENDERER_VISIBLE_ROWS + 1U);

}


/******************************************************************************
 * Visible Item Helper
 ******************************************************************************/

/**
 * @brief
 *      Returns first visible menu item.
 *
 ******************************************************************************/

static MenuItem_t *MenuRenderer_GetVisibleStartItem(void)
{

    MenuPage_t *page;

    MenuItem_t *item;

    uint16_t index;

    uint16_t firstVisible;



    page =
            MenuEngine_GetCurrentPage();

    if(page == NULL)
    {
        return NULL;
    }



    item =
            page->firstItem;



    firstVisible =
            MenuRenderer_GetVisibleStartIndex();



    for(index = 0U;
        index < firstVisible;
        index++)
    {

        if(item == NULL)
        {
            break;
        }

        item =
                item->next;

    }



    return item;

}

/******************************************************************************
 *
 * End Of Part 1/6
 *
 ******************************************************************************/

/******************************************************************************
 * Display Formatting Helpers
 ******************************************************************************/

/**
 * @brief
 *      Formats page title.
 *
 ******************************************************************************/

void MenuRenderer_FormatPageTitle(
        MenuPage_t *page,
        char *buffer,
        uint16_t size)
{

    if((page == NULL) ||
       (buffer == NULL) ||
       (size == 0U))
    {
        return;
    }


    snprintf(
            buffer,
            size,
            "%s",
            page->title);

}





/**
 * @brief
 *      Formats menu item text.
 *
 ******************************************************************************/

void MenuRenderer_FormatItemText(
        MenuItem_t *item,
        char *buffer,
        uint16_t size)
{

    if((item == NULL) ||
       (buffer == NULL) ||
       (size == 0U))
    {
        return;
    }


    switch(item->type)
    {

        case MENU_ITEM_SUBMENU:

            snprintf(
                    buffer,
                    size,
                    "%s",
                    item->title);

            break;



        case MENU_ITEM_ACTION:

            snprintf(
                    buffer,
                    size,
                    "%s",
                    item->title);

            break;



        case MENU_ITEM_EDIT:

            snprintf(
                    buffer,
                    size,
                    "%s",
                    item->title);

            break;



        case MENU_ITEM_INFO:

            snprintf(
                    buffer,
                    size,
                    "%s",
                    item->title);

            break;



        case MENU_ITEM_MONITOR:

            snprintf(
                    buffer,
                    size,
                    "%s",
                    item->title);

            break;



        default:

            snprintf(
                    buffer,
                    size,
                    "%s",
                    item->title);

            break;

    }

}





/******************************************************************************
 * Line Rendering
 ******************************************************************************/

/**
 * @brief
 *      Renders one LCD menu line.
 *
 ******************************************************************************/

static void MenuRenderer_RenderItemLine(
        uint8_t lcdRow,
        MenuItem_t *item,
        bool selected)
{

    char line[MENU_RENDERER_TEXT_SIZE];

    char text[MENU_RENDERER_TEXT_SIZE];



    memset(
            line,
            0,
            sizeof(line));


    memset(
            text,
            0,
            sizeof(text));



    if(item != NULL)
    {

        MenuRenderer_FormatItemText(
                item,
                text,
                sizeof(text));

    }



    if(selected)
    {

        snprintf(
                line,
                sizeof(line),
                "%c %-17.17s",
                MENU_RENDERER_CURSOR_CHARACTER,
                text);

    }
    else
    {

        snprintf(
                line,
                sizeof(line),
                "  %-18.18s",
                text);

    }



    LCD_Display_PrintLine(
            lcdRow,
            line);

}





/******************************************************************************
 * LCD Cleanup
 ******************************************************************************/

/**
 * @brief
 *      Clears remaining LCD rows.
 *
 ******************************************************************************/






/******************************************************************************
 * Visible Item Rendering
 ******************************************************************************/

/**
 * @brief
 *      Draws currently visible menu items.
 *
 ******************************************************************************/

static void MenuRenderer_DrawVisibleItems(void)
{

    MenuPage_t *page;

    MenuItem_t *item;

    uint16_t selectedIndex;

    uint16_t firstVisible;

    uint16_t currentIndex;

    uint8_t lcdRow;



    page =
            MenuEngine_GetCurrentPage();

    if(page == NULL)
    {
        return;
    }



    item =
            MenuRenderer_GetVisibleStartItem();


    firstVisible =
            MenuRenderer_GetVisibleStartIndex();


    selectedIndex =
            page->selectedIndex;



    lcdRow = 1U;

    currentIndex = firstVisible;



    while((item != NULL) &&
          (lcdRow <= MENU_RENDERER_VISIBLE_ROWS))
    {

        MenuRenderer_RenderItemLine(
                lcdRow,
                item,
                (currentIndex == selectedIndex));


        item =
                item->next;


        currentIndex++;


        lcdRow++;

    }



    while(lcdRow <= MENU_RENDERER_VISIBLE_ROWS)
    {

        LCD_Display_PrintLine(
                lcdRow,
                "");


        lcdRow++;

    }

}


/******************************************************************************
 *
 * End Of Part 2/6
 *
 ******************************************************************************/
/******************************************************************************
 * Complete Page Rendering
 ******************************************************************************/

/**
 * @brief
 *      Renders the complete current menu page.
 *
 * @details
 *
 *      Rendering sequence:
 *
 *          1. Draw page title.
 *          2. Draw visible menu items.
 *          3. Update renderer cache.
 *          4. Clear refresh flags.
 *
 ******************************************************************************/

void MenuRenderer_RenderPage(void)
{

    MenuPage_t *page;

    char title[MENU_RENDERER_TEXT_SIZE];



    page =
            MenuEngine_GetCurrentPage();

    if(page == NULL)
    {
        return;
    }



    memset(
            title,
            0,
            sizeof(title));



    MenuRenderer_FormatPageTitle(
            page,
            title,
            sizeof(title));



    LCD_Display_ShowTitle(
            title);



    MenuRenderer_DrawVisibleItems();



    g_menuRenderer.renderedPage =
            page;



    g_menuRenderer.renderedSelection =
            page->selectedIndex;



    g_menuRenderer.forceRedraw =
            false;



    MenuEngine_ClearPageChanged();


    MenuEngine_ClearSelectionChanged();



    LCD_Display_ClearRefreshFlag();

}





/******************************************************************************
 * Complete Renderer Entry
 ******************************************************************************/

/**
 * @brief
 *      Performs complete rendering.
 *
 ******************************************************************************/

void MenuRenderer_Render(void)
{

    MenuRenderer_RenderPage();

}





/******************************************************************************
 * Renderer Update Service
 ******************************************************************************/

/**
 * @brief
 *      Renderer periodic service.
 *
 * @details
 *
 *      Refresh priority:
 *
 *          1.
 *          Full redraw request
 *
 *          2.
 *          Page refresh request
 *
 *          3.
 *          Selection refresh request
 *
 ******************************************************************************/

void MenuRenderer_Update(void)
{

    if(MenuRenderer_IsInitialized() == false)
    {
        return;
    }



    if(MenuRenderer_NeedRedraw())
    {

        MenuRenderer_RenderPage();

        return;

    }



    if(MenuEngine_IsPageChanged())
    {

        MenuRenderer_RenderPage();

        return;

    }



    if(MenuEngine_IsSelectionChanged())
    {

        MenuRenderer_RenderSelection();

        return;

    }

}


/******************************************************************************
 *
 * End Of Part 3/6
 *
 ******************************************************************************/


/******************************************************************************
 * Selection Rendering
 ******************************************************************************/

/**
 * @brief
 *      Updates only menu selection.
 *
 * @details
 *
 *      This function redraws only the currently visible menu rows.
 *
 *      The page title is preserved.
 *
 *      This minimizes LCD traffic while keeping the implementation
 *      simple and compatible with the current LCD abstraction layer.
 *
 ******************************************************************************/

void MenuRenderer_RenderSelection(void)
{

    MenuPage_t *page;

    page =
            MenuEngine_GetCurrentPage();

    if(page == NULL)
    {
        return;
    }



    /*
     * Redraw visible menu rows.
     *
     * The title remains unchanged.
     */
    MenuRenderer_DrawVisibleItems();



    /*
     * Cache current renderer state.
     */
    g_menuRenderer.renderedPage =
            page;


    g_menuRenderer.renderedSelection =
            page->selectedIndex;



    /*
     * Selection update completed.
     */
    MenuEngine_ClearSelectionChanged();



    LCD_Display_ClearRefreshFlag();

}





/******************************************************************************
 * Refresh Management
 ******************************************************************************/

/**
 * @brief
 *      Requests complete renderer redraw.
 *
 ******************************************************************************/

void MenuRenderer_RequestFullRedraw(void)
{

    g_menuRenderer.forceRedraw =
            true;

}





/**
 * @brief
 *      Clears redraw request.
 *
 ******************************************************************************/

void MenuRenderer_ClearRedrawRequest(void)
{

    g_menuRenderer.forceRedraw =
            false;

}





/**
 * @brief
 *      Returns redraw request state.
 *
 ******************************************************************************/

bool MenuRenderer_NeedRedraw(void)
{

    return
        g_menuRenderer.forceRedraw;

}





/******************************************************************************
 * Renderer State
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer state.
 *
 ******************************************************************************/

MenuRendererState_t MenuRenderer_GetState(void)
{

    return
        g_menuRenderer.state;

}





/**
 * @brief
 *      Checks renderer initialization state.
 *
 ******************************************************************************/

bool MenuRenderer_IsInitialized(void)
{

    return
        (g_menuRenderer.state ==
         MENU_RENDERER_READY);

}


/******************************************************************************
 *
 * End Of Part 4/6
 *
 ******************************************************************************/

/******************************************************************************
 * Validation
 ******************************************************************************/

/**
 * @brief
 *      Validates renderer runtime object.
 *
 * @details
 *
 *      Renderer is considered valid when:
 *
 *          - Renderer has been initialized.
 *          - Display handle exists.
 *
 ******************************************************************************/

bool MenuRenderer_IsValid(void)
{

    if(g_menuRenderer.state != MENU_RENDERER_READY)
    {
        return false;
    }



    if(g_menuRenderer.display == NULL)
    {
        return false;
    }



    return true;

}





/******************************************************************************
 * Renderer Synchronization Service
 ******************************************************************************/

/**
 * @brief
 *      Synchronizes renderer with current engine state.
 *
 * @details
 *
 *      This helper keeps renderer cache coherent if another module
 *      changes the current page without requesting a redraw.
 *
 ******************************************************************************/

void MenuRenderer_Service(void)
{

    MenuPage_t *page;

    page =
            MenuEngine_GetCurrentPage();

    if(page == NULL)
    {
        return;
    }



    /*
     * Detect page replacement.
     */
    if(page != g_menuRenderer.renderedPage)
    {

        MenuRenderer_RequestFullRedraw();

    }



    /*
     * Detect selection change.
     */
    if(page->selectedIndex !=
       g_menuRenderer.renderedSelection)
    {

        MenuEngine_RequestSelectionRefresh();

    }

}





/******************************************************************************
 * Renderer Debug Support
 ******************************************************************************/

/**
 * @brief
 *      Forces complete renderer synchronization.
 *
 ******************************************************************************/

void MenuRenderer_DebugRefresh(void)
{

    MenuRenderer_RequestFullRedraw();


    MenuRenderer_Update();

}





/******************************************************************************
 * Internal Design Notes
 ******************************************************************************/

/*

Renderer refresh policy:


    Full redraw:

        - Page title
        - Visible menu items
        - Cursor


    Selection redraw:

        - Visible menu rows only


Reason:

    The current LCD abstraction provides line-based drawing.

    Therefore selection refresh redraws only the menu area
    (rows 1..3) while preserving the title.

*/


/******************************************************************************
 *
 * End Of Part 5/6
 *
 ******************************************************************************/
/******************************************************************************
 * Version Information
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer module version string.
 *
 * @return
 *
 *      Constant version text.
 *
 ******************************************************************************/

const char *MenuRenderer_GetVersion(void)
{

    return MENU_RENDERER_VERSION_STRING;

}





/******************************************************************************
 * End Of File Cleanup
 ******************************************************************************/

/**
 * @brief
 *      Releases renderer runtime state.
 *
 * @details
 *
 *      Static allocation is used in this project.
 *
 *      Therefore this function only resets runtime references.
 *
 ******************************************************************************/

void MenuRenderer_DeInit(void)
{

    memset(
            &g_menuRenderer,
            0,
            sizeof(g_menuRenderer));



    g_menuRenderer.state =
            MENU_RENDERER_NOT_INITIALIZED;

}





/******************************************************************************
 * Revision History
 ******************************************************************************/

/*

Version 1.0.0

Date:

2026-07-16


Changes:


    - Initial implementation of menu renderer.


    - Added LCD abstraction support.


    - Added page rendering.


    - Added selection rendering.


    - Added cursor management.


    - Added refresh synchronization.


    - Preserved separation between:


            Menu Database

            Menu Engine

            Menu Renderer

            LCD Display Layer



Design Rules:


    1.

    Renderer does not modify menu state.



    2.

    Renderer does not access hardware driver directly.



    3.

    Renderer does not process input events.



    4.

    Renderer uses static memory only.



    5.

    Partial refresh is supported.



*/


/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
