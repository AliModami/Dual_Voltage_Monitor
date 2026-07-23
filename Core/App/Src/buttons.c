/******************************************************************************
 *
 * @file    buttons.c
 *
 * @brief   Push Button Driver Implementation
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
 *      This file contains the implementation of the push button driver.
 *
 *
 *      The driver provides a hardware abstraction layer between:
 *
 *
 *              Physical GPIO Buttons
 *
 *                         |
 *
 *                         v
 *
 *              Software Button Events
 *
 *
 *
 *      Application modules must never access button GPIO pins directly.
 *
 *      All button operations must be performed through buttons.h API.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Driver Responsibilities:
 *
 *      - Reading GPIO button inputs
 *
 *      - Removing mechanical switch bounce
 *
 *      - Detecting stable button press events
 *
 *      - Managing internal event FIFO queue
 *
 *      - Providing clean events to application layer
 *
 *
 *------------------------------------------------------------------------------
 *
 * Software Architecture:
 *
 *
 *              GPIO Hardware
 *
 *                    |
 *
 *                    v
 *
 *             Button_Read()
 *
 *                    |
 *
 *                    v
 *
 *             Debounce FSM
 *
 *                    |
 *
 *                    v
 *
 *          Button Event Generator
 *
 *                    |
 *
 *                    v
 *
 *              FIFO Queue
 *
 *                    |
 *
 *                    v
 *
 *          Buttons_GetEvent()
 *
 *                    |
 *
 *                    v
 *
 *          Menu Controller
 *
 *
 *------------------------------------------------------------------------------
 *
 * Design Notes:
 *
 *      1.
 *      Hardware details are isolated inside this module.
 *
 *
 *      2.
 *      Application works only with logical button IDs.
 *
 *
 *      3.
 *      Debounce processing is completely non-blocking.
 *
 *
 *      4.
 *      Driver is designed for periodic polling.
 *
 *
 * Example:
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
 *                  if(event.button == BUTTON_ID_ENTER)
 *                  {
 *                      // Open selected menu item
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
 *      Version 2.0.0
 *
 *          - Rewritten for Page Based Menu architecture
 *
 *          - Removed Long Press and Auto Repeat
 *
 *          - Simplified event model
 *
 *          - Improved hardware abstraction
 *
 *
 ******************************************************************************/




/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "buttons.h"

#include <string.h>





/******************************************************************************
 *                         Private Configuration
 ******************************************************************************/

/*
 * ============================================================================
 *
 * Internal Driver Timing Configuration
 *
 * ============================================================================
 *
 * These parameters control the behavior of the debounce algorithm.
 *
 *
 * The values are intentionally kept private because they define the internal
 * behavior of the driver, not the application interface.
 *
 *
 * Example:
 *
 *      Increasing BUTTON_DEBOUNCE_TIME_MS:
 *
 *          + Better noise rejection
 *
 *          - Slightly slower response
 *
 * ============================================================================
 */


/*
 * Required stable time before accepting a button press.
 *
 * Unit:
 *
 *      milliseconds
 *
 */
#define BUTTON_INTERNAL_DEBOUNCE_TIME_MS     40U





/*
 * Internal queue size.
 *
 * Must match the public definition in buttons.h.
 *
 */
#define BUTTON_INTERNAL_QUEUE_SIZE           BUTTON_EVENT_QUEUE_SIZE





/******************************************************************************
 *                         Private Types
 ******************************************************************************/

/*
 * ============================================================================
 *
 * Button State Machine
 *
 * ============================================================================
 *
 * Every button has its own independent state machine.
 *
 *
 * State transition:
 *
 *
 *
 *          Released
 *
 *              |
 *
 *              | GPIO Pressed
 *
 *              v
 *
 *       Debounce Press
 *
 *              |
 *
 *              | Stable
 *
 *              v
 *
 *          Pressed
 *
 *
 *
 *          Pressed
 *
 *              |
 *
 *              | GPIO Released
 *
 *              v
 *
 *       Debounce Release
 *
 *              |
 *
 *              | Stable
 *
 *              v
 *
 *          Released
 *
 *
 * ============================================================================
 */

