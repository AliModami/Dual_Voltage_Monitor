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
 *      STM32F103C8T6
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
 *      This file implements the push button driver.
 *
 *
 *      Features:
 *
 *          - Software debounce
 *          - Press event detection
 *          - Long press detection
 *          - Auto repeat
 *          - Repeat acceleration
 *          - FIFO event queue
 *
 *
 *      The application layer does not access GPIO directly.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      2.3.0
 *
 ******************************************************************************/

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "buttons.h"

#include <string.h>





/******************************************************************************
 *                         Private Types
 ******************************************************************************/

/*
 * Button state machine.
 *
 *
 * State Flow:
 *
 *
 * RELEASED
 *
 *      |
 *      | Button pressed
 *      v
 *
 * DEBOUNCE_PRESS
 *
 *      |
 *      | Stable press
 *      v
 *
 * PRESSED
 *
 *      |
 *      | Hold time reached
 *      v
 *
 * REPEAT_ACTIVE
 *
 *
 * Release follows:
 *
 * PRESSED / REPEAT_ACTIVE
 *
 *      |
 *      | Button released
 *      v
 *
 * DEBOUNCE_RELEASE
 *
 *      |
 *      | Stable release
 *      v
 *
 * RELEASED
 *
 */

typedef enum
{

    BUTTON_STATE_RELEASED = 0U,


    BUTTON_STATE_DEBOUNCE_PRESS,


    BUTTON_STATE_PRESSED,


    BUTTON_STATE_REPEAT_ACTIVE,


    BUTTON_STATE_DEBOUNCE_RELEASE


} Button_State_t;





/*
 * Hardware mapping information.
 */
typedef struct
{

    GPIO_TypeDef *port;


    uint16_t pin;


} Button_Hardware_t;





/*
 * Runtime information for every button.
 */
typedef struct
{

    Button_State_t state;


    /*
     * Time of last state change.
     */
    uint32_t timestamp;



    /*
     * Time when button became pressed.
     */
    uint32_t press_start_time;



    /*
     * Last repeat generation time.
     */
    uint32_t repeat_timestamp;



    /*
     * Current repeat interval.
     */
    uint32_t repeat_interval;



    /*
     * Acceleration active flag.
     */
    bool acceleration_active;



} Button_Context_t;





/*
 * Event FIFO queue.
 */
typedef struct
{

    Button_Event_t buffer[BUTTON_EVENT_QUEUE_SIZE];


    uint8_t head;


    uint8_t tail;


    uint8_t count;


} Button_EventQueue_t;





/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * GPIO mapping table.
 */
static const Button_Hardware_t button_hardware[BUTTON_ID_COUNT] =
{

    {
        .port = GPIOB,
        .pin  = GPIO_PIN_14
    },


    {
        .port = GPIOB,
        .pin  = GPIO_PIN_13
    },


    {
        .port = GPIOB,
        .pin  = GPIO_PIN_12
    },


    {
        .port = GPIOB,
        .pin  = GPIO_PIN_15
    }

};





/*
 * Runtime context.
 */
static Button_Context_t button_context[BUTTON_ID_COUNT];





/*
 * Event queue.
 */
static Button_EventQueue_t button_queue;





/******************************************************************************
 *                      Private Function Prototypes
 ******************************************************************************/

static bool Button_ReadRaw(Button_Id_t button);


static void Button_Process(Button_Id_t button);


static void Button_ProcessRepeat(Button_Id_t button);


static void Button_GenerateEvent(Button_Id_t button,
                                 Button_EventType_t type);



static bool Button_QueuePush(const Button_Event_t *event);


static bool Button_QueuePop(Button_Event_t *event);

/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize button driver.
 *
 * @details
 *
 *      Initializes:
 *
 *          - Button FSM states
 *          - Repeat parameters
 *          - Event queue
 *
 */
