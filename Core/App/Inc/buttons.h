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
 *      The button driver provides a hardware abstraction layer between
 *      physical GPIO inputs and application software.
 *
 *
 *      Application modules must never access button GPIO pins directly.
 *
 *      All button interactions must be performed through this API.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Driver Responsibilities:
 *
 *      - Reading physical button inputs
 *
 *      - Software debounce processing
 *
 *      - Detecting valid button press events
 *
 *      - Providing event based communication with application layer
 *
 *
 *------------------------------------------------------------------------------
 *
 * Software Architecture:
 *
 *
 *              Physical Buttons
 *
 *                     |
 *                     |
 *                     v
 *
 *              +---------------+
 *              |   buttons.c   |
 *              |---------------|
 *              | GPIO Reading  |
 *              | Debounce FSM  |
 *              | Event Queue   |
 *              +---------------+
 *
 *                     |
 *                     |
 *                     v
 *
 *              Button_Event_t
 *
 *                     |
 *                     |
 *                     v
 *
 *              Application Layer
 *
 *                     |
 *                     |
 *                     v
 *
 *              Menu Controller
 *
 *
 *------------------------------------------------------------------------------
 *
 * Design Principles:
 *
 *      1.
 *      Hardware details are isolated inside the driver.
 *
 *
 *      2.
 *      Application works with logical button IDs only.
 *
 *
 *      3.
 *      Mechanical switch noise is handled internally.
 *
 *
 *      4.
 *      No blocking delay is used.
 *
 *
 *      5.
 *      Driver is designed for periodic polling.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Example Usage:
 *
 *
 *      int main(void)
 *      {
 *
 *          HAL_Init();
 *
 *          MX_GPIO_Init();
 *
 *          Buttons_Init();
 *
 *
 *          while(1)
 *          {
 *
 *              Buttons_Task();
 *
 *
 *              Button_Event_t event;
 *
 *
 *              if(Buttons_GetEvent(&event))
 *              {
 *
 *                  switch(event.button)
 *                  {
 *
 *                      case BUTTON_ID_UP:
 *
 *                          Menu_MoveUp();
 *
 *                          break;
 *
 *
 *                      case BUTTON_ID_DOWN:
 *
 *                          Menu_MoveDown();
 *
 *                          break;
 *
 *
 *                      case BUTTON_ID_ENTER:
 *
 *                          Menu_Select();
 *
 *                          break;
 *
 *
 *                      case BUTTON_ID_BACK:
 *
 *                          Menu_Back();
 *
 *                          break;
 *
 *                  }
 *
 *              }
 *
 *          }
 *
 *      }
 *
 *
 *------------------------------------------------------------------------------
 *
 * Author:
 *
 *      Ali Modami & ChatGPT
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      2.0.0
 *
 *------------------------------------------------------------------------------
 *
 * Change History:
 *
 *
 *      Version 2.0.0
 *
 *          - Simplified event model
 *
 *          - Removed application dependency
 *
 *          - Prepared for Page Based Menu architecture
 *
 *          - Improved documentation
 *
 *
 *      Version 1.x
 *
 *          - Initial button driver implementation
 *
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
 * ============================================================================
 *
 * Button Hardware Mapping
 *
 * ============================================================================
 *
 *
 * Hardware connection:
 *
 *
 *          STM32F103C8T6
 *
 *
 *              PB14  ---- UP Button
 *
 *              PB13  ---- DOWN Button
 *
 *              PB12  ---- ENTER Button
 *
 *              PB15  ---- BACK Button
 *
 *
 *
 * Electrical configuration:
 *
 *
 *              GPIO ---- Button ---- GND
 *
 *
 * Internal Pull-Up configuration:
 *
 *
 *              Button Released:
 *
 *                  GPIO = HIGH
 *
 *
 *              Button Pressed:
 *
 *                  GPIO = LOW
 *
 *
 * The driver converts this electrical behavior into
 * logical button events.
 *
 *
 * Application does not need to know about Active Low logic.
 *
 * ============================================================================
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
 * ============================================================================
 *
 * Timing Configuration
 *
 * ============================================================================
 *
 * All values are expressed in milliseconds.
 *
 * These parameters define the response characteristics
 * of the button driver.
 *
 * ============================================================================
 */



/*
 * Minimum stable time required before accepting
 * a GPIO transition as a valid button press.
 *
 *
 * Larger value:
 *
 *      + Better noise rejection
 *
 *      - Slower response
 *
 */
#define BUTTON_DEBOUNCE_TIME_MS      20U




/*
 * Maximum number of pending events stored internally.
 *
 * Circular buffer is used internally.
 *
 */
#define BUTTON_EVENT_QUEUE_SIZE      16U





/******************************************************************************
 *                         Button Identification
 ******************************************************************************/