typedef enum
{

    /*
     * Button is stable and released.
     */
    BUTTON_STATE_RELEASED = 0U,


    /*
     * Possible press detected.
     *
     * Waiting for debounce validation.
     */
    BUTTON_STATE_DEBOUNCE_PRESS,


    /*
     * Button is stable and pressed.
     */
    BUTTON_STATE_PRESSED,


    /*
     * Possible release detected.
     *
     * Waiting for debounce validation.
     */
    BUTTON_STATE_DEBOUNCE_RELEASE


} Button_State_t;





/**
 * @brief
 *      Hardware mapping information.
 *
 * @details
 *
 *      This structure connects logical button IDs
 *      with physical GPIO resources.
 *
 *
 *      Application never accesses this information.
 *
 */
typedef struct
{

    /*
     * GPIO port.
     */
    GPIO_TypeDef *port;


    /*
     * GPIO pin.
     */
    uint16_t pin;


} Button_Hardware_t;






/**
 * @brief
 *      Runtime context of each button.
 *
 * @details
 *
 *      Each button owns an independent context.
 *
 *      This allows:
 *
 *          UP
 *          DOWN
 *          ENTER
 *          BACK
 *
 *      to be processed independently.
 *
 */
typedef struct
{

    /*
     * Current state machine state.
     */
    Button_State_t state;



    /*
     * Timestamp of last state transition.
     *
     * HAL_GetTick() is used as time reference.
     */
    uint32_t timestamp;


} Button_Context_t;






/**
 * @brief
 *      Internal circular event queue.
 *
 * @details
 *
 *      Events are stored temporarily until
 *      application reads them.
 *
 *
 *      Example flow:
 *
 *
 *          Button Press
 *
 *                |
 *
 *                v
 *
 *          FIFO Queue
 *
 *                |
 *
 *                v
 *
 *          Buttons_GetEvent()
 *
 *
 */
typedef struct
{

    /*
     * Event storage buffer.
     */
    Button_Event_t buffer[BUTTON_INTERNAL_QUEUE_SIZE];



    /*
     * Next write position.
     */
    uint8_t head;



    /*
     * Next read position.
     */
    uint8_t tail;



    /*
     * Current number of stored events.
     */
    uint8_t count;


} Button_EventQueue_t;


/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * ============================================================================
 *
 * Button Hardware Mapping Table
 *
 * ============================================================================
 *
 * This table is the only place where logical buttons are connected
 * to physical STM32 GPIO pins.
 *
 *
 * Application layer only knows:
 *
 *      BUTTON_ID_UP
 *      BUTTON_ID_DOWN
 *      BUTTON_ID_ENTER
 *      BUTTON_ID_BACK
 *
 *
 * Hardware details remain hidden inside this driver.
 *
 *
 * If hardware wiring changes in future:
 *
 *      Only this table must be modified.
 *
 *      Menu Controller and Application code remain unchanged.
 *
 * ============================================================================
 */

static const Button_Hardware_t button_hardware[BUTTON_ID_COUNT] =
{

    /*
     * BUTTON_ID_UP
     *
     * Hardware:
     *
     *      PB14
     */
    {
        .port = GPIOB,
        .pin  = GPIO_PIN_14
    },


    /*
     * BUTTON_ID_DOWN
     *
     * Hardware:
     *
     *      PB13
     */
    {
        .port = GPIOB,
        .pin  = GPIO_PIN_13
    },


    /*
     * BUTTON_ID_ENTER
     *
     * Hardware:
     *
     *      PB12
     */
    {
        .port = GPIOB,
        .pin  = GPIO_PIN_12
    },


    /*
     * BUTTON_ID_BACK
     *
     * Hardware:
     *
     *      PB15
     */
    {
        .port = GPIOB,
        .pin  = GPIO_PIN_15
    }

};





