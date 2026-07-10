/******************************************************************************
 * @file    menu_integration.h
 * @brief   Menu Framework Integration Layer Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file defines the public interface of the menu integration layer.
 *
 *      The integration layer provides a clean connection between:
 *
 *          Application Main Loop
 *                  |
 *                  v
 *          Menu Framework
 *
 *
 *      The purpose of this layer is preventing main.c from knowing internal
 *      details of the menu architecture.
 *
 *
 *      Instead of:
 *
 *          main.c
 *             |
 *             +--> MenuEngine_Init()
 *             |
 *             +--> MenuRenderer_Init()
 *             |
 *             +--> MenuController_Process()
 *
 *
 *      We use:
 *
 *          main.c
 *             |
 *             v
 *       MenuIntegration_Init()
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
 *
 *
 *                  main.c
 *                     |
 *                     v
 *
 *             menu_integration.c
 *
 *                     |
 *        +------------+------------+
 *        |            |            |
 *        v            v            v
 *
 *  menu_engine   menu_controller   menu_renderer
 *
 *
 *
 *-----------------------------------------------------------------------------
 * Responsibilities:
 *
 *      This module handles:
 *
 *          - Menu startup sequence.
 *          - Periodic menu processing.
 *          - Communication between menu layers.
 *
 *
 *      This module DOES NOT:
 *
 *          - Read buttons directly.
 *          - Draw LCD directly.
 *          - Store menu items.
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep main.c simple and clean.
 *
 *      2. Provide a reusable menu package.
 *
 *      3. Make future RTOS migration easier.
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

#ifndef MENU_INTEGRATION_H
#define MENU_INTEGRATION_H



/******************************************************************************
 *                         Initialization Interface
 ******************************************************************************/

/**
 * @brief
 *      Initialize complete menu subsystem.
 *
 * @details
 *
 *      This function initializes:
 *
 *          - Menu engine
 *          - Menu controller
 *          - Menu renderer
 *
 *
 *      Must be called once after LCD initialization.
 *
 */
void MenuIntegration_Init(void);



/******************************************************************************
 *                         Runtime Processing
 ******************************************************************************/

/**
 * @brief
 *      Execute menu processing cycle.
 *
 * @details
 *
 *      This function should be called periodically
 *      inside the main application loop.
 *
 *
 *      Example:
 *
 *          while(1)
 *          {
 *              MenuIntegration_Process();
 *          }
 *
 */
void MenuIntegration_Process(void);



/**
 * @brief
 *      Force complete menu redraw.
 *
 * @details
 *
 *      Used after:
 *
 *          - Returning from another screen.
 *          - LCD recovery.
 *          - Display reset.
 *
 */
void MenuIntegration_Refresh(void);



#endif /* MENU_INTEGRATION_H */
