/******************************************************************************
 *
 * @file    menu_renderer.c
 *
 * @brief   Page Based Menu Renderer Implementation
 *          For Dual Voltage Monitor
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
 *      This module converts Menu Engine state into LCD display output.
 *
 *      Renderer responsibilities:
 *
 *          - Render current menu page.
 *          - Display page title.
 *          - Display visible menu items.
 *          - Display selection cursor.
 *          - Manage display refresh requests.
 *
 *
 *      Renderer does NOT:
 *
 *          - Process buttons.
 *          - Change navigation state.
 *          - Modify menu items.
 *          - Access application data.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Architecture:
 *
 *
 *          menu_items.c
 *                |
 *                v
 *
 *          menu_engine.c
 *                |
 *                v
 *
 *          menu_renderer.c
 *                |
 *                v
 *
 *          lcd_display.c
 *                |
 *                v
 *
 *          lcd_i2c.c
 *
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      1.0.2
 *
 ******************************************************************************/




/******************************************************************************
 * Includes
 ******************************************************************************/

#include <menu_renderer_old.h>
#include <string.h>




/******************************************************************************
 * Private Variables
 ******************************************************************************/

/*
 * Static renderer instance.
 *
 * Dynamic memory allocation is intentionally not used.
 *
 * The renderer owns only presentation state.
 */
static MenuRenderer_t g_menuRenderer;




/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void MenuRenderer_ProcessRefresh(void);


static void MenuRenderer_RenderItemRows(void);


static void MenuRenderer_RenderEmptyRows(void);


static bool MenuRenderer_IsDisplayReady(void);


static void MenuRenderer_SynchronizeState(void);




/******************************************************************************
 * Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initializes menu renderer module.
 *
 * @param display
 *      Pointer to LCD display abstraction handle.
 *
 * @example
 *
 *      LCD_Display_Init(&hi2c1);
 *
 *      MenuRenderer_Init(
 *              LCD_Display_GetHandle());
 *
 ******************************************************************************/

