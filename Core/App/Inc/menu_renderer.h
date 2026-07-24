/******************************************************************************
 *
 * @file    menu_renderer.h
 *
 * @brief   Page Based Menu Renderer Interface
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
 *      This file defines the menu rendering abstraction layer.
 *
 *      Responsibilities:
 *
 *      - Read menu state from menu_controller.
 *      - Prepare visible page information.
 *      - Provide cursor position.
 *      - Provide item text for display layer.
 *
 *      This module does NOT handle:
 *
 *      - LCD hardware
 *      - I2C communication
 *      - Button input
 *
 ******************************************************************************/

#ifndef MENU_RENDERER_H
#define MENU_RENDERER_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "menu_items.h"



/*
 * Number of lines available for menu items.
 *
 * LCD 20x4:
 *
 * Line 0 : Page title
 * Line 1-3 : Menu items
 */
#define MENU_RENDER_VISIBLE_ITEMS     3U




/*
 * Renderer display mode.
 *
 * NORMAL:
 *      Standard page based menu display.
 *
 * EDIT:
 *      Parameter editing display.
 */
typedef enum
{
    MENU_RENDER_MODE_MENU = 0,

    MENU_RENDER_MODE_EDIT

} MenuRenderMode_t;




/*
 * Renderer output structure.
 *
 * This structure contains all information
 * required by LCD layer.
 */
typedef struct
{

    /*
     * Page title.
     */
    const char *title;


    /*
     * Current renderer mode.
     */
    MenuRenderMode_t mode;


    /*
     * Edit screen title.
     */
    const char *edit_title;


    /*
     * Current editing value.
     */
    int32_t edit_value;


    /*
     * Visible menu item strings.
     *
     * Maximum:
     *
     * MENU_RENDER_VISIBLE_ITEMS
     */
    const char *items[MENU_RENDER_VISIBLE_ITEMS];



    /*
     * Current cursor position.
     *
     * Range:
     *
     * 0 ... 2
     */
    uint8_t cursor_position;



    /*
     * Number of valid visible items.
     *
     * Used for pages containing less than
     * three items.
     */
    uint8_t item_count;


} MenuRenderData_t;



/*
 * Initialize renderer.
 *
 * Must be called once before rendering.
 */
void MenuRenderer_Init(void);



/*
 * Update renderer data.
 *
 * Reads current information from:
 *
 *      menu_controller
 *
 * and prepares display data.
 *
 */
void MenuRenderer_Update(void);



/*
 * Get prepared rendering information.
 *
 * LCD driver uses this function.
 */
const MenuRenderData_t *MenuRenderer_GetData(void);



#ifdef __cplusplus
}
#endif


#endif /* MENU_RENDERER_H */
