/******************************************************************************
 * @file    menu_actions.c
 * @brief   Menu Action Layer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file implements actions executed from the menu system.
 *
 *      Menu actions are the connection point between:
 *
 *          Menu Framework
 *                 |
 *                 v
 *          Application Modules
 *
 *
 *      The purpose of this layer is preventing the menu system from directly
 *      depending on application modules.
 *
 *
 *      Example:
 *
 *
 *          User selects:
 *
 *              Main Menu
 *                  |
 *                  +-- Live Monitor
 *
 *
 *          Flow:
 *
 *              menu_data.c
 *                    |
 *                    v
 *              MenuAction_OpenMonitor()
 *                    |
 *                    v
 *              Monitor module
 *
 *
 *
 *      This module DOES NOT:
 *
 *          - Draw LCD screens.
 *          - Read buttons.
 *          - Perform ADC calculations.
 *          - Manage UART communication.
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
 *
 *
 *              menu_engine.c
 *                    |
 *                    v
 *
 *              menu_actions.c
 *
 *                    |
 *        +-----------+-----------+
 *        |           |           |
 *        v           v           v
 *
 *    monitor.c    stream.c   settings.c
 *
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep Menu Framework independent from application logic.
 *
 *      2. Provide a single place for menu-triggered transitions.
 *
 *      3. Make future application changes easier.
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

#include "menu_actions.h"

#include "menu_engine.h"



/*
 * Application modules.
 *
 * These headers will connect the menu
 * with real project functionality.
 *
 * If a module does not exist yet,
 * a temporary implementation can be used.
 */

#include "monitor.h"

#include "stream.h"

#include "settings.h"



/******************************************************************************
 *                              Private Defines
 ******************************************************************************/

/*
 * This function is used to change application mode
 * after selecting a menu item.
 *
 * The actual screen handler can use this mode
 * inside the main application loop.
 */



/******************************************************************************
 *                         Open Monitor Action
 ******************************************************************************/

void MenuAction_OpenMonitor(void)
{
    /*
     * Change application state.
     *
     * The menu system only requests the mode change.
     * The monitor module is responsible for displaying
     * voltage information.
     */
    MenuEngine_SetMode(
            MONITOR_MODE);



    /*
     * Initialize monitor screen.
     */
    Monitor_Init();
}



/******************************************************************************
 *                         Start Stream Action
 ******************************************************************************/

void MenuAction_StartStream(void)
{
    /*
     * Enter streaming mode.
     */
    MenuEngine_SetMode(
            STREAM_MODE);



    /*
     * Initialize UART streaming service.
     */
    Stream_Init();
}


/******************************************************************************
 *                         Settings Action
 ******************************************************************************/

void MenuAction_OpenSettings(void)
{
    /*
     * Enter settings mode.
     *
     * Settings module manages:
     *
     *      - Voltage limits
     *      - Switching delay
     *      - User configuration
     *
     */
    MenuEngine_SetMode(
            SETTINGS_MODE);



    /*
     * Settings initialization.
     */
    Settings_Init();
}



/******************************************************************************
 *                         System Information Action
 ******************************************************************************/

void MenuAction_OpenSystemInfo(void)
{
    /*
     * Change application state to
     * system information screen.
     *
     * The screen handler will display:
     *
     *      - Firmware version
     *      - MCU information
     *      - Configuration status
     *
     */
    MenuEngine_SetMode(
            SYSTEM_INFO_MODE);
}



/******************************************************************************
 *                         Sample Time Action
 ******************************************************************************/

void MenuAction_OpenSampleTime(void)
{
    /*
     * Open sample time configuration.
     *
     * User can configure ADC sampling interval.
     */
    MenuEngine_SetMode(
            SAMPLE_TIME_MODE);
}



/******************************************************************************
 *                         Baud Rate Action
 ******************************************************************************/

void MenuAction_OpenBaudRate(void)
{
    /*
     * Open UART communication settings.
     */
    MenuEngine_SetMode(
            BAUD_RATE_MODE);
}
