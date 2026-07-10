/******************************************************************************
 * @file    menu_controller.c
 * @brief   Menu Input Controller Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module implements the menu control layer.
 *
 *
 *      The purpose of this layer is to provide a clean connection between:
 *
 *
 *              Button Application Layer
 *
 *                         |
 *                         v
 *
 *              Menu Controller
 *
 *                         |
 *                         v
 *
 *              Menu Navigation Engine
 *
 *
 *
 *      The controller receives logical button commands and converts them
 *      into menu navigation operations.
 *
 *
 *-----------------------------------------------------------------------------
 * Responsibilities:
 *
 *      This module:
 *
 *          - Receives application level button commands.
 *
 *          - Executes corresponding menu navigation requests.
 *
 *          - Keeps menu input handling separated from menu engine.
 *
 *          - Generates LCD refresh request after state changes.
 *
 *
 *      This module DOES NOT:
 *
 *          - Read GPIO.
 *
 *          - Debounce buttons.
 *
 *          - Access LCD hardware directly.
 *
 *          - Store menu items.
 *
 *          - Execute application features.
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
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
 *              |
 *              v
 *
 *             LCD
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep input and navigation separated.
 *
 *      2. Make menu engine independent from buttons.
 *
 *      3. Allow future input replacement:
 *
 *              - Rotary encoder
 *              - Touch interface
 *              - Remote control
 *
 *      4. Keep main.c simple.
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
 *          Removed duplicated adapter layer.
 *
 *          Integrated adapter responsibilities into controller.
 *
 ******************************************************************************/



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "menu_controller.h"


#include "menu_engine.h"

#include "menu_renderer.h"



/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * Last received button command.
 *
 * Purpose:
 *
 *      Used for:
 *
 *          - Debugging
 *          - Diagnostics
 *          - Future event logging
 *
 *
 * This variable does not control
 * menu operation.
 */

static Button_AppCommand_t last_command =
        BUTTON_CMD_NONE;





/*
 * Menu refresh request flag.
 *
 * When a valid command changes
 * menu state, this flag becomes true.
 *
 *
 * Renderer can use this information
 * to update LCD only when necessary.
 */

static bool refresh_required = false;





/******************************************************************************
 *                         Private Functions
 ******************************************************************************/

/**
 * @brief
 *      Execute menu command.
 *
 * @param command
 *
 *      Logical button command.
 *
 * @details
 *
 *      This function is the only place
 *      where button commands are mapped
 *      to menu operations.
 *
 *
 *      Keeping this mapping here prevents
 *      menu_engine from knowing anything
 *      about input devices.
 *
 */
static void MenuController_ExecuteCommand(
        Button_AppCommand_t command)
{

    switch(command)
    {

        case BUTTON_CMD_UP:

            /*
             * Move cursor to previous item.
             */
            MenuEngine_MovePrevious();

            break;



        case BUTTON_CMD_DOWN:

            /*
             * Move cursor to next item.
             */
            MenuEngine_MoveNext();

            break;



        case BUTTON_CMD_ENTER:

            /*
             * Enter selected item.
             *
             * The menu engine decides whether
             * the item opens a submenu or runs
             * an application action.
             */
            MenuEngine_Enter();

            break;



        case BUTTON_CMD_BACK:

            /*
             * Return to parent menu.
             */
            MenuEngine_Back();

            break;



        case BUTTON_CMD_NONE:

        default:

            /*
             * No valid command.
             */
            break;

    }

}





/**
 * @brief
 *      Notify renderer about possible display change.
 *
 * @details
 *
 *      Controller does not draw anything.
 *
 *      It only informs the display layer
 *      that menu state may have changed.
 *
 */
static void MenuController_RequestRefresh(void)
{

    refresh_required = true;


    MenuRenderer_ForceRefresh();

}

/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu controller.
 *
 * @details
 *
 *      Clears internal controller state.
 *
 *
 *      This function must be called once
 *      during system startup before processing
 *      any menu command.
 *
 */
void MenuController_Init(void)
{

    /*
     * No command has been received yet.
     */
    last_command =
            BUTTON_CMD_NONE;



    /*
     * No LCD refresh is required.
     */
    refresh_required = false;

}





/**
 * @brief
 *      Process one button command.
 *
 * @param command
 *
 *      Application level button command.
 *
 * @details
 *
 *      Processing flow:
 *
 *
 *          Button Driver
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
 *          menu_engine.c
 *
 *
 *
 *      The controller does not know how
 *      buttons are physically generated.
 *
 */
void MenuController_Process(
        Button_AppCommand_t command)
{

    /*
     * Store command for diagnostics.
     *
     * Even BUTTON_CMD_NONE is stored
     * because it can help debugging
     * the input chain.
     */
    last_command = command;



    /*
     * Ignore empty commands.
     *
     * Empty command means:
     *
     *      - No button pressed.
     *      - No action required.
     */
    if(command == BUTTON_CMD_NONE)
    {
        return;
    }



    /*
     * Convert command into
     * menu navigation operation.
     */
    MenuController_ExecuteCommand(command);



    /*
     * A valid command was processed.
     *
     * Request display update.
     */
    MenuController_RequestRefresh();

}





/**
 * @brief
 *      Check refresh request status.
 *
 * @return
 *
 *      true:
 *          LCD update is required.
 *
 *      false:
 *          No update required.
 *
 * @details
 *
 *      The controller does not update LCD.
 *
 *      It only provides information
 *      for upper layers.
 *
 */
bool MenuController_IsRefreshRequired(void)
{

    return refresh_required;

}





/**
 * @brief
 *      Clear refresh request.
 *
 * @details
 *
 *      Called after renderer finishes
 *      updating the display.
 *
 */
void MenuController_ClearRefreshRequest(void)
{

    refresh_required = false;

}





/**
 * @brief
 *      Get last processed command.
 *
 * @return
 *
 *      Last received application command.
 *
 * @details
 *
 *      Useful for:
 *
 *          - Debugging
 *          - Testing
 *          - Diagnostic screens
 *
 */
Button_AppCommand_t MenuController_GetLastCommand(void)
{

    return last_command;

}





/******************************************************************************
 *                              End Of File
 ******************************************************************************/

/*
 * File continuation:
 *
 *      No more functions remain.
 *
 *      This completes the final menu_controller.c.
 *
 *
 * Next file:
 *
 *      menu_controller.h
 *
 *
 * After verifying build:
 *
 * Delete:
 *
 *      menu_controller_adapter.c
 *      menu_controller_adapter.h
 *
 *
 * Keep:
 *
 *      menu_controller.c
 *      menu_controller.h
 *
 */