/*
 * ============================================================================
 *
 * Button Runtime Context
 *
 * ============================================================================
 *
 * One independent context exists for each button.
 *
 *
 * Example:
 *
 *
 *      button_context[BUTTON_ID_UP]
 *
 *              |
 *              +--> State
 *              +--> Timestamp
 *
 *
 * This design is easier to maintain compared with separate variables:
 *
 *      up_state
 *      down_state
 *      enter_state
 *      back_state
 *
 *
 * ============================================================================
 */

static Button_Context_t button_context[BUTTON_ID_COUNT];






/*
 * ============================================================================
 *
 * Event FIFO Queue
 *
 * ============================================================================
 *
 * Button events are generated by the state machine and stored here.
 *
 *
 * Producer:
 *
 *      Button Driver
 *
 *
 * Consumer:
 *
 *      Application Layer
 *
 *
 * This separation prevents application processing time from affecting
 * button detection.
 *
 * ============================================================================
 */

static Button_EventQueue_t button_queue;






/******************************************************************************
 *                      Private Function Prototypes
 ******************************************************************************/

/**
 * @brief
 *      Read raw GPIO state of a logical button.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *      true  -> button pressed
 *
 *      false -> button released
 *
 *
 * @note
 *      Active Low hardware behavior is hidden here.
 */
static bool Button_Read(Button_Id_t button);





/**
 * @brief
 *      Process one button state machine.
 *
 * @param button
 *      Logical button identifier.
 *
 * @details
 *
 *      This function performs:
 *
 *          - debounce handling
 *          - state transition
 *          - event generation
 *
 */
static void Button_Process(Button_Id_t button);





/**
 * @brief
 *      Add event to internal FIFO queue.
 *
 * @param event
 *      Event to store.
 *
 * @return
 *      true  -> success
 *
 *      false -> queue full
 */
static bool Button_QueuePush(Button_Event_t event);





/**
 * @brief
 *      Remove event from internal FIFO queue.
 *
 * @param event
 *      Destination event pointer.
 *
 * @return
 *      true  -> event available
 *
 *      false -> queue empty
 */
static bool Button_QueuePop(Button_Event_t *event);





/**
 * @brief
 *      Generate button press event.
 *
 * @param button
 *      Source button.
 */
static void Button_GeneratePressEvent(Button_Id_t button);






/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize button driver.
 *
 * @details
 *
 *      This function prepares internal driver resources.
 *
 *
 *      It does NOT initialize GPIO.
 *
 *
 *      GPIO must be configured by STM32CubeMX.
 *
 *
 * Example:
 *
 *
 *      MX_GPIO_Init();
 *
 *      Buttons_Init();
 *
 *
 */
void Buttons_Init(void)
{
    uint8_t index;



    /*
     * Clear all runtime contexts.
     *
     * This guarantees a known startup condition.
     */
    memset(button_context,
           0,
           sizeof(button_context));



    /*
     * Initialize every button state.
     */
    for(index = 0U;
        index < BUTTON_ID_COUNT;
        index++)
    {

        /*
         * Initial condition:
         *
         * Button released.
         */
        button_context[index].state =
                BUTTON_STATE_RELEASED;



        /*
         * Store current system time.
         */
        button_context[index].timestamp =
                HAL_GetTick();

    }



    /*
     * Clear event queue.
     */
    memset(&button_queue,
           0,
           sizeof(button_queue));

}






/**
 * @brief
 *      Periodic button processing task.
 *
 * @details
 *
 *      This function must be called continuously.
 *
 *
 * Example:
 *
 *
 *      while(1)
 *      {
 *          Buttons_Task();
 *          Menu_Task();
 *      }
 *
 *
 * The function is:
 *
 *      - Non blocking
 *      - Deterministic
 *      - Suitable for embedded systems
 *
 */
void Buttons_Task(void)
{
    uint8_t index;



    /*
     * Process all buttons independently.
     */
    for(index = 0U;
        index < BUTTON_ID_COUNT;
        index++)
    {
        Button_Process((Button_Id_t)index);
    }

}






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
 *          Event received.
 *
 *      false:
 *          No event available.
 *
 */
