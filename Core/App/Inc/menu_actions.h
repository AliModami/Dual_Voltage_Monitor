/******************************************************************************
 * @file    menu_actions.h
 * @brief   Menu Action Layer Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file defines the public interface of menu action functions.
 *
 *      Menu actions are the bridge between the menu framework and application
 *      features.
 *
 *
 *      The menu framework should not directly know about:
 *
 *          - Monitor module
 *          - Stream module
 *          - Settings module
 *          - System information module
 *
 *
 *      Instead:
 *
 *
 *              Menu Item
 *                  |
 *                  v
 *              Menu Action
 *                  |
 *                  v
 *          Application Module
 *
 *
 *
 *      Example:
 *
 *
 *          "Live Monitor"
 *
 *                  |
 *                  v
 *
 *          MenuAction_OpenMonitor()
 *
 *                  |
 *                  v
 *
 *          Monitor_Start()
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
 *
 *
 *                    menu_data.c
 *                         |
 *                         v
 *                  menu_actions.h
 *                         |
 *                         v
 *                  menu_actions.c
 *                         |
 *             +-----------+-----------+
 *             |           |           |
 *             v           v           v
 *
 *          monitor     stream     settings
 *
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Keep menu framework independent from application modules.
 *
 *      2. Allow adding new menu items without modifying engine.
 *
 *      3. Centralize navigation related actions.
 *
 *      4. Make application modules reusable outside menu system.
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
 *          Initial architecture version.
 *
 ******************************************************************************/

#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H



/******************************************************************************
 *                         Public Action Functions
 ******************************************************************************/

/**
 * @brief
 *      Open live voltage monitor screen.
 *
 * @details
 *      Called when user selects:
 *
 *          Main Menu
 *              |
 *              +-- Live Monitor
 *
 */
void MenuAction_OpenMonitor(void);



/**
 * @brief
 *      Start UART streaming screen.
 *
 * @details
 *
 *      Called when user selects:
 *
 *          Main Menu
 *              |
 *              +-- Start Stream
 *
 */
void MenuAction_StartStream(void);



/**
 * @brief
 *      Open settings menu.
 *
 * @details
 *
 *      This action changes application state
 *      to settings mode.
 *
 */
void MenuAction_OpenSettings(void);



/**
 * @brief
 *      Open system information screen.
 *
 * @details
 *
 *      Displays:
 *
 *          - Firmware version
 *          - MCU information
 *          - Configuration status
 *
 */
void MenuAction_OpenSystemInfo(void);



/**
 * @brief
 *      Open sample time configuration.
 *
 * @details
 *
 *      Allows user to configure ADC sampling
 *      interval.
 *
 */
void MenuAction_OpenSampleTime(void);



/**
 * @brief
 *      Open UART baud rate configuration.
 *
 * @details
 *
 *      Allows changing communication speed.
 *
 */
void MenuAction_OpenBaudRate(void);



#endif /* MENU_ACTIONS_H */
