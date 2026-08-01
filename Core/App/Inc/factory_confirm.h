/******************************************************************************
 *
 * @file    factory_confirm.h
 *
 * @brief   Factory Default Confirmation Service Interface
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
 *      This module provides user confirmation handling
 *      before executing Factory Default operation.
 *
 *
 *      Responsibilities:
 *
 *          - Display confirmation screen.
 *          - Wait for user decision.
 *          - Handle ENTER as confirmation.
 *          - Handle BACK as cancellation.
 *          - Provide non-blocking task execution.
 *
 *
 *      This module does NOT handle:
 *
 *          - Factory reset execution.
 *          - Configuration storage.
 *          - Flash access.
 *          - Menu navigation.
 *          - LCD low level driver.
 *
 ******************************************************************************/


#ifndef FACTORY_CONFIRM_H
#define FACTORY_CONFIRM_H


#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 *****************************************************************************/

#include <stdint.h>


#include "buttons.h"



/******************************************************************************
 *                              Type Definitions
 *****************************************************************************/


/*
 * Factory confirmation state machine.
 */
typedef enum
{

    /*
     * No confirmation request active.
     */
    FACTORY_CONFIRM_IDLE = 0U,



    /*
     * Waiting for user decision.
     */
    FACTORY_CONFIRM_ACTIVE,



    /*
     * User accepted operation.
     */
    FACTORY_CONFIRM_ACCEPTED,



    /*
     * User cancelled operation.
     */
    FACTORY_CONFIRM_CANCELLED


} FactoryConfirmState_t;






/******************************************************************************
 *                              Public Functions
 *****************************************************************************/
void FactoryConfirm_ProcessButton(
    const Button_Event_t *event);




void FactoryConfirm_Reset(void);


/*
 * Initialize Factory Confirmation module.
 */
void FactoryConfirm_Init(void);






/*
 * Start confirmation process.
 *
 * Displays:
 *
 *      Factory Default?
 *
 *      ENTER = YES
 *      BACK  = NO
 *
 */
void FactoryConfirm_Start(void);






/*
 * Periodic confirmation task.
 *
 * Must be called continuously
 * from main loop.
 *
 * Non-blocking.
 */
void FactoryConfirm_Task(void);






/*
 * Get current confirmation state.
 */
FactoryConfirmState_t FactoryConfirm_GetState(void);






/*
 * Check whether confirmation screen
 * is currently active.
 *
 * Return:
 *
 *      1 = Active
 *      0 = Inactive
 *
 */
uint8_t FactoryConfirm_IsActive(void);






#ifdef __cplusplus
}
#endif


#endif /* FACTORY_CONFIRM_H */



/******************************************************************************
 *
 *                              END OF FILE
 *
 *      factory_confirm.h
 *
 *      Version:
 *
 *          Clean Final v1.0.0
 *
 *
 * Architecture:
 *
 *
 *      menu_actions.c
 *              |
 *              v
 *
 *      factory_confirm.c
 *              |
 *              v
 *
 *      main.c
 *              |
 *              v
 *
 *      factory_default.c
 *
 *
 *
 * Compatible:
 *
 *      STM32F103C8T6
 *      STM32 HAL
 *
 ******************************************************************************/
