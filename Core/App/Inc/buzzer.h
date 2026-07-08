/******************************************************************************
 * @file    buzzer.h
 * @brief   Buzzer control interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file contains the public interface of the buzzer module.
 *
 *      The buzzer module is responsible for generating different notification
 *      patterns used by the application.
 *
 *      The module is designed independently from the application logic.
 *      Other modules should only use the functions declared in this file and
 *      should not access buzzer hardware directly.
 *
 *      Supported buzzer patterns:
 *
 *          - BUZZER_OFF
 *          - BUZZER_SHORT_BEEP
 *          - BUZZER_DOUBLE_BEEP
 *          - BUZZER_LONG_BEEP
 *          - BUZZER_ERROR
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      1. Hardware access remains inside buzzer.c.
 *      2. Application modules communicate only through this interface.
 *      3. No global variables are declared in this header file.
 *      4. Timing and state management remain private inside buzzer.c.
 *      5. The buzzer module is non-blocking and uses periodic task execution.
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.1.0
 *
 * Created :
 *      2026-07-08
 ******************************************************************************/


#ifndef BUZZER_H
#define BUZZER_H


#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "app_types.h"



/******************************************************************************
 *                              Type Definitions
 ******************************************************************************/

/**
 * @brief
 *      Available buzzer operating patterns.
 *
 * @details
 *      The application selects a pattern instead of controlling the buzzer
 *      hardware directly.
 *
 *      This separation keeps the software modular and allows the buzzer
 *      implementation to change without modifying other application modules.
 */
typedef enum
{
    /*
     * Disable buzzer output.
     */
    BUZZER_OFF = 0,


    /*
     * One short notification beep.
     *
     * Typical usage:
     *      Button confirmation
     *      Simple user feedback
     */
    BUZZER_SHORT_BEEP,


    /*
     * Two short notification beeps.
     *
     * Typical usage:
     *      Different user notification
     */
    BUZZER_DOUBLE_BEEP,


    /*
     * One long notification beep.
     *
     * Typical usage:
     *      Important user information
     */
    BUZZER_LONG_BEEP,


    /*
     * Error notification pattern.
     *
     * Typical usage:
     *      Fault conditions
     *      Protection alarms
     */
    BUZZER_ERROR


} Buzzer_Pattern_t;



/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize buzzer module.
 *
 * @details
 *      This function prepares the buzzer module and places the hardware
 *      in a safe OFF state.
 *
 * @return
 *      None.
 */
void Buzzer_Init(void);



/**
 * @brief
 *      Select buzzer operating pattern.
 *
 * @param pattern
 *      Requested buzzer pattern.
 *
 * @details
 *      This function only requests a new pattern.
 *
 *      The actual execution is performed by Buzzer_Task().
 *
 *      This function is non-blocking and does not stop the CPU.
 *
 * @return
 *      None.
 */
void Buzzer_SetPattern(Buzzer_Pattern_t pattern);



/**
 * @brief
 *      Turn buzzer off immediately.
 *
 * @details
 *      Cancels any running buzzer pattern.
 *
 * @return
 *      None.
 */
void Buzzer_Off(void);



/**
 * @brief
 *      Execute buzzer state machine.
 *
 * @details
 *      This function must be called periodically from the main application
 *      loop.
 *
 *      Example:
 *
 *          while(1)
 *          {
 *              Buzzer_Task();
 *
 *              Other_Application_Tasks();
 *          }
 *
 *      The function uses HAL_GetTick() internally and does not block
 *      program execution.
 *
 * @return
 *      None.
 */
void Buzzer_Task(void);



#ifdef __cplusplus
}
#endif


#endif /* BUZZER_H */