void MenuRenderer_Init(
        LCD_DisplayHandle_t *display)
{

    memset(
            &g_menuRenderer,
            0,
            sizeof(g_menuRenderer));



    if(display == NULL)
    {

        g_menuRenderer.state =
                MENU_RENDERER_ERROR;


        return;

    }



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
 * Instance Access
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer instance.
 *
 * @return
 *      Pointer to renderer object.
 *
 ******************************************************************************/

MenuRenderer_t *MenuRenderer_GetInstance(void)
{

    return &g_menuRenderer;

}




/******************************************************************************
 * Renderer Update Service
 ******************************************************************************/

/**
 * @brief
 *      Executes renderer periodic service.
 *
 * @details
 *
 *      This function should be called from main loop.
 *
 *      Example:
 *
 *          while(1)
 *          {
 *
 *              MenuEngine_Service();
 *
 *              MenuRenderer_Update();
 *
 *          }
 *
 ******************************************************************************/

void MenuRenderer_Update(void)
{

    if(MenuRenderer_IsInitialized() == false)
    {
        return;
    }



    MenuRenderer_ProcessRefresh();

}




/******************************************************************************
 * Complete Rendering
 ******************************************************************************/

/**
 * @brief
 *      Performs complete LCD rendering.
 *
 * @details
 *
 *      Render sequence:
 *
 *          1. Draw page title.
 *
 *          2. Draw visible menu items.
 *
 *          3. Draw selection cursor.
 *
 ******************************************************************************/

void MenuRenderer_Render(void)
{

    if(MenuRenderer_IsDisplayReady() == false)
    {
        return;
    }



    MenuRenderer_RenderPage();


    MenuRenderer_RenderSelection();

}




/******************************************************************************
 * Page Rendering
 ******************************************************************************/

/**
 * @brief
 *      Renders current menu page.
 *
 ******************************************************************************/

void MenuRenderer_RenderPage(void)
{

    MenuPage_t *page;


    char title[
            MENU_RENDERER_TEXT_SIZE];



    page =
        MenuEngine_GetCurrentPage();



    if(page == NULL)
    {
        return;
    }



    MenuRenderer_FormatPageTitle(
            page,
            title,
            sizeof(title));



    LCD_Display_Clear();



    LCD_Display_ShowTitle(
            title);



    MenuRenderer_RenderItemRows();



    MenuRenderer_RenderEmptyRows();



    LCD_Display_ClearRefreshFlag();

}



/******************************************************************************
 *
 * End Of Part 1/4
 *
 ******************************************************************************/

/******************************************************************************
 * Item Rows Rendering
 ******************************************************************************/

/**
 * @brief
 *      Renders visible menu items of current page.
 *
 * @details
 *
 *      Page Based Rendering:
 *
 *          Row 0:
 *              Page title
 *
 *          Row 1:
 *              Item 0
 *
 *          Row 2:
 *              Item 1
 *
 *          Row 3:
 *              Item 2
 *
 *
 *      No scrolling calculation exists.
 *
 *      Renderer always starts from:
 *
 *              page->firstItem
 *
 ******************************************************************************/

static void MenuRenderer_RenderItemRows(void)
{

    MenuPage_t *page;

    MenuItem_t *item;

    char line[
            MENU_RENDERER_TEXT_SIZE];



    page =
        MenuEngine_GetCurrentPage();



    if(page == NULL)
    {
        return;
    }



    item =
        page->firstItem;



    for(uint8_t row = 0U;
        row < MENU_RENDERER_VISIBLE_ROWS;
        row++)
    {

        memset(
                line,
                0,
                sizeof(line));



        if(item != NULL)
        {

            MenuRenderer_FormatItemText(
                    item,
                    line,
                    sizeof(line));



            LCD_Display_PrintLine(
                    row + 1U,
                    line);



            item =
                    item->next;

        }
        else
        {

            LCD_Display_PrintLine(
                    row + 1U,
                    "");

        }

    }

}




/******************************************************************************
 * Selection Rendering
 ******************************************************************************/

/**
 * @brief
 *      Updates visible selection cursor.
 *
 * @details
 *
 *      This function only updates presentation state.
 *
 *      Navigation ownership remains inside menu_engine.c.
 *
 ******************************************************************************/

void MenuRenderer_RenderSelection(void)
{

    MenuPage_t *page;

    MenuItem_t *item;

    char line[
            MENU_RENDERER_TEXT_SIZE];



    uint16_t selectedIndex;



    page =
        MenuEngine_GetCurrentPage();



    if(page == NULL)
    {
        return;
    }



    selectedIndex =
            page->selectedIndex;



    /*
     * Page Based architecture supports only visible rows.
     *
     * If engine reports an invalid selection index,
     * renderer ignores it to prevent memory access errors.
     */
    if(selectedIndex >= MENU_RENDERER_VISIBLE_ROWS)
    {
        return;
    }



    item =
        page->firstItem;



    for(uint8_t index = 0U;
        index < MENU_RENDERER_VISIBLE_ROWS;
        index++)
    {

        memset(
                line,
                0,
                sizeof(line));



        if(item == NULL)
        {

            LCD_Display_PrintLine(
                    index + 1U,
                    "");



            continue;

        }



        if(index == selectedIndex)
        {

            line[0] =
                    MENU_RENDERER_CURSOR_CHARACTER;



            line[1] =
                    MENU_RENDERER_SPACE_CHARACTER;



            if(item->title != NULL)
            {

                strncpy(
                        &line[2],
                        item->title,
                        sizeof(line) - 3U);

            }

        }
        else
        {

            if(item->title != NULL)
            {

                strncpy(
                        line,
                        item->title,
                        sizeof(line) - 1U);

            }

        }



        LCD_Display_PrintLine(
                index + 1U,
                line);



        item =
                item->next;

    }



    g_menuRenderer.renderedSelection =
            selectedIndex;

}




/******************************************************************************
 * Empty Row Handling
 ******************************************************************************/

/**
 * @brief
 *      Clears unused LCD item rows.
 *
 * @details
 *
 *      When a page contains fewer items than visible rows,
 *      remaining LCD rows are cleared.
 *
 ******************************************************************************/

static void MenuRenderer_RenderEmptyRows(void)
{

    MenuPage_t *page;

    uint16_t itemCount;



    page =
        MenuEngine_GetCurrentPage();



    if(page == NULL)
    {
        return;
    }



    itemCount =
            page->itemCount;



    if(itemCount < MENU_RENDERER_VISIBLE_ROWS)
    {

        for(uint8_t row =
                (uint8_t)(itemCount + 1U);

            row <= MENU_RENDERER_VISIBLE_ROWS;

            row++)
        {

            LCD_Display_PrintLine(
                    row,
                    "");

        }

    }

}




/******************************************************************************
 * Item Text Formatting
 ******************************************************************************/

/**
 * @brief
 *      Converts menu item data into display text.
 *
 * @details
 *
 *      This function prepares text only.
 *
 *      It does not communicate with LCD hardware.
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



    memset(
            buffer,
            0,
            size);



    if(item->title != NULL)
    {

        strncpy(
                buffer,
                item->title,
                size - 1U);

    }

}




/******************************************************************************
 * Page Title Formatting
 ******************************************************************************/

/**
 * @brief
 *      Converts page title into display text.
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



    memset(
            buffer,
            0,
            size);



    if(page->title != NULL)
    {

        strncpy(
                buffer,
                page->title,
                size - 1U);

    }

}




/******************************************************************************
 *
 * End Of Part 2/4
 *
 ******************************************************************************/

/******************************************************************************
 * Renderer Synchronization
 ******************************************************************************/

/**
 * @brief
 *      Synchronizes renderer cached state.
 *
 * @details
 *
 *      Renderer keeps only presentation information.
 *
 *      Menu engine remains the owner of:
 *
 *          - Navigation state
 *          - Current selection
 *          - Page transitions
 *
 ******************************************************************************/

static void MenuRenderer_SynchronizeState(void)
{

    MenuPage_t *page;



    page =
        MenuEngine_GetCurrentPage();



    if(page == NULL)
    {
        return;
    }



    g_menuRenderer.renderedPage =
            page;



    g_menuRenderer.renderedSelection =
            page->selectedIndex;

}




/******************************************************************************
 * Display Validation
 ******************************************************************************/

/**
 * @brief
 *      Checks LCD display availability.
 *
 * @return
 *
 *      true:
 *          Display handle is available.
 *
 ******************************************************************************/

static bool MenuRenderer_IsDisplayReady(void)
{

    if(g_menuRenderer.display == NULL)
    {
        return false;
    }



    return true;

}




/******************************************************************************
 * Refresh Processing
 ******************************************************************************/

/**
 * @brief
 *      Processes pending renderer refresh requests.
 *
 * @details
 *
 *      Refresh priority:
 *
 *          1. Forced complete redraw
 *
 *          2. Page change redraw
 *
 *          3. Selection update
 *
 ******************************************************************************/

static void MenuRenderer_ProcessRefresh(void)
{

    if(MenuRenderer_IsDisplayReady() == false)
    {
        return;
    }



    /*
     * Highest priority:
     *
     * Complete redraw request.
     */
    if(g_menuRenderer.forceRedraw == true)
    {

        MenuRenderer_Render();



        g_menuRenderer.forceRedraw =
                false;



        MenuRenderer_SynchronizeState();



        return;

    }




    /*
     * Page change handling.
     *
     * Page title and all menu rows
     * must be redrawn.
     */
    if(MenuEngine_IsPageChanged())
    {

        MenuRenderer_RenderPage();



        MenuEngine_ClearPageChanged();



        MenuRenderer_SynchronizeState();

    }




    /*
     * Selection change handling.
     *
     * Only cursor presentation is updated.
     */
    if(MenuEngine_IsSelectionChanged())
    {

        MenuRenderer_RenderSelection();



        MenuEngine_ClearSelectionChanged();



        MenuRenderer_SynchronizeState();

    }

}




/******************************************************************************
 * Refresh Management API
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
 *      Checks redraw request state.
 *
 * @return
 *
 *      true:
 *          Full redraw requested.
 *
 ******************************************************************************/

bool MenuRenderer_NeedRedraw(void)
{

    return
        g_menuRenderer.forceRedraw;

}




/******************************************************************************
 * Renderer State Access
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer state.
 *
 ******************************************************************************/

MenuRendererState_t MenuRenderer_GetState(void)
{

    return g_menuRenderer.state;

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




/**
 * @brief
 *      Checks renderer object validity.
 *
 ******************************************************************************/

bool MenuRenderer_IsValid(void)
{

    if(g_menuRenderer.state !=
            MENU_RENDERER_READY)
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
 *
 * End Of Part 3/4
 *
 ******************************************************************************/

/******************************************************************************
 * End Of File Services
 ******************************************************************************/

/**
 * @brief
 *      Resets renderer internal state.
 *
 * @details
 *
 *      This function resets only renderer presentation state.
 *
 *      Menu engine state is not modified.
 *
 ******************************************************************************/

void MenuRenderer_Reset(void)
{

    g_menuRenderer.renderedPage =
            NULL;



    g_menuRenderer.renderedSelection =
            0U;



    g_menuRenderer.forceRedraw =
            true;



    if(g_menuRenderer.display != NULL)
    {

        g_menuRenderer.state =
                MENU_RENDERER_READY;

    }
    else
    {

        g_menuRenderer.state =
                MENU_RENDERER_ERROR;

    }

}




/**
 * @brief
 *      Deinitializes renderer module.
 *
 * @details
 *
 *      Dynamic memory is not used.
 *
 *      Only internal state is cleared.
 *
 ******************************************************************************/

void MenuRenderer_DeInit(void)
{

    g_menuRenderer.display =
            NULL;



    g_menuRenderer.renderedPage =
            NULL;



    g_menuRenderer.renderedSelection =
            0U;



    g_menuRenderer.forceRedraw =
            false;



    g_menuRenderer.state =
            MENU_RENDERER_NOT_INITIALIZED;

}




/******************************************************************************
 * Revision History
 *
 ******************************************************************************

Version 1.0.2

Date:
2026-07-22


Changes:


    - Cleaned renderer implementation.


    - Verified compatibility with:

          menu_types.h      v2.0.0

          menu_engine.h     v2.1.0

          menu_renderer.h  v1.0.1

          lcd_display.h    v1.0.0



    - Removed unsupported LCD API usage.


    - Removed scrolling logic completely.


    - Preserved Page Based rendering architecture.


    - Removed navigation ownership from renderer.


    - Added selectedIndex boundary protection.


    - Removed unused renderer helper functions.


    - Preserved static memory architecture.


    - Renderer remains independent from:

          Buttons

          ADC

          UART

          Flash

          Application settings



 ******************************************************************************/

/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