bool Buttons_GetEvent(Button_Event_t *event)
{

    /*
     * Validate pointer.
     */
    if(event == NULL)
    {
        return false;
    }



    /*
     * Read from FIFO.
     */
    return Button_QueuePop(event);

}






/**
 * @brief
 *      Remove all pending events.
 *
 * @details
 *
 * Useful when changing application mode.
 *
 *
 * Example:
 *
 *      MenuController_OpenPage();
 *
 *      Buttons_Flush();
 *
 */
void Buttons_Flush(void)
{
    /*
     * Reset FIFO indexes.
     */
    button_queue.head  = 0U;

    button_queue.tail  = 0U;

    button_queue.count = 0U;

}






/**
 * @brief
 *      Check current button electrical state.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *
 *      true:
 *          button pressed
 *
 *      false:
 *          button released
 *
 *
 * @note
 *      This function bypasses debounce.
 *
 *      Normally application should use
 *      Buttons_GetEvent().
 *
 */
bool Buttons_IsPressed(Button_Id_t button)
{

    /*
     * Validate button ID.
     */
    if(button >= BUTTON_ID_COUNT)
    {
        return false;
    }



    return Button_Read(button);

}


/******************************************************************************
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Read physical state of a button.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *
 *      true:
 *          Button is pressed.
 *
 *      false:
 *          Button is released.
 *
 *
 * @details
 *
 *      Hardware uses Active Low configuration:
 *
 *
 *          Released:
 *
 *              GPIO = HIGH
 *
 *
 *          Pressed:
 *
 *              GPIO = LOW
 *
 *
 *      This function converts the electrical behavior
 *      into a logical software state.
 *
 */
static bool Button_Read(Button_Id_t button)
{
    GPIO_PinState state;



    /*
     * Read GPIO input.
     */
    state =
        HAL_GPIO_ReadPin(button_hardware[button].port,
                         button_hardware[button].pin);



    /*
     * Active Low logic:
     *
     * RESET  -> Pressed
     *
     * SET    -> Released
     */
    if(state == GPIO_PIN_RESET)
    {
        return true;
    }


    return false;

}







/**
 * @brief
 *      Process one button state machine.
 *
 * @param button
 *      Logical button identifier.
 *
 * @details
 *
 *      This function implements software debounce.
 *
 *
 *      A mechanical switch does not change state instantly.
 *
 *
 *      Example:
 *
 *
 *          Real Press:
 *
 *              LOW HIGH LOW HIGH LOW
 *
 *
 *          Driver Result:
 *
 *              BUTTON_EVENT_PRESS
 *
 *
 *      Only stable transitions are accepted.
 *
 */
