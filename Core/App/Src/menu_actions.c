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


/******************************************************************************
 *                              Include Files
 *****************************************************************************/


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
 *
 *
 * This action only requests
 * a beep pattern.
 *
 *
 * Buzzer driver is responsible for:
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

}


/*
 * LCD diagnostic.
 *
 *
 * This action starts
 * the LCD hardware test routine.
 *
 *
 * The LCD test module is responsible for:
 *
 *      - Displaying test patterns
 *      - Checking LCD rows
 *      - Returning display to normal state
 *
 */
void MenuAction_LCDTest(void)
{

    ScreenManager_SetScreen(
            SCREEN_LCD_TEST);

}







/*
 * Auto calibration.
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
 *          Clean Final v1.0.3
 *
 *
 * Changes:
 *
 *      v1.0.3
 *
 *          - Preserved real Buzzer Test action.
 *
 *          - Added LCD Test module dependency.
 *
 *          - Connected MenuAction_LCDTest()
 *            to LCD_Test_Start().
 *
 *          - Kept menu action layer independent
 *            from LCD hardware implementation.
 *
 *          - No menu navigation changes.
 *
 ******************************************************************************/

