/******************************************************************************
 *
 * @file    factory_default.h
 *
 * @brief   Factory Default Service Module Interface
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
 *      This module provides the Factory Default service used by
 *      Service Mode.
 *
 *
 *      Responsibilities:
 *
 *          - Start factory default operation.
 *          - Request user confirmation.
 *          - Restore default configuration.
 *          - Save default configuration to Flash.
 *          - Display operation status.
 *          - Provide non-blocking task execution.
 *
 *
 *      This module does NOT handle:
 *
 *          - Menu navigation.
 *          - Button hardware processing.
 *          - LCD low level communication.
 *          - Flash driver implementation.
 *
 ******************************************************************************/


#ifndef FACTORY_DEFAULT_H
#define FACTORY_DEFAULT_H


#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>





/******************************************************************************
 *                              Type Definitions
 ******************************************************************************/


/*
 * Factory Default state machine.
 *
 * Execution flow:
 *
 *
 *      IDLE
 *        |
 *        |
 *        v
 *
 *  CONFIRMATION
 *
 *        |
 *        |
 *        +----------+
 *        |          |
 *        v          v
 *
 *    RUNNING     CANCEL
 *
 *        |
 *        |
 *        v
 *
 *      DONE
 *
 */
typedef enum
{

    /*
     * Module inactive.
     */
    FACTORY_DEFAULT_IDLE = 0,



    /*
     * Waiting for user decision.
     *
     * User actions:
     *
     *      ENTER = Confirm
     *
     *      BACK  = Cancel
     */
    FACTORY_DEFAULT_CONFIRMATION,



    /*
     * Factory restore operation
     * is being executed.
     */
    FACTORY_DEFAULT_RUNNING,



    /*
     * Operation completed.
     */
    FACTORY_DEFAULT_DONE


} FactoryDefaultState_t;





/******************************************************************************
 *                              Public Functions
 ******************************************************************************/


/*
 * Initialize Factory Default module.
 *
 * Must be called once during system startup.
 */
void FactoryDefault_Init(void);





/*
 * Start Factory Default service.
 *
 * This function does NOT immediately
 * erase configuration.
 *
 * It enters confirmation state.
 */
void FactoryDefault_Start(void);





/*
 * Confirm Factory Default operation.
 *
 * Called when user selects:
 *
 *      YES
 *
 * The module changes from:
 *
 *      CONFIRMATION
 *
 * to:
 *
 *      RUNNING
 *
 */
void FactoryDefault_Confirm(void);





/*
 * Cancel Factory Default operation.
 *
 * Called when user selects:
 *
 *      NO
 *
 * The module returns to idle state.
 */
void FactoryDefault_Cancel(void);





/*
 * Periodic Factory Default task.
 *
 * Must be called continuously from
 * the main application loop.
 *
 * Performs:
 *
 *      - Restore operation
 *      - Flash save
 *      - Status timing
 */
void FactoryDefault_Task(void);





/*
 * Check whether Factory Default
 * operation is active.
 *
 * Return:
 *
 *      1 = Active
 *      0 = Idle
 */
uint8_t FactoryDefault_IsActive(void);



/*
 * Get current Factory Default state.
 */
FactoryDefaultState_t FactoryDefault_GetState(void);




#ifdef __cplusplus
}
#endif


#endif /* FACTORY_DEFAULT_H */


/******************************************************************************
 *
 *                              END OF FILE
 *
 *      factory_default.h
 *
 *      Version:
 *
 *          Clean Final v1.1.0
 *
 ******************************************************************************/
/******************************************************************************
 *
 *      factory_default.h
 *
 *      Version:
 *
 *          Clean Final v1.1.0
 *
 *
 * Change History:
 *
 *      v1.1.0
 *
 *          - Added user confirmation state.
 *
 *          - Added FACTORY_DEFAULT_CONFIRMATION state.
 *
 *          - Added FactoryDefault_Confirm().
 *
 *          - Added FactoryDefault_Cancel().
 *
 *          - Factory Default operation no longer
 *            executes immediately after menu selection.
 *
 *          - User confirmation is now required:
 *
 *                ENTER = YES
 *
 *                BACK  = NO
 *
 *
 *      v1.0.0
 *
 *          - Initial Factory Default service module.
 *
 *
 ******************************************************************************/




/******************************************************************************
 *
 * Architecture
 *
 ******************************************************************************/

/*
 *
 * Updated execution flow:
 *
 *
 *
 *          Service Mode Menu
 *
 *                 |
 *                 |
 *                 v
 *
 *        MenuAction_FactoryDefault()
 *
 *                 |
 *                 |
 *                 v
 *
 *        FactoryDefault_Start()
 *
 *                 |
 *                 |
 *                 v
 *
 *      FACTORY_DEFAULT_CONFIRMATION
 *
 *                 |
 *                 |
 *          +------+------+
 *          |             |
 *          |             |
 *       ENTER          BACK
 *          |             |
 *          |             |
 *          v             v
 *
 *     FactoryDefault   Cancel
 *        Confirm()
 *
 *          |
 *          |
 *          v
 *
 *   FACTORY_DEFAULT_RUNNING
 *
 *          |
 *          |
 *          v
 *
 *   FactoryDefault_Task()
 *
 *          |
 *          |
 *          +----------------+
 *          |                |
 *          v                v
 *
 *    Config_ResetDefault()  Config_Save()
 *
 *          |
 *          |
 *          v
 *
 *   FACTORY_DEFAULT_DONE
 *
 *
 *
 */





/******************************************************************************
 *
 * Integration Notes
 *
 ******************************************************************************/

/*
 *
 * This module remains independent from:
 *
 *      - Menu Controller
 *      - Menu Renderer
 *      - LCD Driver
 *      - Button Driver
 *
 *
 * The application layer is responsible for:
 *
 *      - Sending confirmation commands.
 *
 *
 * Example:
 *
 *
 * ENTER button:
 *
 *
 *      if(FactoryDefault_GetState()
 *             == FACTORY_DEFAULT_CONFIRMATION)
 *      {
 *
 *          FactoryDefault_Confirm();
 *
 *      }
 *
 *
 *
 * BACK button:
 *
 *
 *      if(FactoryDefault_GetState()
 *             == FACTORY_DEFAULT_CONFIRMATION)
 *      {
 *
 *          FactoryDefault_Cancel();
 *
 *      }
 *
 *
 */





/******************************************************************************
 *
 * Compatibility Checklist
 *
 ******************************************************************************/

/*
 *
 * [OK] Existing Config API preserved.
 *
 * [OK] No direct Flash access.
 *
 * [OK] No direct LCD hardware access.
 *
 * [OK] Non-blocking design preserved.
 *
 * [OK] Suitable for STM32F103C8T6.
 *
 * [OK] Compatible with Service Mode architecture.
 *
 * [OK] Ready for factory_default.c v1.1.0.
 *
 */





/******************************************************************************
 *
 * End Of File
 *
 *      factory_default.h
 *
 * Version:
 *
 *      Clean Final v1.1.0
 *
 ******************************************************************************/
