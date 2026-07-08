/******************************************************************************
 * @file    buttons.c
 * @brief   Professional Button Driver Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file implements the button driver module.
 *
 *      The driver is responsible for:
 *
 *          - Reading button hardware
 *          - Software debounce processing
 *          - Button state management
 *          - Event generation
 *          - Event buffering
 *
 *      Application modules must never access button GPIO directly.
 *      They receive button actions through the public API defined in
 *      buttons.h.
 *
 *-----------------------------------------------------------------------------
 * Design:
 *
 *      GPIO Input
 *          |
 *          v
 *      Button State Machine
 *          |
 *          v
 *      Event Generator
 *          |
 *          v
 *      Event Queue
 *          |
 *          v
 *      Application
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 * Created :
 *      2026-07-08
 ******************************************************************************/


/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "buttons.h"

#include "main.h"
#include <string.h>



/******************************************************************************
 *                         Private Constants
 ******************************************************************************/

/*
 * Button timing parameters.
 *
 * These values are kept here instead of being written directly inside
 * functions.
 *
 * This makes the driver easier to tune during testing.
 */


/*
 * Minimum stable time required for a button state transition.
 *
 * This prevents mechanical contact bouncing from generating
 * multiple false events.
 */

#define BUTTON_DEBOUNCE_TIME_MS          30U



/*
 * Time required to generate a Long Press event.
 */

#define BUTTON_LONG_PRESS_TIME_MS       800U



/*
 * Delay before Auto Repeat starts after Long Press.
 */

#define BUTTON_REPEAT_START_TIME_MS     800U



/*
 * Period between Auto Repeat events.
 */

#define BUTTON_REPEAT_PERIOD_MS         200U



/*
 * Number of events stored inside the internal FIFO queue.
 */

#define BUTTON_EVENT_QUEUE_SIZE          16U



/******************************************************************************
 *                         Private Types
 ******************************************************************************/


/**
 * @brief
 *      Internal button processing state.
 *
 * The application never sees this state.
 * It is only used by the driver state machine.
 */

typedef enum
{
    BUTTON_STATE_RELEASED = 0,

    BUTTON_STATE_DEBOUNCE_PRESS,

    BUTTON_STATE_PRESSED,

    BUTTON_STATE_LONG_PRESS,

    BUTTON_STATE_DEBOUNCE_RELEASE

} Button_State_t;



/**
 * @brief
 *      Hardware information for each button.
 *
 * Logical button IDs are separated from physical GPIO pins.
 *
 * This allows changing hardware wiring without modifying
 * application code.
 */

typedef struct
{
    GPIO_TypeDef *port;

    uint16_t pin;

} Button_Hardware_t;



/**
 * @brief
 *      Runtime context for each button.
 *
 * Every button owns an independent context.
 */

typedef struct
{
    Button_State_t state;


    /*
     * Timestamp of last state change.
     */

    uint32_t timestamp;


    /*
     * Timestamp used for repeat generation.
     */

    uint32_t repeat_timestamp;


    /*
     * Indicates whether Long Press was already generated.
     */

    bool long_press_sent;


} Button_Context_t;



/**
 * @brief
 *      Internal FIFO queue for button events.
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
 * Hardware mapping table.
 *
 * The application works with logical button IDs:
 *
 *      BUTTON_ID_UP
 *      BUTTON_ID_DOWN
 *      BUTTON_ID_ENTER
 *      BUTTON_ID_BACK
 *
 * This table connects those logical IDs to the physical STM32 GPIO pins.
 *
 * Current hardware configuration:
 *
 *      UP       -> PB14
 *      DOWN     -> PB13
 *      ENTER    -> PB12
 *      BACK     -> PB15
 *
 * Buttons are configured with internal/external pull-up resistors.
 *
 * Therefore:
 *
 *      GPIO_PIN_RESET  = Button Pressed
 *      GPIO_PIN_SET    = Button Released
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
 * Runtime context of every button.
 *
 * Each button has its own independent state machine.
 */

static Button_Context_t button_context[BUTTON_ID_COUNT];



/*
 * Internal event FIFO queue.
 *
 * Button events are stored here until the application reads them.
 */

static Button_EventQueue_t button_queue;



/******************************************************************************
 *                      Private Function Prototypes
 ******************************************************************************/

/**
 * @brief
 *      Read current physical button level.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *      true:
 *          Button is pressed.
 *
 *      false:
 *          Button is released.
 */
static bool Button_Read(Button_Id_t button);



/**
 * @brief
 *      Process one button state machine.
 *
 * @param button
 *      Logical button identifier.
 */
static void Button_Process(Button_Id_t button);



/**
 * @brief
 *      Add new event to FIFO queue.
 *
 * @param event
 *      Button event to store.
 *
 * @return
 *      true:
 *          Event stored successfully.
 *
 *      false:
 *          Queue is full.
 */
