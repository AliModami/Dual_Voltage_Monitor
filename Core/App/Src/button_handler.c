/******************************************************************************
 *
 * @file    button_handler.c
 *
 * @brief   Menu Button Event Handler Implementation
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor - Page Based Menu Prototype
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file converts button events into menu controller commands.
 *
 *      Supported commands:
 *
 *      BUTTON_EVENT_UP
 *          |
 *          v
 *      MenuController_MoveUp()
 *
 *
 *      BUTTON_EVENT_DOWN
 *          |
 *          v
 *      MenuController_MoveDown()
 *
 *
 *      BUTTON_EVENT_ENTER
 *          |
 *          v
 *      MenuController_Enter()
 *
 *
 *      BUTTON_EVENT_BACK
 *          |
 *          v
 *      MenuController_Back()
 *
 *
 *      This module does NOT access:
 *
 *      - GPIO
 *      - EXTI
 *      - LCD
 *
 ******************************************************************************/

#include "button_handler.h"

#include "menu_controller.h"



/*
 * --------------------------------------------------------------------------
 * Private variables
 * --------------------------------------------------------------------------
 */


/*
 * Button handler initialized flag.
 *
 * Reserved for future extensions:
 *
 * - Debounce control
 * - Long press detection
 * - Repeat key
 */
static uint8_t button_handler_initialized = 0U;



/*
 * --------------------------------------------------------------------------
 * Public Functions
 * --------------------------------------------------------------------------
 */


/*
 * Initialize button handler.
 */
void ButtonHandler_Init(void)
{
    button_handler_initialized = 1U;
}



/*
 * Process button event.
 */
void ButtonHandler_Process(ButtonEvent_t event)
{

    /*
     * Ignore events before initialization.
     */
    if (button_handler_initialized == 0U)
    {
        return;
    }



    switch(event)
    {

        /*
         * Move cursor upward.
         */
        case BUTTON_EVENT_UP:

            MenuController_MoveUp();

            break;



        /*
         * Move cursor downward.
         */
        case BUTTON_EVENT_DOWN:

            MenuController_MoveDown();

            break;



        /*
         * Select current item.
         */
        case BUTTON_EVENT_ENTER:

            MenuController_Enter();

            break;



        /*
         * Return to parent page.
         */
        case BUTTON_EVENT_BACK:

            MenuController_Back();

            break;



        /*
         * No action.
         */
        case BUTTON_EVENT_NONE:


            break;



        default:


            break;

    }

}
