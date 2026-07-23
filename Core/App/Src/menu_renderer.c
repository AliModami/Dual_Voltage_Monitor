/******************************************************************************
 *
 * @file    menu_renderer.c
 *
 * @brief   Page Based Menu Renderer Implementation
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
 *      This file implements the presentation layer of the menu system.
 *
 *      The renderer is responsible only for converting the current
 *      menu controller state into display-ready information.
 *
 *      Responsibilities:
 *
 *      - Read current page information from menu_controller.
 *      - Read selected item position.
 *      - Prepare title and item strings.
 *      - Provide cursor position for LCD layer.
 *
 *
 *      This module does NOT handle:
 *
 *      - Button input.
 *      - Menu navigation.
 *      - Application actions.
 *      - LCD hardware access.
 *      - I2C communication.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Architecture:
 *
 *
 *          menu_items.c
 *               |
 *               v
 *       menu_controller.c
 *               |
 *               v
 *       menu_renderer.c
 *               |
 *               v
 *        lcd_display.c
 *               |
 *               v
 *          lcd_i2c.c
 *
 *
 *------------------------------------------------------------------------------
 *
 * Design Rules:
 *
 *      1- Renderer never modifies menu state.
 *
 *      2- Renderer only reads data from controller.
 *
 *      3- Each LCD page contains:
 *
 *              Row 0 : Page title
 *              Row 1 : Menu item 0
 *              Row 2 : Menu item 1
 *              Row 3 : Menu item 2
 *
 *
 *      4- No scrolling mechanism exists.
 *
 *      5- Large menus are handled by multiple pages.
 *
 ******************************************************************************/




/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_renderer.h"

#include "menu_controller.h"

#include <stddef.h>





/******************************************************************************
 *                              Private Variables
 ******************************************************************************/

/*
 * Renderer output buffer.
 *
 * This structure stores prepared information
 * for the LCD display layer.
 *
 * The LCD driver never accesses menu data directly.
 * It only reads this prepared buffer.
 */
static MenuRenderData_t render_data;






/******************************************************************************
 *                         Private Function Prototypes
 ******************************************************************************/

/**
 * @brief
 *      Clear renderer output buffer.
 *
 * @details
 *
 *      Before generating new display data,
 *      old information must be removed.
 *
 *      This prevents displaying outdated menu
 *      information after changing pages.
 */
static void MenuRenderer_ClearBuffer(void);




/**
 * @brief
 *      Copy current page information into renderer buffer.
 *
 * @param page
 *      Pointer to active menu page.
 *
 * @details
 *
 *      This function converts menu database information
 *      into display-oriented information.
 */
static void MenuRenderer_FillPageItems(
        const MenuPage_t *page);






/******************************************************************************
 *                         Private Functions
 ******************************************************************************/

/**
 * @brief
 *      Clear renderer data buffer.
 *
 * @details
 *
 *      All fields are initialized to safe values.
 *
 *      NULL pointers are used for unavailable strings.
 */
static void MenuRenderer_ClearBuffer(void)
{
    uint8_t index;



    /*
     * Remove previous page title.
     */
    render_data.title = NULL;



    /*
     * Clear all visible item strings.
     */
    for(index = 0U;
        index < MENU_RENDER_VISIBLE_ITEMS;
        index++)
    {
        render_data.items[index] = NULL;
    }



    /*
     * Reset cursor position.
     */
    render_data.cursor_position = 0U;



    /*
     * No visible items available.
     */
    render_data.item_count = 0U;
}





/**
 * @brief
 *      Fill renderer buffer with current page data.
 *
 * @param page
 *      Current active menu page.
 *
 * @details
 *
 *      The renderer copies only information needed
 *      by the display layer.
 *
 *      Menu logic remains completely independent.
 */
static void MenuRenderer_FillPageItems(
        const MenuPage_t *page)
{

    uint8_t index;



    /*
     * Safety check.
     */
    if(page == NULL)
    {
        return;
    }



    /*
     * Copy page title.
     */
    render_data.title = page->title;



    /*
     * Copy number of available items.
     *
     * The value is limited to the maximum
     * visible LCD rows.
     */
    if(page->item_count > MENU_RENDER_VISIBLE_ITEMS)
    {
        render_data.item_count =
                MENU_RENDER_VISIBLE_ITEMS;
    }
    else
    {
        render_data.item_count =
                page->item_count;
    }



    /*
     * Copy menu item text.
     *
     * Only visible page items are copied.
     */
    for(index = 0U;
        index < render_data.item_count;
        index++)
    {
        render_data.items[index] =
                page->items[index].text;
    }

}

