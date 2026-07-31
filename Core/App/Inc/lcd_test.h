/******************************************************************************
 *
 * @file    lcd_test.h
 *
 * @brief   LCD Diagnostic Test Interface
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
 *      This module provides a diagnostic interface
 *      for testing the LCD display from Service Mode.
 *
 *
 *      Responsibilities:
 *
 *      - Start LCD test sequence.
 *      - Manage LCD test state.
 *      - Provide periodic test task.
 *      - Return test status.
 *
 *
 *      This module does NOT handle:
 *
 *      - LCD low level communication.
 *      - I2C communication.
 *      - Menu navigation.
 *      - Button processing.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Architecture:
 *
 *
 *          menu_actions.c
 *
 *                |
 *                v
 *
 *          lcd_test.c
 *
 *                |
 *                v
 *
 *          lcd_display.c
 *
 *                |
 *                v
 *
 *          lcd_i2c.c
 *
 *
 *****************************************************************************/


#ifndef LCD_TEST_H
#define LCD_TEST_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>





/******************************************************************************
 *
 * LCD Test State
 *
 *****************************************************************************/


/*
 * LCD diagnostic test states.
 *
 */
typedef enum
{

    /*
     * No test running.
     */
    LCD_TEST_IDLE = 0,


    /*
     * Display test screen.
     */
    LCD_TEST_RUNNING,


    /*
     * Test finished.
     */
    LCD_TEST_DONE


} LCD_TestState_t;






/******************************************************************************
 *
 * Initialization
 *
 *****************************************************************************/


/*
 * Initialize LCD test module.
 *
 * Must be called once during system startup.
 *
 */
void LCD_Test_Init(void);






/******************************************************************************
 *
 * Test Control
 *
 *****************************************************************************/


/*
 * Start LCD diagnostic test.
 *
 *
 * Called from:
 *
 *      MenuAction_LCDTest()
 *
 *
 * Behavior:
 *
 *      - Clears previous test state.
 *      - Starts LCD test sequence.
 *
 */
void LCD_Test_Start(void);






/*
 * Periodic LCD test task.
 *
 *
 * Must be called continuously
 * from main loop.
 *
 *
 * Example:
 *
 *
 * while(1)
 * {
 *
 *      LCD_Test_Task();
 *
 * }
 *
 */
void LCD_Test_Task(void);






/******************************************************************************
 *
 * Status Information
 *
 *****************************************************************************/


/*
 * Check if LCD test is active.
 *
 *
 * Return:
 *
 *      1:
 *          Test running
 *
 *      0:
 *          Idle
 *
 */
uint8_t LCD_Test_IsActive(void);






/*
 * Get current LCD test state.
 *
 */
LCD_TestState_t LCD_Test_GetState(void);






#ifdef __cplusplus
}
#endif


#endif /* LCD_TEST_H */


/******************************************************************************
 *
 * End Of File
 *
 *      lcd_test.h
 *
 *      Version:
 *
 *          Clean Final v1.0.0
 *
 *****************************************************************************/
/******************************************************************************
 *
 *                              Interface Notes
 *
 *****************************************************************************/


/*
 *
 * LCD Test Execution Flow:
 *
 *
 *
 *      Service Mode
 *
 *           |
 *           |
 *           | ENTER
 *           v
 *
 *      MenuAction_LCDTest()
 *
 *           |
 *           v
 *
 *      LCD_Test_Start()
 *
 *           |
 *           v
 *
 *      LCD_Test_Task()
 *
 *           |
 *           v
 *
 *      lcd_display
 *
 *
 *
 *
 * Test Sequence:
 *
 *
 *      Step 1:
 *
 *          Display test title
 *
 *
 *      Step 2:
 *
 *          Display character pattern
 *
 *
 *      Step 3:
 *
 *          Display row test
 *
 *
 *      Step 4:
 *
 *          Finish test
 *
 *
 *
 */






/******************************************************************************
 *
 * Timing Rules
 *
 *****************************************************************************/


/*
 *
 * LCD test module uses
 * non-blocking timing.
 *
 *
 * Allowed:
 *
 *      HAL_GetTick()
 *
 *
 * Not allowed:
 *
 *      HAL_Delay()
 *
 *
 * Reason:
 *
 *      Service diagnostics must not
 *      stop the main application loop.
 *
 *
 */







/******************************************************************************
 *
 * Application Integration
 *
 *****************************************************************************/


/*
 *
 * main.c example:
 *
 *
 * while(1)
 * {
 *
 *      LCD_Test_Task();
 *
 * }
 *
 *
 *
 * menu_actions.c:
 *
 *
 * void MenuAction_LCDTest(void)
 * {
 *
 *      LCD_Test_Start();
 *
 * }
 *
 *
 */






/******************************************************************************
 *
 * Dependency Rules
 *
 *****************************************************************************/


/*
 *
 * Required modules:
 *
 *
 *      lcd_display.h
 *
 *
 *
 * Forbidden dependencies:
 *
 *
 *      menu_controller.h
 *
 *      button.h
 *
 *      i2c driver
 *
 *      hardware GPIO
 *
 *
 *
 */






/******************************************************************************
 *
 * Design Verification
 *
 *****************************************************************************/


/*
 *
 * Compatibility checklist:
 *
 *
 *  [OK] Independent diagnostic module.
 *
 *  [OK] No menu dependency.
 *
 *  [OK] No button dependency.
 *
 *  [OK] No I2C dependency.
 *
 *  [OK] Non-blocking design.
 *
 *  [OK] Compatible with Service Mode.
 *
 *  [OK] Compatible with lcd_display layer.
 *
 *
 */





/******************************************************************************
 *
 * End Of File
 *
 *****************************************************************************/