/**
 * @brief
 *      Logical button identifiers.
 *
 *
 * @details
 *
 *      These identifiers represent buttons from the
 *      application point of view.
 *
 *      Hardware pins are hidden inside buttons.c.
 *
 *
 * Example:
 *
 *
 *      if(event.button == BUTTON_ID_ENTER)
 *      {
 *
 *          Open selected menu item;
 *
 *      }
 *
 */
typedef enum
{

    /*
     * Move cursor upward.
     */
    BUTTON_ID_UP = 0U,



    /*
     * Move cursor downward.
     */
    BUTTON_ID_DOWN,



    /*
     * Confirm selection.
     */
    BUTTON_ID_ENTER,



    /*
     * Return to previous menu page.
     */
    BUTTON_ID_BACK,



    /*
     * Number of available buttons.
     *
     * Used internally for array sizing.
     */
    BUTTON_ID_COUNT


} Button_Id_t;





/******************************************************************************
 *                         Button Event Definition
 ******************************************************************************/

/**
 * @brief
 *      Button event types generated by driver.
 *
 *
 * @details
 *
 *      The driver converts raw GPIO changes into
 *      clean software events.
 *
 *
 *      Example:
 *
 *          Mechanical switch:
 *
 *              HIGH LOW HIGH LOW HIGH
 *
 *
 *          After debounce:
 *
 *              BUTTON_EVENT_PRESS
 *
 *
 */
typedef enum
{

    /*
     * No valid event.
     */
    BUTTON_EVENT_NONE = 0U,



    /*
     * Valid button press event.
     *
     * This is the main event used by menu system.
     */
    BUTTON_EVENT_PRESS


} Button_EventType_t;




/**
 * @brief
 *      Button event data container.
 *
 *
 * @details
 *
 *      Every generated event contains:
 *
 *          - Source button
 *
 *          - Event type
 *
 *
 * Example:
 *
 *
 *      Button_Event_t event;
 *
 *
 *      if(Buttons_GetEvent(&event))
 *      {
 *
 *          if(event.button == BUTTON_ID_BACK)
 *          {
 *
 *              Menu_Back();
 *
 *          }
 *
 *      }
 *
 */
typedef struct
{

    /*
     * Logical button source.
     */
    Button_Id_t button;



    /*
     * Generated button action.
     */
    Button_EventType_t event;


} Button_Event_t;



/******************************************************************************
 *                         Public API Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize button driver.
 *
 * @details
 *
 *      This function must be called once during system startup.
 *
 *      Responsibilities:
 *
 *          - Initialize internal button states
 *          - Clear event queue
 *          - Prepare driver runtime context
 *
 *
 *      Note:
 *
 *          GPIO configuration is NOT performed here.
 *
 *          GPIO initialization must be handled by STM32CubeMX
 *          generated code.
 *
 *
 * Example:
 *
 *      int main(void)
 *      {
 *          HAL_Init();
 *
 *          MX_GPIO_Init();
 *
 *          Buttons_Init();
 *
 *          while(1)
 *          {
 *              Buttons_Task();
 *          }
 *      }
 *
 */
void Buttons_Init(void);



/**
 * @brief
 *      Periodic execution function of button driver.
 *
 * @details
 *
 *      This function must be called continuously from the main loop.
 *
 *
 *      It performs:
 *
 *          - GPIO sampling
 *          - Debounce processing
 *          - State machine update
 *          - Event generation
 *
 *
 *      The function is non-blocking.
 *
 *
 * Example:
 *
 *      while(1)
 *      {
 *          Buttons_Task();
 *
 *          Menu_Task();
 *      }
 *
 */
void Buttons_Task(void);



/**
 * @brief
 *      Read next button event from internal queue.
 *
 * @param event
 *      Pointer to destination event structure.
 *
 * @return
 *
 *      true:
 *          A valid event was received.
 *
 *      false:
 *          Queue is empty or parameter is invalid.
 *
 *
 * Example:
 *
 *      Button_Event_t event;
 *
 *      if(Buttons_GetEvent(&event))
 *      {
 *          if(event.button == BUTTON_ID_ENTER)
 *          {
 *              MenuController_Enter();
 *          }
 *      }
 *
 */
bool Buttons_GetEvent(Button_Event_t *event);



/**
 * @brief
 *      Remove all pending button events.
 *
 * @details
 *
 *      Useful when changing application mode or menu page.
 *
 *      Example:
 *
 *          MenuController_OpenPage();
 *
 *          Buttons_Flush();
 *
 */
void Buttons_Flush(void);



/**
 * @brief
 *      Read current physical button state.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *
 *      true:
 *          Button is currently pressed.
 *
 *      false:
 *          Button is released.
 *
 *
 * @note
 *
 *      This function does NOT apply debounce.
 *
 *      It should only be used for special cases,
 *      not for normal menu navigation.
 *
 */
bool Buttons_IsPressed(Button_Id_t button);



#ifdef __cplusplus
}
#endif


#endif /* BUTTONS_H */

/******************************************************************************
 *                              End Of File
 ******************************************************************************/
