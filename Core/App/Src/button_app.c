/******************************************************************************
 * @file    button_app.c
 * @brief   Button Application Layer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This module connects the low level button driver with the
 *      application layer.
 *
 *      The button driver generates hardware independent events.
 *      This module converts those events into application commands.
 *
 *      Flow:
 *
 *          buttons.c
 *              |
 *              v
 *          Button Events
 *              |
 *              v
 *          button_app.c
 *              |
 *              v
 *          Application Commands
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 * Created :
 *      2026-07-08
 ******************************************************************************/


/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "button_app.h"
#include <stddef.h>



/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * Latest generated application command.
 *
 * This variable is private to this module.
 */

static Button_AppCommand_t current_command =
        BUTTON_CMD_NONE;



/*
 * Temporary event storage.
 *
 * Button driver events are received here before
 * being converted into application commands.
 */

static Button_Event_t button_event;



/******************************************************************************
 *                         Private Functions
 ******************************************************************************/

/**
 * @brief
 *      Convert driver event into application command.
 *
 * @param event
 *      Button driver event.
 *
 * @return
 *      Application command.
 */
static Button_AppCommand_t ButtonApp_ConvertEvent(
        Button_Event_t event)
{
    /*
     * Only press events generate commands.
     *
     * Release, Long Press and Repeat handling
     * will be expanded when menu control is added.
     */

    if(event.event != BUTTON_EVENT_PRESS)
    {
        return BUTTON_CMD_NONE;
    }



    switch(event.button)
    {

        case BUTTON_ID_UP:

            return BUTTON_CMD_UP;



        case BUTTON_ID_DOWN:

            return BUTTON_CMD_DOWN;



        case BUTTON_ID_ENTER:

            return BUTTON_CMD_ENTER;



        case BUTTON_ID_BACK:

            return BUTTON_CMD_BACK;



        default:

            return BUTTON_CMD_NONE;
    }
}



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/


/**
 * @brief
 *      Initialize button application layer.
 */
void ButtonApp_Init(void)
{
    current_command = BUTTON_CMD_NONE;
}



/**
 * @brief
 *      Process button events.
 *
 * @details
 *      This function must be called periodically
 *      from the main loop.
 */
void ButtonApp_Task(void)
{
    Button_AppCommand_t command;



    /*
     * Read available driver events.
     */

    while(Buttons_GetEvent(&button_event))
    {

        command = ButtonApp_ConvertEvent(button_event);



        if(command != BUTTON_CMD_NONE)
        {
            current_command = command;
        }
    }
}



/**
 * @brief
 *      Get latest application command.
 *
 * @param command
 *      Destination variable.
 *
 * @return
 *
 *      true:
 *          Command available.
 *
 *      false:
 *          No command available.
 */
bool ButtonApp_GetCommand(Button_AppCommand_t *command)
{
    if(command == NULL)
    {
        return false;
    }



    if(current_command == BUTTON_CMD_NONE)
    {
        return false;
    }



    *command = current_command;



    /*
     * Clear command after reading.
     */

    current_command = BUTTON_CMD_NONE;



    return true;
}