/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu renderer.
 *
 * @details
 *
 *      This function prepares the renderer
 *      internal buffer before the first display update.
 *
 *      It must be called once during system startup.
 *
 *
 * Example:
 *
 *      int main(void)
 *      {
 *          HAL_Init();
 *
 *          MenuController_Init();
 *          MenuRenderer_Init();
 *
 *          while(1)
 *          {
 *              MenuRenderer_Update();
 *          }
 *      }
 *
 */
void MenuRenderer_Init(void)
{
    /*
     * Initialize output buffer.
     */
    MenuRenderer_ClearBuffer();
}






/**
 * @brief
 *      Update renderer data from menu controller.
 *
 * @details
 *
 *      This function reads the current menu state
 *      and prepares display information.
 *
 *
 *      Data flow:
 *
 *
 *          menu_controller
 *
 *                 |
 *                 |
 *                 v
 *
 *          MenuRenderer_Update()
 *
 *                 |
 *                 |
 *                 v
 *
 *          MenuRenderData_t
 *
 *
 *
 *      The renderer never changes menu state.
 *
 */
void MenuRenderer_Update(void)
{
    const MenuPage_t *page;

    uint8_t selected_index;



    /*
     * Remove previous rendering information.
     */
    MenuRenderer_ClearBuffer();



    /*
     * Get current active page
     * from menu controller.
     */
    page = MenuController_GetCurrentPage();



    /*
     * Stop if current page is invalid.
     */
    if(page == NULL)
    {
        return;
    }



    /*
     * Generate visible page information.
     */
    MenuRenderer_FillPageItems(page);




    /*
     * Get selected menu item index.
     */
    selected_index =
            MenuController_GetSelectedIndex();



    /*
     * Convert controller selection
     * into LCD cursor position.
     *
     * In Page Based navigation:
     *
     *      selected item index
     *
     *              =
     *
     *      LCD cursor row offset
     *
     *
     * Example:
     *
     *      Item 0 -> Row 1
     *      Item 1 -> Row 2
     *      Item 2 -> Row 3
     *
     */
    if(selected_index < render_data.item_count)
    {
        render_data.cursor_position =
                selected_index;
    }
    else
    {
        /*
         * Safety fallback.
         *
         * If controller reports invalid
         * selection, place cursor on first item.
         */
        render_data.cursor_position = 0U;
    }

}






/**
 * @brief
 *      Get prepared renderer information.
 *
 * @return
 *
 *      Pointer to read-only rendering data.
 *
 *
 * @details
 *
 *      The LCD display layer uses this function
 *      to retrieve information prepared by renderer.
 *
 *
 * Example:
 *
 *      const MenuRenderData_t *data;
 *
 *
 *      data = MenuRenderer_GetData();
 *
 *
 *      LCD_Print(data->title);
 *
 *
 */
const MenuRenderData_t *MenuRenderer_GetData(void)
{
    return &render_data;
}

/******************************************************************************
 *
 *                              Interface Notes
 *
 ******************************************************************************/

/*
 * Renderer API Usage Example
 *
 *
 * Application main loop:
 *
 *
 * while(1)
 * {
 *
 *      Buttons_Task();
 *
 *      ButtonApp_Task();
 *
 *
 *      if(ButtonApp_GetCommand(&command))
 *      {
 *
 *          switch(command)
 *          {
 *
 *              case BUTTON_CMD_UP:
 *
 *                  MenuController_MoveUp();
 *                  break;
 *
 *
 *              case BUTTON_CMD_DOWN:
 *
 *                  MenuController_MoveDown();
 *                  break;
 *
 *
 *              case BUTTON_CMD_ENTER:
 *
 *                  MenuController_Enter();
 *                  break;
 *
 *
 *              case BUTTON_CMD_BACK:
 *
 *                  MenuController_Back();
 *                  break;
 *
 *          }
 *      }
 *
 *
 *      MenuRenderer_Update();
 *
 *
 *      LCD_Display_Menu(
 *              MenuRenderer_GetData());
 *
 *
 * }
 *
 *
 *
 * Notice:
 *
 *      Renderer does not know anything about buttons.
 *
 *      Renderer does not execute menu actions.
 *
 *      Renderer only converts menu state
 *      into display information.
 *
 */






/******************************************************************************
 *
 *                              Design Verification
 *
 ******************************************************************************/

/*
 *
 * Compatibility checklist:
 *
 *
 *  [OK] Uses menu_controller public API only.
 *
 *  [OK] Uses menu_items data structures only.
 *
 *  [OK] No dependency on legacy menu_engine.
 *
 *  [OK] No dependency on menu_types.
 *
 *  [OK] No dependency on menu_ids.
 *
 *  [OK] No dependency on LCD driver.
 *
 *  [OK] No dependency on button layer.
 *
 *  [OK] Supports Page Based navigation.
 *
 *  [OK] Supports LCD 20x4 layout.
 *
 *
 */






/******************************************************************************
 *
 *                              End Of File
 *
 ******************************************************************************/
