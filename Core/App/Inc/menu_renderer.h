/******************************************************************************
 * @file    menu_renderer.h
 * @brief   Menu Renderer Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file defines the public interface of the menu renderer layer.
 *
 *
 *      The renderer is responsible for converting the current menu state
 *      into LCD output.
 *
 *
 *      Responsibilities:
 *
 *          - Read current menu state.
 *          - Prepare display content.
 *          - Draw menu title.
 *          - Draw visible menu items.
 *          - Show selected item indicator.
 *
 *
 *      This module DOES NOT:
 *
 *          - Read buttons.
 *          - Control menu navigation.
 *          - Change menu state.
 *          - Access GPIO.
 *          - Access ADC.
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
 *
 *
 *              Button
 *                |
 *                v
 *
 *          menu_engine.c
 *
 *                |
 *                v
 *
 *        menu_renderer.c
 *
 *                |
 *                v
 *
 *             lcd_i2c.c
 *
 *
 *-----------------------------------------------------------------------------
 * Rendering Flow:
 *
 *
 *          Menu state changed
 *
 *                  |
 *                  v
 *
 *          MenuRenderer_Update()
 *
 *                  |
 *                  v
 *
 *          LCD_Clear()
 *
 *                  |
 *                  v
 *
 *          Draw title
 *
 *                  |
 *                  v
 *
 *          Draw visible items
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep LCD hardware independent from menu logic.
 *
 *      2. Allow replacing LCD without changing menu engine.
 *
 *      3. Support future displays:
 *
 *              - Character LCD
 *              - TFT LCD
 *              - OLED
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



#ifndef MENU_RENDERER_H
#define MENU_RENDERER_H



#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>



#include "menu.h"

#include "menu_engine.h"

#include "lcd_i2c.h"





/******************************************************************************
 *                         Configuration
 ******************************************************************************/

/*
 * LCD geometry.
 *
 * Current hardware:
 *
 *      LCD 20x4
 *
 */

#define MENU_RENDERER_LCD_ROWS          (4U)

#define MENU_RENDERER_LCD_COLUMNS       (20U)



/*
 * Selected item marker.
 *
 * Example:
 *
 *      > Live Monitor
 *
 */

#define MENU_SELECTED_MARKER            '>'

#define MENU_NORMAL_MARKER              ' '





/******************************************************************************
 *                         Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu renderer.
 *
 * @details
 *
 *      Clears internal renderer state.
 *
 */
void MenuRenderer_Init(void);





/******************************************************************************
 *                         Rendering Functions
 ******************************************************************************/

/**
 * @brief
 *      Update LCD menu display.
 *
 * @details
 *
 *      This function:
 *
 *          1. Reads current menu state.
 *
 *          2. Clears LCD if required.
 *
 *          3. Draws title.
 *
 *          4. Draws visible menu items.
 *
 */
void MenuRenderer_Update(void);





/**
 * @brief
 *      Force complete redraw.
 *
 * @details
 *
 *      Used after:
 *
 *          - Changing application screen.
 *          - Returning from submenu.
 *
 */
void MenuRenderer_ForceRefresh(void);





/**
 * @brief
 *      Draw menu title.
 *
 * @param title
 *
 *      Title string.
 *
 */
void MenuRenderer_DrawTitle(
        const char *title);





/**
 * @brief
 *      Draw menu items.
 *
 * @details
 *
 *      Draws only visible items
 *      according to current scroll position.
 *
 */
void MenuRenderer_DrawItems(void);





/**
 * @brief
 *      Clear renderer refresh flag.
 *
 */
void MenuRenderer_ClearRefresh(void);





/**
 * @brief
 *      Check renderer refresh request.
 *
 * @return
 *
 *      true:
 *          Refresh required.
 *
 *      false:
 *          No refresh required.
 *
 */
bool MenuRenderer_IsRefreshRequired(void);





#ifdef __cplusplus
}
#endif



#endif /* MENU_RENDERER_H */
