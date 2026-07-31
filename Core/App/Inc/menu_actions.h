/******************************************************************************
 *
 * @file    menu_actions.h
 *
 * @brief   Menu Action Interface
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file defines the application action interface
 *      used by the menu system.
 *
 *
 *      Design rules:
 *
 *      - Menu database does not contain application logic.
 *      - Menu controller does not know application details.
 *      - Actions are isolated in this layer.
 *
 *
 *      Dependency direction:
 *
 *
 *          menu_items.c
 *                |
 *                v
 *          menu_actions.h
 *                |
 *                v
 *          menu_actions.c
 *
 *
 *      This module does NOT handle:
 *
 *      - LCD rendering
 *      - Button scanning
 *      - Menu navigation
 *      - Configuration storage directly
 *
 ******************************************************************************/

#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H


#ifdef __cplusplus
extern "C"
{
#endif


/*
 * ============================================================================
 * General Menu Actions
 * ============================================================================
 */


/*
 * Open live monitor screen.
 */
void MenuAction_LiveMonitor(void);



/*
 * Start UART data streaming.
 */
void MenuAction_StartStream(void);




/*
 * ============================================================================
 * Stream Settings Actions
 * ============================================================================
 */


/*
 * Edit UART baud rate.
 */
void MenuAction_BaudRate(void);



/*
 * Edit stream sample interval.
 */
void MenuAction_SampleRate(void);




/*
 * ============================================================================
 * Alarm Settings Actions
 * ============================================================================
 */


/*
 * Edit alarm enable state.
 */
void MenuAction_AlarmEnable(void);



/*
 * Edit low voltage threshold.
 */
void MenuAction_LowVoltageLimit(void);



/*
 * Edit high voltage threshold.
 */
void MenuAction_HighVoltageLimit(void);



/*
 * Edit alarm operation mode.
 */
void MenuAction_AlarmMode(void);




/*
 * ============================================================================
 * Calibration Actions
 * ============================================================================
 */


/*
 * Edit input voltage calibration offset.
 */
void MenuAction_InputVoltageOffset(void);



/*
 * Edit output voltage calibration offset.
 */
void MenuAction_OutputVoltageOffset(void);




/*
 * ============================================================================
 * Service Mode Actions
 * ============================================================================
 */



/*
 * Run buzzer diagnostic.
 */
void MenuAction_BuzzerTest(void);



/*
 * Run LCD diagnostic.
 */
void MenuAction_LCDTest(void);


void MenuAction_AutoCalibrate(void);


void MenuAction_FactoryDefault(void);



#ifdef __cplusplus
}
#endif


#endif /* MENU_ACTIONS_H */


/******************************************************************************
 *
 *                              END OF FILE
 *
 ******************************************************************************/