static bool Button_QueuePush(Button_Event_t event);



/**
 * @brief
 *      Remove event from FIFO queue.
 *
 * @param event
 *      Destination event structure.
 *
 * @return
 *      true:
 *          Event received.
 *
 *      false:
 *          Queue is empty.
 */
static bool Button_QueuePop(Button_Event_t *event);



/**
 * @brief
 *      Generate a button event.
 *
 * @param button
 *      Button identifier.
 *
 * @param event
 *      Event type.
 */
static void Button_GenerateEvent(Button_Id_t button,
                                 Button_EventType_t event);



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/

/******************************************************************************
 *                         Public Functions
 ******************************************************************************/


/**
 * @brief
 *      Initialize button driver.
 *
 * @details
 *
 *      This function initializes all internal button states.
 *
 *      GPIO initialization is not performed here.
 *
 *      GPIO pins must already be configured by STM32CubeMX.
 *
 *      Because the hardware uses pull-up configuration:
 *
 *          Released  -> GPIO_SET
 *          Pressed   -> GPIO_RESET
 *
 */
void Buttons_Init(void)
{
    uint8_t index;


    /*
     * Clear all button runtime contexts.
     *
     * memset is used because the context structure contains
     * multiple state variables that must start from a known value.
     */

    memset(button_context,
           0,
           sizeof(button_context));



    /*
     * Initialize every button state machine.
     */

    for(index = 0U;
        index < BUTTON_ID_COUNT;
        index++)
    {
        button_context[index].state = BUTTON_STATE_RELEASED;

        button_context[index].timestamp = HAL_GetTick();

        button_context[index].repeat_timestamp = 0U;

        button_context[index].long_press_sent = false;
    }



    /*
     * Initialize event queue.
     */

    memset(&button_queue,
           0,
           sizeof(button_queue));
}



/**
 * @brief
 *      Execute button processing task.
 *
 * @details
 *
 *      This function must be called continuously
 *      from the main loop.
 *
 *      Example:
 *
 *          while(1)
 *          {
 *              Buttons_Task();
 *          }
 *
 *
 *      This function performs:
 *
 *          - Hardware reading
 *          - Debounce processing
 *          - State machine update
 *          - Event generation
 *
 */
