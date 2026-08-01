/******************************************************************************
 *
 * @file    factory_confirm.c
 *
 * @brief   Factory Default Confirmation Service Implementation
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
 *      This module handles user confirmation before executing
 *      Factory Default operation.
 *
 *
 *      Responsibilities:
 *
 *          - Display confirmation screen.
 *          - Process confirmation button events.
 *          - Provide confirmation result.
 *          - Execute as non-blocking state machine.
 *
 *
 *      This module does NOT handle:
 *
 *          - Configuration reset.
 *          - Flash operations.
 *          - Factory default execution.
 *          - Menu navigation.
 *          - Button FIFO reading.
 *          - LCD low level communication.
 *
 *
 *      Button ownership:
 *
 *          main.c
 *              |
 *              v
 *          Buttons_GetEvent()
 *              |
 *              v
 *          FactoryConfirm_ProcessButton()
 *
 *
 ******************************************************************************/

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "factory_confirm.h"

#include "lcd_display.h"
#include "screen_manager.h"
#include "factory_default.h"





/******************************************************************************
 *                              Private Variables
 ******************************************************************************/

/*
 * Current confirmation state.
 */
static FactoryConfirmState_t confirm_state =
        FACTORY_CONFIRM_IDLE;







/******************************************************************************
 *                              Private Functions
 ******************************************************************************/

/*
 * Display confirmation screen.
 */
static void FactoryConfirm_ShowScreen(void);




/******************************************************************************
 *                              Public Functions
 ******************************************************************************/




/*
 * Initialize Factory Confirmation module.
 */
void FactoryConfirm_Init(void)
{

    confirm_state =
            FACTORY_CONFIRM_IDLE;

}






/*
 * Start confirmation process.
 *
 * Display:
 *
 *      Factory Default?
 *
 *      ENTER = YES
 *      BACK  = NO
 *
 */
void FactoryConfirm_Start(void)
{

    confirm_state =
            FACTORY_CONFIRM_ACTIVE;



    FactoryConfirm_ShowScreen();

}






/*
 * Process button event.
 *
 * Button events are received
 * from main.c.
 *
 * This function does NOT access
 * button FIFO directly.
 */
void FactoryConfirm_ProcessButton(
        const Button_Event_t *event)
{

    if(event == NULL)
    {
        return;
    }




    /*
     * Ignore non press events.
     */
    if(event->event != BUTTON_EVENT_PRESS)
    {
        return;
    }





    /*
     * ENTER confirms operation.
     */
    if(event->button == BUTTON_ID_ENTER)
    {

        confirm_state =
                FACTORY_CONFIRM_ACCEPTED;

    }






    /*
     * BACK cancels operation.
     */
    else if(event->button == BUTTON_ID_BACK)
    {

        confirm_state =
                FACTORY_CONFIRM_CANCELLED;

    }


}

/*
 * Periodic confirmation task.
 *
 * This function intentionally
 * does not process buttons.
 *
 * Button events are handled by:
 *
 *      main.c
 *          |
 *          v
 *      FactoryConfirm_ProcessButton()
 *
 *
 * This task remains available
 * for future non-blocking additions.
 */

void FactoryConfirm_Task(void)
{

    if(confirm_state == FACTORY_CONFIRM_ACCEPTED)
    {

        FactoryDefault_Start();

        confirm_state =
                FACTORY_CONFIRM_IDLE;
    }



    else if(confirm_state == FACTORY_CONFIRM_CANCELLED)
    {

        ScreenManager_SetScreen(
                SCREEN_MENU);


        confirm_state =
                FACTORY_CONFIRM_IDLE;
    }

}





/*
 * Get current confirmation state.
 */
FactoryConfirmState_t FactoryConfirm_GetState(void)
{

    return confirm_state;

}







/*
 * Reset confirmation state.
 *
 * Used after:
 *
 *      - Accepted operation
 *      - Cancelled operation
 */
void FactoryConfirm_Reset(void)
{

    confirm_state =
            FACTORY_CONFIRM_IDLE;

}







/*
 * Check whether confirmation
 * screen is active.
 *
 * Return:
 *
 *      1 = Active
 *      0 = Inactive
 */
uint8_t FactoryConfirm_IsActive(void)
{

    if(confirm_state ==
       FACTORY_CONFIRM_ACTIVE)
    {

        return 1U;

    }


    return 0U;

}







/******************************************************************************
 *                              Private Functions
 ******************************************************************************/




/*
 * Display confirmation screen.
 *
 *
 * LCD Layout:
 *
 *
 *      Row 0:
 *          Factory Default?
 *
 *      Row 1:
 *          Empty
 *
 *      Row 2:
 *          ENTER = YES
 *
 *      Row 3:
 *          BACK = NO
 *
 */
static void FactoryConfirm_ShowScreen(void)
{

    LCD_Display_Clear();




    LCD_Display_PrintLine(
            0U,
            "Factory Default?");





    LCD_Display_PrintLine(
            1U,
            "");





    LCD_Display_PrintLine(
            2U,
            "ENTER = YES");





    LCD_Display_PrintLine(
            3U,
            "BACK = NO");

}







/******************************************************************************
 *
 *                              END OF PART 2/3
 *
 ******************************************************************************/
/******************************************************************************
 *
 *                              END OF FILE
 *
 *      factory_confirm.c
 *
 *      Version:
 *
 *          Clean Final v1.0.1
 *
 *
 * Changes:
 *
 *      v1.0.1
 *
 *          - Removed direct button FIFO access.
 *
 *          - Added FactoryConfirm_ProcessButton().
 *
 *          - Moved button event ownership
 *            to main.c.
 *
 *          - Kept confirmation logic
 *            independent from button driver.
 *
 *          - Preserved non-blocking architecture.
 *
 *          - Preserved LCD confirmation display.
 *
 *
 *
 * Architecture:
 *
 *
 *
 *      buttons.c
 *
 *          |
 *          v
 *
 *      main.c
 *
 *          |
 *          v
 *
 *      FactoryConfirm_ProcessButton()
 *
 *          |
 *          v
 *
 *      factory_confirm.c
 *
 *          |
 *          v
 *
 *      FactoryDefault_Start()
 *
 *          |
 *          v
 *
 *      factory_default.c
 *
 *
 *
 *
 * Dependencies:
 *
 *      factory_confirm.h
 *      buttons.h
 *      lcd_display.h
 *
 *
 *
 * Compatible:
 *
 *      STM32F103C8T6
 *
 *      STM32 HAL
 *
 ******************************************************************************/
