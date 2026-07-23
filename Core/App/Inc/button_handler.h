/******************************************************************************
 *
 * @file    button_handler.h
 *
 * @brief   Menu Button Event Handler Interface
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
 *      This file defines the button abstraction layer used
 *      to control the menu system.
 *
 *      Responsibilities:
 *
 *      - Define button events.
 *      - Receive button events from hardware layer.
 *      - Convert events into menu commands.
 *
 *      This module does NOT handle:
 *
 *      - GPIO hardware access
 *      - Debouncing hardware
 *      - LCD display
 *
 ******************************************************************************/

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>



/*
 * Button event definition.
 *
 * The hardware layer should translate
 * physical buttons into these events.
 *
 * Example:
 *
 * GPIO Button UP pressed
 *
 *        |
 *        v
 *
 * BUTTON_EVENT_UP
 */
typedef enum
{

    /*
     * No button event.
     */
    BUTTON_EVENT_NONE = 0,


    /*
     * Cursor movement upward.
     */
    BUTTON_EVENT_UP,


    /*
     * Cursor movement downward.
     */
    BUTTON_EVENT_DOWN,


    /*
     * Enter menu / select item.
     */
    BUTTON_EVENT_ENTER,


    /*
     * Return to previous menu.
     */
    BUTTON_EVENT_BACK,


    /*
     * Number of button events.
     */
    BUTTON_EVENT_COUNT


} ButtonEvent_t;



/*
 * Initialize button handler.
 *
 * Must be called once during system startup.
 */
void ButtonHandler_Init(void);



/*
 * Process a button event.
 *
 * This function receives an event from
 * the hardware button driver and forwards
 * it to menu_controller.
 *
 * Example:
 *
 * ButtonHandler_Process(BUTTON_EVENT_UP);
 *
 */
void ButtonHandler_Process(ButtonEvent_t event);



#ifdef __cplusplus
}
#endif


#endif /* BUTTON_HANDLER_H */