void Buttons_Task(void)
{
    uint8_t index;


    /*
     * Process each button independently.
     *
     * Each button has its own state machine.
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
 *      Pointer to destination event structure.
 *
 * @return
 *
 *      true:
 *          Event received successfully.
 *
 *      false:
 *          No event available.
 */
bool Buttons_GetEvent(Button_Event_t *event)
{
    if(event == NULL)
    {
        return false;
    }


    return Button_QueuePop(event);
}



/******************************************************************************
 *                         Private Functions
 ******************************************************************************/


/**
 * @brief
 *      Read button hardware state.
 *
 * @param button
 *      Logical button identifier.
 *
 * @return
 *
 *      true:
 *          Button is physically pressed.
 *
 *      false:
 *          Button is released.
 */
static bool Button_Read(Button_Id_t button)
{
    GPIO_PinState pin_state;


    pin_state = HAL_GPIO_ReadPin(button_hardware[button].port,
                                 button_hardware[button].pin);



    /*
     * Buttons are active low because they use pull-up resistors.
     */

    if(pin_state == GPIO_PIN_RESET)
    {
        return true;
    }
    else
    {
        return false;
    }
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
 *      Each button is processed independently.
 *
 *      State flow:
 *
 *          RELEASED
 *              |
 *              | Button Press
 *              v
 *          DEBOUNCE_PRESS
 *              |
 *              | Stable Press
 *              v
 *          PRESSED
 *              |
 *              | Long Time
 *              v
 *          LONG_PRESS
 *              |
 *              | Button Release
 *              v
 *          DEBOUNCE_RELEASE
 *              |
 *              | Stable Release
 *              v
 *          RELEASED
 *
 */
static void Button_Process(Button_Id_t button)
{
    uint32_t now;

    bool pressed;



    now = HAL_GetTick();


    pressed = Button_Read(button);



    switch(button_context[button].state)
    {


        case BUTTON_STATE_RELEASED:


            if(pressed)
            {
                /*
                 * Possible button press detected.
                 *
                 * Start debounce timing.
                 */

                button_context[button].timestamp = now;


                button_context[button].state =
                        BUTTON_STATE_DEBOUNCE_PRESS;
            }


            break;



        case BUTTON_STATE_DEBOUNCE_PRESS:


            if(!pressed)
            {
                /*
                 * Noise detected.
                 *
                 * Return to stable released state.
                 */

                button_context[button].state =
                        BUTTON_STATE_RELEASED;
            }
            else if((now - button_context[button].timestamp)
                    >= BUTTON_DEBOUNCE_TIME_MS)
            {
                /*
                 * Button press confirmed.
                 */

                Button_GenerateEvent(button,
                                     BUTTON_EVENT_PRESS);


                button_context[button].timestamp = now;


                button_context[button].repeat_timestamp = now;


                button_context[button].long_press_sent = false;


                button_context[button].state =
                        BUTTON_STATE_PRESSED;
            }


            break;



        case BUTTON_STATE_PRESSED:


            if(!pressed)
            {
                /*
                 * Button released before long press.
                 */

                button_context[button].timestamp = now;


                button_context[button].state =
                        BUTTON_STATE_DEBOUNCE_RELEASE;
            }
            else
            {
                /*
                 * Check Long Press condition.
                 */

                if((!button_context[button].long_press_sent) &&
                   ((now - button_context[button].timestamp)
                    >= BUTTON_LONG_PRESS_TIME_MS))
                {
                    Button_GenerateEvent(button,
                                         BUTTON_EVENT_LONG_PRESS);


                    button_context[button].long_press_sent = true;


                    button_context[button].repeat_timestamp = now;


                    button_context[button].state =
                            BUTTON_STATE_LONG_PRESS;
                }
            }


            break;



        case BUTTON_STATE_LONG_PRESS:


            if(!pressed)
            {
                /*
                 * Start release debounce.
                 */

                button_context[button].timestamp = now;


                button_context[button].state =
                        BUTTON_STATE_DEBOUNCE_RELEASE;
            }
            else
            {
                /*
                 * Generate Auto Repeat events.
                 */

                if((now - button_context[button].repeat_timestamp)
                    >= BUTTON_REPEAT_PERIOD_MS)
                {
                    Button_GenerateEvent(button,
                                         BUTTON_EVENT_REPEAT);


                    button_context[button].repeat_timestamp = now;
                }
            }


            break;



        case BUTTON_STATE_DEBOUNCE_RELEASE:


            if(pressed)
            {
                /*
                 * False release caused by bouncing.
                 */

                button_context[button].state =
                        BUTTON_STATE_PRESSED;
            }
            else if((now - button_context[button].timestamp)
                    >= BUTTON_DEBOUNCE_TIME_MS)
            {
                /*
                 * Release confirmed.
                 */

                Button_GenerateEvent(button,
                                     BUTTON_EVENT_RELEASE);


                button_context[button].state =
                        BUTTON_STATE_RELEASED;
            }


            break;



        default:


            /*
             * Safety recovery.
             */

            button_context[button].state =
                    BUTTON_STATE_RELEASED;


            break;
    }
}



/**
 * @brief
 *      Generate and store a button event.
 *
 * @param button
 *      Button identifier.
 *
 * @param event
 *      Event type.
 */
static void Button_GenerateEvent(Button_Id_t button,
                                 Button_EventType_t event)
{
    Button_Event_t new_event;


    new_event.button = button;

    new_event.event = event;


    Button_QueuePush(new_event);
}

/**
 * @brief
 *      Add event into button FIFO queue.
 *
 * @param event
 *      Button event to store.
 *
 * @return
 *
 *      true:
 *          Event stored successfully.
 *
 *      false:
 *          Queue is full.
 *
 * @details
 *
 *      The queue prevents losing button events when the application
 *      cannot process them immediately.
 *
 *      Example:
 *
 *          Button pressed
 *                |
 *                v
 *          Event Queue
 *                |
 *                v
 *          Application reads event later
 *
 */
static bool Button_QueuePush(Button_Event_t event)
{
    if(button_queue.count >= BUTTON_EVENT_QUEUE_SIZE)
    {
        /*
         * Queue overflow.
         *
         * The oldest events are preserved.
         * New event is discarded.
         */

        return false;
    }



    /*
     * Store new event at current head position.
     */

    button_queue.buffer[button_queue.head] = event;



    /*
     * Move head index forward.
     *
     * Circular buffer operation.
     */

    button_queue.head++;


    if(button_queue.head >= BUTTON_EVENT_QUEUE_SIZE)
    {
        button_queue.head = 0U;
    }



    button_queue.count++;



    return true;
}





/**
 * @brief
 *      Remove event from button FIFO queue.
 *
 * @param event
 *      Destination event structure.
 *
 * @return
 *
 *      true:
 *          Event successfully received.
 *
 *      false:
 *          Queue is empty.
 */
static bool Button_QueuePop(Button_Event_t *event)
{
    if(event == NULL)
    {
        return false;
    }



    if(button_queue.count == 0U)
    {
        return false;
    }



    /*
     * Read oldest event.
     */

    *event = button_queue.buffer[button_queue.tail];



    /*
     * Move tail index forward.
     */

    button_queue.tail++;


    if(button_queue.tail >= BUTTON_EVENT_QUEUE_SIZE)
    {
        button_queue.tail = 0U;
    }



    button_queue.count--;



    return true;
}



/******************************************************************************
 *                              End of File
 ******************************************************************************/
