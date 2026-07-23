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
 *      Dual Voltage Monitor - Page Based Menu Prototype
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file converts menu controller state into
 *      display-ready information.
 *
 *      Responsibilities:
 *
 *      - Read current page.
 *      - Read selected item.
 *      - Prepare visible item list.
 *      - Provide cursor position.
 *
 *      This module does NOT access:
 *
 *      - LCD hardware
 *      - I2C
 *      - GPIO
 *
 ******************************************************************************/

#include "menu_renderer.h"

#include "menu_controller.h"



/*
 * --------------------------------------------------------------------------
 * Private variables
 * --------------------------------------------------------------------------
 */


/*
 * Current renderer output buffer.
 */
static MenuRenderData_t render_data;



/*
 * --------------------------------------------------------------------------
 * Public Functions
 * --------------------------------------------------------------------------
 */


/*
 * Initialize renderer.
 */
void MenuRenderer_Init(void)
{
    render_data.title = 0;


    for (uint8_t i = 0U;
         i < MENU_RENDER_VISIBLE_ITEMS;
         i++)
    {
        render_data.items[i] = 0;
    }


    render_data.cursor_position = 0U;

    render_data.item_count = 0U;
}



/*
 * Update rendering information.
 */
void MenuRenderer_Update(void)
{
    const MenuPage_t *page;


    /*
     * Get current page from controller.
     */
    page = MenuController_GetCurrentPage();



    if (page == 0)
    {
        return;
    }



    /*
     * Page title.
     */
    render_data.title = page->title;



    /*
     * Number of visible items.
     */
    render_data.item_count = page->item_count;



    /*
     * Copy item text pointers.
     */
    for (uint8_t i = 0U;
         i < MENU_RENDER_VISIBLE_ITEMS;
         i++)
    {

        if (i < page->item_count)
        {
            render_data.items[i] =
                    page->items[i].text;
        }
        else
        {
            render_data.items[i] = 0;
        }
    }



    /*
     * Current cursor position.
     *
     * Controller already guarantees
     * valid range.
     */
    render_data.cursor_position =
            MenuController_GetSelectedIndex();
}



/*
 * Return renderer data.
 */
const MenuRenderData_t *MenuRenderer_GetData(void)
{
    return &render_data;
}
