/******************************************************************************
 *
 * @file    factory_default.c
 *
 * @brief   Factory Default Service Implementation
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
 *      This module executes Factory Default operation.
 *
 *
 *      Responsibilities:
 *
 *          - Restore application configuration.
 *          - Save configuration into Flash.
 *          - Display operation result.
 *          - Provide non-blocking execution.
 *
 *
 *      This module does NOT handle:
 *
 *          - User confirmation.
 *          - Button processing.
 *          - Menu navigation.
 *          - Screen transitions.
 *
 *
 ******************************************************************************/


/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "factory_default.h"

#include "config.h"

#include "screen_manager.h"




/******************************************************************************
 *                              Private Variables
 ******************************************************************************/


/*
 * Current Factory Default state.
 */
static FactoryDefaultState_t factory_state =
        FACTORY_DEFAULT_IDLE;



/*
 * Prevent repeated execution.
 */
static uint8_t factory_executed = 0U;







/******************************************************************************
 *                              Private Functions
 ******************************************************************************/


/*
 * Execute restore operation.
 */
static void FactoryDefault_Execute(void);




/******************************************************************************
 *                              Public Functions
 ******************************************************************************/



/*
 * Initialize Factory Default module.
 */
void FactoryDefault_Init(void)
{

    factory_state =
            FACTORY_DEFAULT_IDLE;


    factory_executed =
            0U;

}






/*
 * Start Factory Default execution.
 *
 * This function is called after
 * user confirmation.
 */
void FactoryDefault_Start(void)
{

    factory_state =
            FACTORY_DEFAULT_RUNNING;


    factory_executed =
            0U;


    //LCD_Display_Clear();

}






/*
 * Periodic Factory Default task.
 *
 * Must be called from main loop.
 *
 * Non-blocking.
 */
void FactoryDefault_Task(void)
{

    /*
     * Ignore inactive state.
     */
    if(factory_state != FACTORY_DEFAULT_RUNNING)
    {
        return;
    }






    /*
     * Execute operation only once.
     */
    if(factory_executed == 0U)
    {

        FactoryDefault_Execute();


        factory_executed = 1U;


        factory_state =
                FACTORY_DEFAULT_DONE;

    }


}






/*
 * Check whether operation is active.
 */
uint8_t FactoryDefault_IsActive(void)
{

    if(factory_state == FACTORY_DEFAULT_RUNNING)
    {
        return 1U;
    }


    return 0U;

}





/*
 * Get current state.
 */
FactoryDefaultState_t FactoryDefault_GetState(void)
{

    return factory_state;

}

/******************************************************************************
 *                              Private Functions
 ******************************************************************************/



/*
 * Execute Factory Default operation.
 *
 * This function performs:
 *
 *      - Reset configuration values.
 *      - Save configuration into Flash.
 *      - Display completion message.
 *
 */


static void FactoryDefault_Execute(void)
{

    /*
     * Restore configuration.
     */
    Config_ResetDefault();


    /*
     * Return to main menu
     * after factory operation.
     */
    ScreenManager_SetScreen(
            SCREEN_MENU);

}






/*
 * Display Factory Default result.
 *
 *
 * LCD Layout:
 *
 *
 *      Row 0:
 *          Factory Default
 *
 *      Row 1:
 *          Configuration
 *
 *      Row 2:
 *          Restored
 *
 *      Row 3:
 *          BACK = Exit
 *
 *
 */



/******************************************************************************
 *
 *                              END OF PART 2/3
 *
 ******************************************************************************/
/******************************************************************************
 *
 *                              END OF FILE
 *
 *      factory_default.c
 *
 *      Version:
 *
 *          Clean Final v1.2.0
 *
 *
 * Changes:
 *
 *      v1.2.0
 *
 *          - Removed internal timeout handling.
 *
 *          - Removed automatic screen transition.
 *
 *          - Factory Default result remains visible
 *            until application layer decides next action.
 *
 *          - Simplified state handling.
 *
 *          - Kept non-blocking execution model.
 *
 *          - Removed unnecessary HAL_GetTick dependency.
 *
 *          - Preserved separation:
 *
 *                factory_confirm.c
 *                        |
 *                        v
 *                factory_default.c
 *
 *
 *
 *      v1.1.0
 *
 *          - Removed confirmation logic.
 *
 *          - Separated user confirmation layer.
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
