/******************************************************************************
 * @file    menu_integration.c
 * @brief   Menu Framework Integration Layer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file implements the integration layer between the application
 *      main loop and the menu framework.
 *
 *
 *      The purpose of this layer:
 *
 *          - Hide internal menu modules from main.c.
 *          - Provide a single entry point for menu startup.
 *          - Provide a single periodic processing function.
 *
 *
 *      Application flow:
 *
 *
 *              main.c
 *                 |
 *                 v
 *
 *        MenuIntegration_Process()
 *
 *                 |
 *        +--------+---------+
 *        |                  |
 *        v                  v
 *
 *   menu_controller     menu_renderer
 *
 *                 |
 *                 v
 *
 *            menu_engine
 *
 *
 *
 *-----------------------------------------------------------------------------
 * Startup Sequence:
 *
 *
 *      LCD_Init()
 *
 *          |
 *          v
 *
 *      MenuIntegration_Init()
 *
 *          |
 *          +--> MenuEngine_Init()
 *          |
 *          +--> MenuController_Init()
 *          |
 *          +--> MenuRenderer_Init()
 *
 *
 *-----------------------------------------------------------------------------
 * Runtime:
 *
 *
 *      while(1)
 *      {
 *
 *          MenuIntegration_Process();
 *
 *      }
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep main.c clean.
 *
 *      2. Allow menu framework reuse in other projects.
 *
 *      3. Make future RTOS task migration easier.
 *
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami
 *
 * Version :
 *      1.0.0
 *
 ******************************************************************************/

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_integration.h"



#include "menu_engine.h"

#include "menu_controller.h"

#include "menu_renderer.h"



/******************************************************************************
 *                              Private Variables
 ******************************************************************************/

/*
 * Integration initialization state.
 *
 * Prevents processing before startup.
 */
static bool menu_initialized = false;



/******************************************************************************
 *                         Initialization Function
 ******************************************************************************/

void MenuIntegration_Init(void)
{
    /*
     * Initialize menu navigation engine.
     *
     * This loads:
     *
     *      - Root menu
     *      - Current selection
     *
     */
    MenuEngine_Init();



    /*
     * Initialize input controller.
     *
     * Clears pending button events.
     */
    MenuController_Init();



    /*
     * Initialize LCD renderer.
     *
     * Clears renderer internal cache.
     */
    MenuRenderer_Init();



    /*
     * Mark subsystem ready.
     */
    menu_initialized = true;



    /*
     * Draw initial menu.
     */
    MenuRenderer_DrawFull();
}


/******************************************************************************
 *                         Runtime Processing
 ******************************************************************************/

void MenuIntegration_Process(void)
{
    /*
     * Safety check.
     *
     * The menu system must be initialized
     * before processing events.
     */
    if(menu_initialized == false)
    {
        return;
    }



    /*
     * Process pending user input.
     *
     * Flow:
     *
     *      Button Event
     *             |
     *             v
     *
     *      menu_controller
     *             |
     *             v
     *
     *      menu_engine
     *
     */
    MenuController_Process();



    /*
     * Update LCD if required.
     *
     * Renderer internally decides whether
     * a real LCD transfer is necessary.
     */
    MenuRenderer_Refresh();
}





/******************************************************************************
 *                         Manual Refresh
 ******************************************************************************/

void MenuIntegration_Refresh(void)
{
    /*
     * Force complete redraw.
     *
     * Used after:
     *
     *      - LCD reset
     *      - Returning from another screen
     *      - Recovering display
     *
     */
    if(menu_initialized == false)
    {
        return;
    }



    MenuRenderer_DrawFull();
}