void Buttons_Init(void)
{
    uint8_t index;


    /*
     * Clear runtime contexts.
     */
    memset(button_context,
           0,
           sizeof(button_context));



    /*
     * Initialize every button.
     */
    for(index = 0U;
        index < BUTTON_ID_COUNT;
        index++)
    {

        button_context[index].state =
                BUTTON_STATE_RELEASED;



        button_context[index].timestamp =
                HAL_GetTick();



        button_context[index].repeat_interval =
                BUTTON_REPEAT_INTERVAL_NORMAL_MS;



        button_context[index].acceleration_active =
                false;

    }



    /*
     * Clear event FIFO.
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
 *      Must run continuously.
 *
 */
void Buttons_Task(void)
{
    uint8_t index;



    for(index = 0U;
        index < BUTTON_ID_COUNT;
        index++)
    {

        Button_Process((Button_Id_t)index);

    }

}





/**
 * @brief
 *      Get next event from FIFO.
 */
bool Buttons_GetEvent(Button_Event_t *event)
{

    if(event == NULL)
    {
        return false;
    }



    return Button_QueuePop(event);

}





/**
 * @brief
 *      Remove all pending events.
 */
void Buttons_Flush(void)
{

    button_queue.head = 0U;


    button_queue.tail = 0U;


    button_queue.count = 0U;

}





/**
 * @brief
 *      Read current button state.
 */
bool Buttons_IsPressed(Button_Id_t button)
{

    if(button >= BUTTON_ID_COUNT)
    {
        return false;
    }


    return Button_ReadRaw(button);

}





/******************************************************************************
 *                         Private Functions
 ******************************************************************************/

/**
 * @brief
 *      Read physical button input.
 *
 * @details
 *
 *      Hardware is Active Low.
 *
 */
static bool Button_ReadRaw(Button_Id_t button)
{

    GPIO_PinState state;



    if(button >= BUTTON_ID_COUNT)
    {
        return false;
    }



    state =
        HAL_GPIO_ReadPin(button_hardware[button].port,
                         button_hardware[button].pin);



    if(state == GPIO_PIN_RESET)
    {
        return true;
    }


    return false;

}





/**
 * @brief
 *      Process one button state machine.
 */
static void Button_Process(Button_Id_t button)
{

    uint32_t now;


    bool pressed;



    now = HAL_GetTick();



    pressed = Button_ReadRaw(button);



    switch(button_context[button].state)
    {


        /*
         * ------------------------------------------------------------
         * Released state
         * ------------------------------------------------------------
         */
        case BUTTON_STATE_RELEASED:


            if(pressed)
            {

                button_context[button].timestamp = now;


                button_context[button].state =
                        BUTTON_STATE_DEBOUNCE_PRESS;

            }


            break;



        /*
         * ------------------------------------------------------------
         * Debounce press
         * ------------------------------------------------------------
         */
        case BUTTON_STATE_DEBOUNCE_PRESS:


            if(!pressed)
            {

                button_context[button].state =
                        BUTTON_STATE_RELEASED;

            }


            else if((now -
                     button_context[button].timestamp)
                    >= BUTTON_DEBOUNCE_TIME_MS)
            {

                /*
                 * Valid press.
                 */
                Button_GenerateEvent(button,
                                     BUTTON_EVENT_PRESS);



                button_context[button].press_start_time =
                        now;



                button_context[button].repeat_timestamp =
                        now;



                button_context[button].repeat_interval =
                        BUTTON_REPEAT_INTERVAL_NORMAL_MS;



                button_context[button].acceleration_active =
                        false;



                button_context[button].state =
                        BUTTON_STATE_PRESSED;

            }


            break;
            /*
             * ------------------------------------------------------------
             * Pressed state
             *
             * Button is stable pressed.
             *
             * Waiting for:
             *
             *      - Release
             *      - Repeat start
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


                else
                {

                    /*
                     * Only UP and DOWN support repeat.
                     */
                    if((button == BUTTON_ID_UP) ||
                       (button == BUTTON_ID_DOWN))
                    {

                        if((now -
                            button_context[button].press_start_time)
                           >= BUTTON_REPEAT_START_TIME_MS)
                        {

                            /*
                             * Start repeat mode.
                             */
                        	button_context[button].repeat_timestamp =
                        	        now - button_context[button].repeat_interval;


                        	button_context[button].state =
                        	        BUTTON_STATE_REPEAT_ACTIVE;

                        }

                    }

                }


                break;





            /*
             * ------------------------------------------------------------
             *
             * Repeat Active state
             *
             * Generates repeated events while
             * button remains pressed.
             *
             * ------------------------------------------------------------
             */
            case BUTTON_STATE_REPEAT_ACTIVE:


                if(!pressed)
                {

                    /*
                     * Release detected.
                     */
                    button_context[button].timestamp = now;


                    button_context[button].state =
                            BUTTON_STATE_DEBOUNCE_RELEASE;

                }


                else
                {

                    Button_ProcessRepeat(button);

                }


                break;





            /*
             * ------------------------------------------------------------
             *
             * Debounce Release
             *
             * ------------------------------------------------------------
             */
            case BUTTON_STATE_DEBOUNCE_RELEASE:


                if(pressed)
                {

                    /*
                     * Release was noise.
                     */
                    button_context[button].state =
                            BUTTON_STATE_REPEAT_ACTIVE;

                }


                else if((now -
                         button_context[button].timestamp)
                        >= BUTTON_DEBOUNCE_TIME_MS)
                {

                    /*
                     * Stable release confirmed.
                     */
                    button_context[button].state =
                            BUTTON_STATE_RELEASED;



                    /*
                     * Reset repeat parameters.
                     */
                    button_context[button].repeat_interval =
                            BUTTON_REPEAT_INTERVAL_NORMAL_MS;


                    button_context[button].acceleration_active =
                            false;

                }


                break;





            default:


                button_context[button].state =
                        BUTTON_STATE_RELEASED;


                break;


            }


            }





            /**
             * @brief
             *      Process auto repeat and acceleration.
             *
             * @param button
             *      Logical button identifier.
             *
             *
             * @details
             *
             *      Repeat starts slowly:
             *
             *          300ms
             *
             *      After holding:
             *
             *          250ms
             *          200ms
             *          150ms
             *          100ms
             *
             *
             */