static void Button_Process(Button_Id_t button)
{
    uint32_t now;

    bool pressed;



    /*
     * Current system time.
     */
    now = HAL_GetTick();



    /*
     * Logical button state.
     */
    pressed = Button_Read(button);



    switch(button_context[button].state)
    {


        /*
         * ------------------------------------------------------------
         *
         * Button is released.
         *
         * Waiting for new press.
         *
         * ------------------------------------------------------------
         */
        case BUTTON_STATE_RELEASED:


            if(pressed)
            {

                /*
                 * Possible press detected.
                 *
                 * Start debounce timer.
                 */
                button_context[button].timestamp = now;


                button_context[button].state =
                        BUTTON_STATE_DEBOUNCE_PRESS;

            }


            break;





        /*
         * ------------------------------------------------------------
         *
         * Possible press detected.
         *
         * Waiting for stable condition.
         *
         * ------------------------------------------------------------
         */
        case BUTTON_STATE_DEBOUNCE_PRESS:


            if(!pressed)
            {

                /*
                 * False trigger.
                 *
                 * Mechanical bounce rejected.
                 */
                button_context[button].state =
                        BUTTON_STATE_RELEASED;

            }


            else if((now -
                     button_context[button].timestamp)
                    >= BUTTON_INTERNAL_DEBOUNCE_TIME_MS)
            {

                /*
                 * Valid press confirmed.
                 */
                Button_GeneratePressEvent(button);



                /*
                 * Move to pressed state.
                 */
                button_context[button].state =
                        BUTTON_STATE_PRESSED;

            }


            break;





        /*
         * ------------------------------------------------------------
         *
         * Button is confirmed pressed.
         *
         * Waiting for release.
         *
         * ------------------------------------------------------------
         */
        case BUTTON_STATE_PRESSED:


            if(!pressed)
            {

                /*
                 * Possible release detected.
                 */
                button_context[button].timestamp = now;


                button_context[button].state =
                        BUTTON_STATE_DEBOUNCE_RELEASE;

            }


            break;





        /*
         * ------------------------------------------------------------
         *
         * Possible release detected.
         *
         * Waiting for stable release.
         *
         * ------------------------------------------------------------
         */
        case BUTTON_STATE_DEBOUNCE_RELEASE:


            if(pressed)
            {

                /*
                 * Release was caused by bounce.
                 *
                 * Return to pressed state.
                 */
                button_context[button].state =
                        BUTTON_STATE_PRESSED;

            }


            else if((now -
                     button_context[button].timestamp)
                    >= BUTTON_INTERNAL_DEBOUNCE_TIME_MS)
            {

                /*
                 * Stable release confirmed.
                 */
                button_context[button].state =
                        BUTTON_STATE_RELEASED;

            }


            break;





        /*
         * Safety recovery.
         */
        default:


            button_context[button].state =
                    BUTTON_STATE_RELEASED;


            break;


    }

}








/**
 * @brief
 *      Generate button press event.
 *
 * @param button
 *      Source button.
 *
 * @details
 *
 *      This function creates a clean software event
 *      from a validated physical press.
 *
 *
 *      The application never knows:
 *
 *          - GPIO level
 *          - debounce timing
 *          - electrical configuration
 *
 *
 *      It only receives:
 *
 *          BUTTON_EVENT_PRESS
 *
 */
static void Button_GeneratePressEvent(Button_Id_t button)
{
    Button_Event_t event;



    /*
     * Prepare event packet.
     */
    event.button = button;


    event.event = BUTTON_EVENT_PRESS;



    /*
     * Store event.
     */
    Button_QueuePush(event);

}







/**
 * @brief
 *      Insert event into FIFO queue.
 *
 * @param event
 *      Event to store.
 *
 * @return
 *
 *      true:
 *          Stored successfully.
 *
 *      false:
 *          Queue full.
 *
 */
static bool Button_QueuePush(Button_Event_t event)
{

    /*
     * Check overflow.
     */
    if(button_queue.count >= BUTTON_INTERNAL_QUEUE_SIZE)
    {
        return false;
    }



    /*
     * Store event at head position.
     */
    button_queue.buffer[button_queue.head] = event;



    /*
     * Move head pointer.
     */
    button_queue.head++;



    if(button_queue.head >= BUTTON_INTERNAL_QUEUE_SIZE)
    {
        button_queue.head = 0U;
    }



    /*
     * Increase item count.
     */
    button_queue.count++;



    return true;

}







/**
 * @brief
 *      Remove oldest event from FIFO queue.
 *
 * @param event
 *      Destination event pointer.
 *
 * @return
 *
 *      true:
 *          Event returned.
 *
 *      false:
 *          Queue empty.
 *
 */
static bool Button_QueuePop(Button_Event_t *event)
{

    /*
     * Validate pointer.
     */
    if(event == NULL)
    {
        return false;
    }



    /*
     * Check empty queue.
     */
    if(button_queue.count == 0U)
    {
        return false;
    }



    /*
     * Read oldest event.
     */
    *event =
        button_queue.buffer[button_queue.tail];



    /*
     * Move tail pointer.
     */
    button_queue.tail++;



    if(button_queue.tail >= BUTTON_INTERNAL_QUEUE_SIZE)
    {
        button_queue.tail = 0U;
    }



    /*
     * Decrease item count.
     */
    button_queue.count--;



    return true;

}






/******************************************************************************
 *                              End Of File
 ******************************************************************************/
