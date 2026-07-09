/******************************************************************************
 * @file    menu_renderer.h
 * @brief   LCD Menu Renderer Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module is responsible only for drawing the menu
 *      on the LCD.
 *
 *      It does NOT contain:
 *
 *          - Button handling
 *          - Menu navigation
 *          - Application logic
 *
 *      Those responsibilities belong to:
 *
 *          button_app.c
 *          menu.c
 *
 *      This module only converts menu information into
 *      LCD output.
 *
 *-----------------------------------------------------------------------------
 * Architecture
 *
 *          Button Driver
 *                 │
 *                 ▼
 *          Button Application
 *                 │
 *                 ▼
 *              Menu Logic
 *                 │
 *                 ▼
 *           Menu Renderer
 *                 │
 *                 ▼
 *             LCD Driver
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
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



/******************************************************************************
 *                      Public Function Prototypes
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu renderer.
 *
 * @details
 *      This function prepares the renderer.
 *
 *      Currently no hardware initialization is performed
 *      because LCD Driver is initialized separately.
 */
void MenuRenderer_Init(void);



/**
 * @brief
 *      Refresh LCD menu.
 *
 * @details
 *      Reads the current menu state from menu.c
 *      and redraws the LCD when required.
 */
void MenuRenderer_Update(void);



/**
 * @brief
 *      Force complete LCD redraw.
 *
 * @details
 *      Future versions may use this function after
 *      changing application screens.
 */
void MenuRenderer_Refresh(void);



#ifdef __cplusplus
}
#endif

#endif /* MENU_RENDERER_H */