static void Button_ProcessRepeat(Button_Id_t button)
{

    uint32_t now;


    now = HAL_GetTick();



    /*
     * Generate repeat event
     */
    if((now - button_context[button].repeat_timestamp)
       >= button_context[button].repeat_interval)
    {

        Button_GenerateEvent(button,
                             BUTTON_EVENT_REPEAT);



        /*
         * Keep accurate timing.
         */
        button_context[button].repeat_timestamp = now;




        /*
         * Enable acceleration earlier.
         */
        if((now - button_context[button].press_start_time)
           >= BUTTON_REPEAT_ACCELERATION_TIME_MS)
        {

            button_context[button].acceleration_active = true;

        }




        /*
         * Reduce repeat interval.
         *
         * Example:
         *
         * 300
         * 250
         * 200
         * 150
         * 100 ms
         *
         */
        if(button_context[button].acceleration_active)
        {

            if(button_context[button].repeat_interval >
               BUTTON_REPEAT_INTERVAL_FAST_MS)
            {


                if(button_context[button].repeat_interval -
                   BUTTON_REPEAT_ACCELERATION_STEP_MS >=
                   BUTTON_REPEAT_INTERVAL_FAST_MS)
                {

                    button_context[button].repeat_interval -=
                            BUTTON_REPEAT_ACCELERATION_STEP_MS;

                }
                else
                {

                    button_context[button].repeat_interval =
                            BUTTON_REPEAT_INTERVAL_FAST_MS;

                }

            }

        }


    }

}




      /******************************************************************************
             *                         Event Generation
             ******************************************************************************/

            /**
             * @brief
             *      Generate button event.
             *
             * @param button
             *      Source button.
             *
             * @param type
             *      Event type.
             *
             */
            static void Button_GenerateEvent(Button_Id_t button,
                                             Button_EventType_t type)
            {

                Button_Event_t event;



                /*
                 * Prepare event packet.
                 */
                event.button = button;


                event.event = type;



                /*
                 * Store event.
                 */
                Button_QueuePush(&event);

            }





            /******************************************************************************
             *                         FIFO Queue
             ******************************************************************************/

            /**
             * @brief
             *      Add event to queue.
             *
             */
            static bool Button_QueuePush(const Button_Event_t *event)
            {

                if(event == NULL)
                {
                    return false;
                }



                /*
                 * Queue full.
                 */
                if(button_queue.count >= BUTTON_EVENT_QUEUE_SIZE)
                {
                    return false;
                }



                /*
                 * Store event.
                 */
                button_queue.buffer[button_queue.head] =
                        *event;



                /*
                 * Advance head.
                 */
                button_queue.head++;



                if(button_queue.head >= BUTTON_EVENT_QUEUE_SIZE)
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
             *      Remove oldest event.
             *
             */
            static bool Button_QueuePop(Button_Event_t *event)
            {

                if(event == NULL)
                {
                    return false;
                }



                /*
                 * Empty queue.
                 */
                if(button_queue.count == 0U)
                {
                    return false;
                }



                /*
                 * Read event.
                 */
                *event =
                    button_queue.buffer[button_queue.tail];



                /*
                 * Move tail.
                 */
                button_queue.tail++;



                if(button_queue.tail >= BUTTON_EVENT_QUEUE_SIZE)
                {
                    button_queue.tail = 0U;
                }



                /*
                 * Decrease count.
                 */
                button_queue.count--;



                return true;

            }





            /******************************************************************************
             *                              End Of File
             ******************************************************************************/

            /*
             *
             * Repeat Behavior Summary:
             *
             *
             * UP / DOWN:
             *
             *
             * Press:
             *
             *      BUTTON_EVENT_PRESS
             *
             *
             * Hold 600ms:
             *
             *      Start repeat
             *
             *
             * Repeat:
             *
             *      300ms interval
             *
             *
             * Hold 3 seconds:
             *
             *      Acceleration starts
             *
             *
             * Interval:
             *
             *      300
             *      250
             *      200
             *      150
             *      100 ms
             *
             *
             *
             * ENTER / BACK:
             *
             *      Press only
             *
             *
             ******************************************************************************/
            /******************************************************************************
             *
             *                      Compatibility Notes
             *
             ******************************************************************************/

            /*
             *
             * Version 2.3.0 maintains compatibility with:
             *
             *
             *      menu_controller.c
             *
             *      menu_renderer.c
             *
             *      application event handler
             *
             *
             * Existing code:
             *
             *
             *      if(Buttons_GetEvent(&event))
             *      {
             *
             *          switch(event.button)
             *          {
             *
             *              case BUTTON_ID_UP:
             *
             *                  MenuController_MoveUp();
             *
             *                  break;
             *
             *
             *              case BUTTON_ID_DOWN:
             *
             *                  MenuController_MoveDown();
             *
             *                  break;
             *
             *          }
             *
             *      }
             *
             *
             * remains valid.
             *
             *
             *
             * New repeat events are handled by checking:
             *
             *
             *      event.event
             *
             *
             * Example:
             *
             *
             *      if(event.event == BUTTON_EVENT_REPEAT)
             *      {
             *
             *          MenuController_MoveDown();
             *
             *      }
             *
             *
             ******************************************************************************/




            /******************************************************************************
             *
             *                      Design Validation
             *
             ******************************************************************************/

            /*
             *
             * Non blocking:
             *
             *      YES
             *
             *
             * HAL_Delay usage:
             *
             *      NONE
             *
             *
             * UART dependency:
             *
             *      NONE
             *
             *
             * LCD dependency:
             *
             *      NONE
             *
             *
             * Hardware dependency:
             *
             *      Only GPIO mapping table
             *
             *
             *
             * Supported buttons:
             *
             *
             *      UP
             *          Press
             *          Long press
             *          Repeat
             *          Acceleration
             *
             *
             *      DOWN
             *          Press
             *          Long press
             *          Repeat
             *          Acceleration
             *
             *
             *      ENTER
             *          Press only
             *
             *
             *      BACK
             *          Press only
             *
             *
             ******************************************************************************/




            /******************************************************************************
             *
             *                              End Of File
             *
             ******************************************************************************/
