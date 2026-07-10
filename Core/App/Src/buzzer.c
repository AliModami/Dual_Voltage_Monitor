/******************************************************************************
 * @file    buzzer.c
 * @brief   Buzzer control implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file implements a non-blocking buzzer driver.
 *
 *      The buzzer module uses HAL_GetTick() for timing instead of HAL_Delay().
 *
 *      This design allows the CPU to continue executing other tasks while
 *      the buzzer pattern is being generated.
 *
 *      Example:
 *
 *          - LCD refresh can continue
 *          - ADC measurement can continue
 *          - UART communication can continue
 *
 *-----------------------------------------------------------------------------
 * Design:
 *
 *      Buzzer_SetPattern()
 *              |
 *              v
 *      Store requested pattern
 *              |
 *              v
 *      Buzzer_Task()
 *              |
 *              v
 *      Execute state machine using HAL_GetTick()
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

#include "buzzer.h"
#include "main.h"
#include <stdbool.h>



/******************************************************************************
 *                         Private Constants
 ******************************************************************************/

/*
 * Timing values are defined here instead of being written directly inside
 * functions.
 *
 * This improves readability and allows easy adjustment during testing.
 */

#define BUZZER_SHORT_TIME_MS          100U

#define BUZZER_LONG_TIME_MS           800U

#define BUZZER_PAUSE_TIME_MS          150U

#define BUZZER_ERROR_PAUSE_MS         500U



/******************************************************************************
 *                         Private Variables
 ******************************************************************************/

/*
 * Current requested buzzer pattern.
 *
 * This variable is private to this module.
 * Other modules communicate through Buzzer_SetPattern().
 */

static Buzzer_Pattern_t current_pattern = BUZZER_OFF;


/*
 * Current execution step of the buzzer state machine.
 */

static uint8_t buzzer_step = 0U;


/*
 * Timestamp of the last buzzer state change.
 */

static uint32_t buzzer_timestamp = 0U;


/*
 * Indicates whether a pattern is currently running.
 */

static bool buzzer_active = false;



/******************************************************************************
 *                         Private Functions
 ******************************************************************************/

/**
 * @brief
 *      Turn buzzer hardware ON.
 */
static void Buzzer_HardwareOn(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port,
                      BUZZER_Pin,
                      GPIO_PIN_SET);
}



/**
 * @brief
 *      Turn buzzer hardware OFF.
 */
static void Buzzer_HardwareOff(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port,
                      BUZZER_Pin,
                      GPIO_PIN_RESET);
}



/**
 * @brief
 *      Start a new buzzer timing sequence.
 */
static void Buzzer_Start(void)
{
    buzzer_timestamp = HAL_GetTick();

    buzzer_step = 0U;

    buzzer_active = true;
}



/******************************************************************************
 *                         Public Functions
 ******************************************************************************/


/**
 * @brief
 *      Initialize buzzer module.
 */
void Buzzer_Init(void)
{
    Buzzer_HardwareOff();

    current_pattern = BUZZER_OFF;

    buzzer_active = false;

    buzzer_step = 0U;
}



/**
 * @brief
 *      Select buzzer pattern.
 *
 * @param pattern
 *      Requested buzzer notification pattern.
 *
 * @details
 *      This function does not block program execution.
 *      Actual sound generation is handled by Buzzer_Task().
 */
void Buzzer_SetPattern(Buzzer_Pattern_t pattern)
{
    current_pattern = pattern;

    if(pattern == BUZZER_OFF)
    {
        Buzzer_HardwareOff();

        buzzer_active = false;
    }
    else
    {
        Buzzer_Start();
    }
}



/**
 * @brief
 *      Turn buzzer off immediately.
 */
void Buzzer_Off(void)
{
    current_pattern = BUZZER_OFF;

    buzzer_active = false;

    Buzzer_HardwareOff();
}



/**
 * @brief
 *      Execute buzzer state machine.
 *
 * @details
 *      This function must be called periodically from the main loop.
 *
 *      Example:
 *
 *          while(1)
 *          {
 *              Buzzer_Task();
 *          }
 */
void Buzzer_Task(void)
{
    uint32_t now = HAL_GetTick();


    if(!buzzer_active)
    {
        return;
    }



    switch(current_pattern)
    {

        case BUZZER_SHORT_BEEP:

            if(buzzer_step == 0U)
            {
                Buzzer_HardwareOn();

                buzzer_timestamp = now;

                buzzer_step = 1U;
            }
            else if((now - buzzer_timestamp) >= BUZZER_SHORT_TIME_MS)
            {
                Buzzer_HardwareOff();

                buzzer_active = false;
            }

            break;



        case BUZZER_DOUBLE_BEEP:

            if(buzzer_step == 0U)
            {
                Buzzer_HardwareOn();

                buzzer_timestamp = now;

                buzzer_step = 1U;
            }
            else if(buzzer_step == 1U &&
                    (now - buzzer_timestamp) >= BUZZER_SHORT_TIME_MS)
            {
                Buzzer_HardwareOff();

                buzzer_timestamp = now;

                buzzer_step = 2U;
            }
            else if(buzzer_step == 2U &&
                    (now - buzzer_timestamp) >= BUZZER_PAUSE_TIME_MS)
            {
                Buzzer_HardwareOn();

                buzzer_timestamp = now;

                buzzer_step = 3U;
            }
            else if(buzzer_step == 3U &&
                    (now - buzzer_timestamp) >= BUZZER_SHORT_TIME_MS)
            {
                Buzzer_HardwareOff();

                buzzer_active = false;
            }

            break;



        case BUZZER_LONG_BEEP:

            if(buzzer_step == 0U)
            {
                Buzzer_HardwareOn();

                buzzer_timestamp = now;

                buzzer_step = 1U;
            }
            else if((now - buzzer_timestamp) >= BUZZER_LONG_TIME_MS)
            {
                Buzzer_HardwareOff();

                buzzer_active = false;
            }

            break;



        case BUZZER_ERROR:

            /*
             * Error pattern:
             *
             * Three short beeps.
             */

            if(buzzer_step == 0U)
            {
                Buzzer_HardwareOn();

                buzzer_timestamp = now;

                buzzer_step = 1U;
            }
            else if(buzzer_step == 1U &&
                    (now - buzzer_timestamp) >= BUZZER_SHORT_TIME_MS)
            {
                Buzzer_HardwareOff();

                buzzer_timestamp = now;

                buzzer_step = 2U;
            }
            else if(buzzer_step == 2U &&
                    (now - buzzer_timestamp) >= BUZZER_PAUSE_TIME_MS)
            {
                Buzzer_HardwareOn();

                buzzer_timestamp = now;

                buzzer_step = 3U;
            }
            else if(buzzer_step == 3U &&
                    (now - buzzer_timestamp) >= BUZZER_SHORT_TIME_MS)
            {
                Buzzer_HardwareOff();

                buzzer_timestamp = now;

                buzzer_step = 4U;
            }
            else if(buzzer_step == 4U &&
                    (now - buzzer_timestamp) >= BUZZER_ERROR_PAUSE_MS)
            {
                buzzer_active = false;
            }

            break;



        default:

            Buzzer_Off();

            break;
    }
}
