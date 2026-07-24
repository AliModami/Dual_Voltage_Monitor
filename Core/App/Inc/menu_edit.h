/******************************************************************************
 *
 * @file    menu_edit.h
 *
 * @brief   Menu Edit Engine Public Interface
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
 *      This file defines the public interface of the
 *      menu parameter editing engine.
 *
 *      Supported editable parameters:
 *
 *      - Baud Rate
 *      - Sample Rate
 *      - Alarm Enable
 *      - Low Voltage Limit
 *      - High Voltage Limit
 *      - Alarm Mode
 *
 ******************************************************************************/

#ifndef MENU_EDIT_H
#define MENU_EDIT_H


#ifdef __cplusplus
extern "C"
{
#endif


#include <stdbool.h>
#include <stdint.h>





/*
 * ============================================================================
 * Edit Target Definition
 * ============================================================================
 */


/*
 * Defines the parameter currently
 * being edited.
 */
typedef enum
{

    EDIT_NONE = 0U,


    /*
     * UART communication speed.
     */
    EDIT_BAUD_RATE,


    /*
     * ADC sampling interval.
     */
    EDIT_SAMPLE_RATE,


    /*
     * Enable or disable alarm.
     */
    EDIT_ALARM_ENABLE,


    /*
     * Low voltage threshold.
     */
    EDIT_LOW_VOLTAGE_LIMIT,


    /*
     * High voltage threshold.
     */
    EDIT_HIGH_VOLTAGE_LIMIT,


    /*
     * Alarm operation mode.
     *
     * Once / Repeat
     */
    EDIT_ALARM_MODE


} MenuEditTarget_t;





/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */


/*
 * Initialize edit engine.
 */
void MenuEdit_Init(void);





/*
 * ============================================================================
 * Edit Control
 * ============================================================================
 */


/*
 * Start editing selected parameter.
 */
void MenuEdit_Start(MenuEditTarget_t target);





/*
 * Confirm current edited value.
 */
void MenuEdit_Confirm(void);





/*
 * Cancel current edit operation.
 */
void MenuEdit_Cancel(void);





/*
 * Exit edit mode.
 */
void MenuEdit_Exit(void);





/*
 * Enter edit mode.
 */
void MenuEdit_Enter(void);





/*
 * ============================================================================
 * Value Modification
 * ============================================================================
 */


/*
 * Increase current value.
 *
 * Examples:
 *
 * Baud Rate:
 *      9600 -> 19200
 *
 * Sample Rate:
 *      100ms -> 200ms
 *
 * Voltage:
 *      180V -> 185V
 */
void MenuEdit_Increment(void);





/*
 * Decrease current value.
 *
 * Examples:
 *
 * Baud Rate:
 *      19200 -> 9600
 *
 * Sample Rate:
 *      200ms -> 100ms
 *
 * Voltage:
 *      185V -> 180V
 */
void MenuEdit_Decrement(void);





/*
 * ============================================================================
 * Status Information
 * ============================================================================
 */


/*
 * Check whether edit mode is active.
 */
bool MenuEdit_IsActive(void);





/*
 * Get current edit target.
 */
MenuEditTarget_t MenuEdit_GetTarget(void);





/*
 * Get current edited value.
 */
int32_t MenuEdit_GetValue(void);





#ifdef __cplusplus
}
#endif


#endif /* MENU_EDIT_H */


/******************************************************************************
 *
 *                              END OF FILE
 *
 ******************************************************************************/
