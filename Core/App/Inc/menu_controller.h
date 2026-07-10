/******************************************************************************
 * @file    menu_controller.h
 * @brief   Menu Controller Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file defines the public interface of the menu controller layer.
 *
 *
 *      The menu controller is responsible for connecting the application
 *      button layer with the menu navigation engine.
 *
 *
 *      Data flow:
 *
 *
 *          buttons.c
 *
 *              |
 *              v
 *
 *          button_app.c
 *
 *              |
 *              v
 *
 *       menu_controller.c
 *
 *              |
 *              v
 *
 *          menu_engine.c
 *
 *              |
 *              v
 *
 *       menu_renderer.c
 *
 *
 *-----------------------------------------------------------------------------
 * Responsibilities:
 *
 *      This module:
 *
 *          - Receives logical button commands.
 *
 *          - Converts commands into menu operations.
 *
 *          - Requests display refresh after menu changes.
 *
 *          - Keeps input handling separated from navigation engine.
 *
 *
 *      This module DOES NOT:
 *
 *          - Read GPIO.
 *
 *          - Debounce buttons.
 *
 *          - Access LCD hardware.
 *
 *          - Store menu structure.
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep menu_engine independent from input hardware.
 *
 *      2. Provide one clear entry point for menu control.
 *
 *      3. Allow future input replacement:
 *
 *              - Rotary encoder
 *              - Touch interface
 *              - Remote control
 *
 *      4. Keep main.c clean.
 *
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami
 *
 * Version :
 *      2.0.0
 *
 * Change History :
 *
 *      2.0.0
 *
 *          Removed duplicated menu_controller_adapter layer.
 *
 *          Integrated adapter responsibility into controller.
 *
 ******************************************************************************/



#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H



#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>

#include <stdbool.h>


#include "button_app.h"





/******************************************************************************
 *                         Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu controller.
 *
 * @details
 *
 *      Clears internal controller state.
 *
 *      Must be called once during system
 *      initialization.
 *
 */
void MenuController_Init(void);





/******************************************************************************
 *                         Command Processing
 ******************************************************************************/

/**
 * @brief
 *      Process one button command.
 *
 * @param command
 *
 *      Application level button command.
 *
 *
 * @details
 *
 *      This function receives commands from
 *      button application layer and forwards
 *      them to menu engine.
 *
 *
 *      Example:
 *
 *
 *          User presses DOWN
 *
 *                 |
 *                 v
 *
 *          button_app.c
 *
 *                 |
 *                 v
 *
 *          MenuController_Process()
 *
 *                 |
 *                 v
 *
 *          MenuEngine_MoveNext()
 *
 */
void MenuController_Process(
        Button_AppCommand_t command);





/******************************************************************************
 *                         Refresh Management
 ******************************************************************************/

/**
 * @brief
 *      Check display refresh requirement.
 *
 * @return
 *
 *      true:
 *          Renderer should update LCD.
 *
 *      false:
 *          No update required.
 *
 */
bool MenuController_IsRefreshRequired(void);





/**
 * @brief
 *      Clear refresh request flag.
 *
 * @details
 *
 *      Called after successful LCD refresh.
 *
 */
void MenuController_ClearRefreshRequest(void);





/******************************************************************************
 *                         Diagnostic Functions
 ******************************************************************************/

/**
 * @brief
 *      Get last processed command.
 *
 * @return
 *
 *      Last received button command.
 *
 * @details
 *
 *      Useful for:
 *
 *          - Debugging
 *          - Testing
 *          - Diagnostic screen
 *
 */
Button_AppCommand_t MenuController_GetLastCommand(void);





#ifdef __cplusplus
}
#endif



#endif /* MENU_CONTROLLER_H */
