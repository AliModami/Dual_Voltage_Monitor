/******************************************************************************
 * @file    button_app.h
 * @brief   Button Application Layer Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This module provides the application layer interface between
 *      the button driver and higher level modules such as menu system.
 *
 *      The button driver generates hardware independent events.
 *      This module receives those events and translates them into
 *      application commands.
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

#ifndef BUTTON_APP_H
#define BUTTON_APP_H


#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdbool.h>

#include "buttons.h"



/******************************************************************************
 *                              Types
 ******************************************************************************/

/**
 * @brief
 *      Application level button commands.
 *
 *      These commands are independent from hardware events.
 */

typedef enum
{
    BUTTON_CMD_NONE = 0,

    BUTTON_CMD_UP,

    BUTTON_CMD_DOWN,

    BUTTON_CMD_ENTER,

    BUTTON_CMD_BACK


} Button_AppCommand_t;



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize button application layer.
 */
void ButtonApp_Init(void);



/**
 * @brief
 *      Process button events.
 *
 * @details
 *      This function reads events from button driver
 *      and converts them into application commands.
 */
void ButtonApp_Task(void);



/**
 * @brief
 *      Get latest application button command.
 *
 * @param command
 *      Destination command variable.
 *
 * @return
 *
 *      true:
 *          New command available.
 *
 *      false:
 *          No command available.
 */
bool ButtonApp_GetCommand(Button_AppCommand_t *command);



#ifdef __cplusplus
}
#endif


#endif /* BUTTON_APP_H */
