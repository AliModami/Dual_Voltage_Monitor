/******************************************************************************
 *
 * @file    buttons.h
 *
 * @brief   Push Button Driver Public Interface
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU:
 *
 *      STM32F103C8T6 (Blue Pill)
 *
 *------------------------------------------------------------------------------
 *
 * Framework:
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file defines the public interface of the push button driver.
 *
 *
 *      Features:
 *
 *          - Software debounce
 *          - Single press detection
 *          - Long press detection
 *          - Auto repeat
 *          - Repeat acceleration
 *          - Event based communication
 *
 *
 *      Application modules must never access GPIO directly.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      2.3.0
 *
 *------------------------------------------------------------------------------
 *
 * Change History:
 *
 *      Version 2.3.0
 *
 *          - Added long press support
 *          - Added auto repeat
 *          - Added repeat acceleration
 *          - Added configurable repeat timing
 *
 *      Version 2.2.0
 *
 *          - Added repeat event model
 *
 *      Version 2.0.0
 *
 *          - Basic debounce FSM
 *
 ******************************************************************************/

#ifndef BUTTONS_H
#define BUTTONS_H


#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "stm32f1xx_hal.h"

#include <stdbool.h>

#include <stdint.h>





/******************************************************************************
 *                         Hardware Configuration
 ******************************************************************************/

/*
 * Button electrical configuration:
 *
 * GPIO ---- Button ---- GND
 *
 * Active Low:
 *
 * Released:
 *      GPIO = HIGH
 *
 * Pressed:
 *      GPIO = LOW
 */



#define BUTTON_UP_PORT          GPIOB
#define BUTTON_UP_PIN           GPIO_PIN_14


#define BUTTON_DOWN_PORT        GPIOB
#define BUTTON_DOWN_PIN         GPIO_PIN_13


#define BUTTON_ENTER_PORT       GPIOB
#define BUTTON_ENTER_PIN        GPIO_PIN_12


#define BUTTON_BACK_PORT        GPIOB
#define BUTTON_BACK_PIN         GPIO_PIN_15






/******************************************************************************
 *                         Driver Configuration
 ******************************************************************************/

/*
 * Debounce time.
 *
 * The input must remain stable for this duration
 * before accepting a state change.
 */
#define BUTTON_DEBOUNCE_TIME_MS              10U




/*
 * Maximum number of stored events.
 */
#define BUTTON_EVENT_QUEUE_SIZE              16U






/******************************************************************************
 *                         Long Press Configuration
 ******************************************************************************/

/*
 * Time after initial press before repeat starts.
 *
 * Example:
 *
 * Hold DOWN:
 *
 *      Press
 *       |
 *       |
 *      600ms
 *       |
 *       v
 *
 *      Start repeat
 */
#define BUTTON_REPEAT_START_TIME_MS          200U





/*
 * Normal repeat interval.
 *
 * After repeat starts:
 *
 *      Event
 *
 *      wait 300ms
 *
 *      Event
 */
#define BUTTON_REPEAT_INTERVAL_NORMAL_MS     100U





/*
 * Fastest repeat interval.
 *
 * Acceleration will never
 * go below this value.
 */
#define BUTTON_REPEAT_INTERVAL_FAST_MS       40U





/*
 * Time required while holding
 * before acceleration begins.
 *
 * Example:
 *
 * Hold button:
 *
 * 0 - 3 seconds:
 *      300ms interval
 *
 * After 3 seconds:
 *      interval decreases
 */
#define BUTTON_REPEAT_ACCELERATION_TIME_MS   500U





/*
 * Amount of interval reduction
 * after each acceleration step.
 */
#define BUTTON_REPEAT_ACCELERATION_STEP_MS   50U





/******************************************************************************
 *                         Button Identification
 ******************************************************************************/

typedef enum
{

    /*
     * Increase value.
     */
    BUTTON_ID_UP = 0U,



    /*
     * Decrease value.
     */
    BUTTON_ID_DOWN,



    /*
     * Confirm operation.
     */
    BUTTON_ID_ENTER,



    /*
     * Return to previous page.
     */
    BUTTON_ID_BACK,



    /*
     * Number of buttons.
     */
    BUTTON_ID_COUNT


} Button_Id_t;





/******************************************************************************
 *                         Button Event Definition
 ******************************************************************************/

typedef enum
{

    /*
     * No event.
     */
    BUTTON_EVENT_NONE = 0U,



    /*
     * Initial press event.
     */
    BUTTON_EVENT_PRESS,



    /*
     * Auto repeat event.
     *
     * Generated while button
     * remains pressed.
     */
    BUTTON_EVENT_REPEAT


} Button_EventType_t;





