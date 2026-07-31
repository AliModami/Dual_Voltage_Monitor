/******************************************************************************
 *
 * @file    menu_actions.c
 *
 * @brief   Menu Action Implementation
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
 *      This file implements application actions
 *      called by the menu database.
 *
 *
 *      Responsibilities:
 *
 *      - Connect menu selections to application modules.
 *      - Start parameter edit operations.
 *      - Change application screens.
 *
 *
 *      This module does NOT handle:
 *
 *      - LCD rendering
 *      - Button processing
 *      - Menu navigation
 *
 ******************************************************************************/

#include "menu_actions.h"

#include "menu_edit.h"
#include "screen_manager.h"
#include "buzzer.h"


/*
 * ============================================================================
 * General Menu Actions
 * ============================================================================
 */


/*
 * Open live monitor screen.
 *
 * Flow:
 *
 *      Menu
 *        |
 *        v
 *      MenuAction_LiveMonitor()
 *        |
 *        v
 *      Screen Manager
 *
 */
void MenuAction_LiveMonitor(void)
{

    /*
     * Switch application display
     * to live monitor screen.
     */
    ScreenManager_SetScreen(
            SCREEN_LIVE_MONITOR);

}





/*
 * Start UART streaming.
 */
void MenuAction_StartStream(void)
{

    /*
     * TODO:
     *
     * Connect to UART Stream Manager.
     *
     */

}






/*
 * ============================================================================
 * Stream Settings Actions
 * ============================================================================
 */


/*
 * Edit baud rate.
 */
void MenuAction_BaudRate(void)
{

    MenuEdit_Start(
            EDIT_BAUD_RATE);

}





/*
 * Edit sample rate.
 */
void MenuAction_SampleRate(void)
{

    MenuEdit_Start(
            EDIT_SAMPLE_RATE);

}






/*
 * ============================================================================
 * Alarm Settings Actions
 * ============================================================================
 */


/*
 * Edit alarm enable state.
 */
void MenuAction_AlarmEnable(void)
{

    MenuEdit_Start(
            EDIT_ALARM_ENABLE);

}





/*
 * Edit low voltage limit.
 */
void MenuAction_LowVoltageLimit(void)
{

    MenuEdit_Start(
            EDIT_LOW_VOLTAGE_LIMIT);

}





/*
 * Edit high voltage limit.
 */
void MenuAction_HighVoltageLimit(void)
{

    MenuEdit_Start(
            EDIT_HIGH_VOLTAGE_LIMIT);

}





/*
 * Edit alarm mode.
 */
void MenuAction_AlarmMode(void)
{

    MenuEdit_Start(
            EDIT_ALARM_MODE);

}

/*
 * ============================================================================
 * Calibration Actions
 * ============================================================================
 */


/*
 * Input voltage calibration offset.
 */
void MenuAction_InputVoltageOffset(void)
{

    MenuEdit_Start(
            EDIT_INPUT_VOLTAGE_OFFSET);

}





/*
 * Output voltage calibration offset.
 */
void MenuAction_OutputVoltageOffset(void)
{

    MenuEdit_Start(
            EDIT_OUTPUT_VOLTAGE_OFFSET);

}






/*
 * ============================================================================
 * Service Mode Actions
 * ============================================================================
 */



/*
 * Buzzer diagnostic.
 */
/*
 * Buzzer diagnostic.
 *
 * This action only requests a beep pattern.
 *
 * The buzzer driver is responsible for:
 *
 *      - Timing
 *      - GPIO control
 *      - Non-blocking execution
 *
 */
void MenuAction_BuzzerTest(void)
{

    Buzzer_SetPattern(
            BUZZER_DOUBLE_BEEP);
    		//BUZZER_ERROR);
}





/*
 * LCD diagnostic.
 */
void MenuAction_LCDTest(void)
{

    /*
     * TODO:
     *
     * Connect LCD test module.
     *
     */

}





/*
 * Factory calibration.
 */
void MenuAction_AutoCalibrate(void)
{

    /*
     * TODO:
     *
     * Connect auto calibration routine.
     *
     */

}





/*
 * Restore default configuration.
 */
void MenuAction_FactoryDefault(void)
{

    /*
     * TODO:
     *
     * Connect factory default configuration reset.
     *
     */

}





/******************************************************************************
 *
 *                              END OF FILE
 *
 *      menu_actions.c
 *
 *      Version:
 *
 *          v1.0.2
 *
 *
 * Changes:
 *
 *      v1.0.2
 *
 *          - Added Screen Manager dependency.
 *          - Connected Live Monitor action.
 *          - Preserved existing edit actions.
 *          - No menu navigation changes.
 *
 ******************************************************************************/