typedef struct
{

    /*
     * Source button.
     */
    Button_Id_t button;



    /*
     * Event type.
     */
    Button_EventType_t event;


} Button_Event_t;

/******************************************************************************
 *                         Button Runtime Information
 ******************************************************************************/

/*
 * This structure is intentionally hidden from application.
 *
 * Application only receives:
 *
 *      Button_Event_t
 *
 * Internal state machine details remain inside buttons.c.
 */



/******************************************************************************
 *                         Public API Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize button driver.
 *
 * @details
 *
 *      Responsibilities:
 *
 *          - Clear runtime states
 *          - Initialize debounce FSM
 *          - Clear event queue
 *
 *
 *      GPIO initialization is performed by CubeMX.
 *
 */
void Buttons_Init(void);





/**
 * @brief
 *      Periodic button processing task.
 *
 * @details
 *
 *      Must be called continuously from main loop.
 *
 *
 *      Performs:
 *
 *          - GPIO sampling
 *          - Debounce processing
 *          - Long press detection
 *          - Auto repeat generation
 *          - Repeat acceleration
 *
 *
 *      Non blocking function.
 *
 *
 * Example:
 *
 *      while(1)
 *      {
 *          Buttons_Task();
 *      }
 *
 */
void Buttons_Task(void);





/**
 * @brief
 *      Get next available button event.
 *
 * @param event
 *      Destination event structure.
 *
 * @return
 *
 *      true:
 *          Valid event returned.
 *
 *      false:
 *          Queue empty.
 *
 */
bool Buttons_GetEvent(Button_Event_t *event);





/**
 * @brief
 *      Remove all pending events.
 *
 * @details
 *
 *      Useful after changing application mode.
 *
 */
void Buttons_Flush(void);





/**
 * @brief
 *      Read current electrical state.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *
 *      true:
 *          Button currently pressed.
 *
 *      false:
 *          Button released.
 *
 *
 * @note
 *
 *      This function bypasses debounce.
 *
 *      Normal application should use
 *
 *          Buttons_GetEvent()
 *
 */
bool Buttons_IsPressed(Button_Id_t button);





/******************************************************************************
 *                         Event Usage Example
 ******************************************************************************/

/*
 *
 * Example:
 *
 *
 * Button_Event_t event;
 *
 *
 * if(Buttons_GetEvent(&event))
 * {
 *
 *      switch(event.button)
 *      {
 *
 *          case BUTTON_ID_UP:
 *
 *              if(event.event == BUTTON_EVENT_PRESS)
 *              {
 *                  IncreaseValue();
 *              }
 *
 *              else if(event.event == BUTTON_EVENT_REPEAT)
 *              {
 *                  IncreaseValueFast();
 *              }
 *
 *              break;
 *
 *
 *          case BUTTON_ID_DOWN:
 *
 *              if(event.event == BUTTON_EVENT_REPEAT)
 *              {
 *                  DecreaseValue();
 *              }
 *
 *              break;
 *
 *      }
 *
 * }
 *
 *
 ******************************************************************************/




#ifdef __cplusplus
}
#endif


#endif /* BUTTONS_H */

/******************************************************************************
 *                              End Of File
 ******************************************************************************/

/*
 * Notes:
 *
 *      Version 2.3.0 introduces:
 *
 *      1.
 *      Debounced press detection
 *
 *
 *      2.
 *      Long press detection
 *
 *
 *      3.
 *      Auto repeat event generation
 *
 *
 *      4.
 *      Repeat acceleration
 *
 *
 *      Repeat behavior:
 *
 *
 *          Initial Press
 *
 *                |
 *                |
 *                v
 *
 *          BUTTON_EVENT_PRESS
 *
 *
 *                |
 *                |
 *          Hold button
 *
 *                |
 *                |
 *                v
 *
 *          Wait BUTTON_REPEAT_START_TIME_MS
 *
 *                |
 *                |
 *                v
 *
 *          BUTTON_EVENT_REPEAT
 *
 *
 *                |
 *                |
 *          Hold longer
 *
 *                |
 *                |
 *                v
 *
 *          Reduce repeat interval
 *
 *                |
 *                |
 *                v
 *
 *          Faster repeat events
 *
 *
 *
 *      Recommended usage:
 *
 *          UP / DOWN:
 *
 *              PRESS
 *              REPEAT
 *
 *
 *          ENTER / BACK:
 *
 *              PRESS only
 *
 *
 ******************************************************************************/
