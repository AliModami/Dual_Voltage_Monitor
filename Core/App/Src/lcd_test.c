/******************************************************************************
 *
 * @file    lcd_test.c
 *
 * @brief   LCD Diagnostic Test Implementation
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
 *      This module implements the LCD diagnostic test
 *      used by Service Mode.
 *
 *
 *      The module provides:
 *
 *          - Test start control.
 *          - Non-blocking test sequence.
 *          - LCD display verification.
 *
 *
 *      This module does NOT handle:
 *
 *          - Menu navigation.
 *          - Button input.
 *          - LCD low level communication.
 *          - I2C communication.
 *
 ******************************************************************************/

/******************************************************************************
 *                              Include Files
 *****************************************************************************/

#include "lcd_test.h"

#include "lcd_display.h"

#include "stm32f1xx_hal.h"






/******************************************************************************
 *
 * Private Definitions
 *
 *****************************************************************************/


/*
 * LCD test duration.
 *
 * After this time the test finishes.
 *
 */
#define LCD_TEST_DURATION_MS      3000U




/*
 * Number of LCD rows.
 *
 * HD44780 20x4 LCD.
 *
 */
#define LCD_TEST_ROWS             4U






/******************************************************************************
 *
 * Private Variables
 *
 *****************************************************************************/


/*
 * Current LCD test state.
 *
 */
static LCD_TestState_t lcd_test_state =
        LCD_TEST_IDLE;





/*
 * Test start timestamp.
 *
 * Used for non-blocking timing.
 *
 */
static uint32_t lcd_test_start_tick = 0U;






/*
 * Prevent multiple rendering
 * during the same test.
 *
 */
static uint8_t lcd_test_displayed = 0U;







/******************************************************************************
 *
 * Private Function Prototypes
 *
 *****************************************************************************/


/*
 * Display LCD test pattern.
 *
 */
static void LCD_Test_DisplayPattern(void);






/*
 * Finish LCD test.
 *
 */
static void LCD_Test_Finish(void);







/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/


/*
 * Display LCD diagnostic pattern.
 *
 *
 * The test verifies:
 *
 *      - Row addressing
 *      - Character writing
 *      - Full display area
 *
 */
static void LCD_Test_DisplayPattern(void)
{

    /*
     * Clear previous application content.
     */
    LCD_Display_Clear();





    /*
     * Row 0:
     * Title
     */
    LCD_Display_PrintLine(
            0U,
            "*** LCD TEST ***");






    /*
     * Row 1:
     * Character test
     */
    LCD_Display_PrintLine(
            1U,
            "12345678901234567890");






    /*
     * Row 2:
     * Alphabet test
     */
    LCD_Display_PrintLine(
            2U,
            "ABCDEFGHIJKLMNOPQRST");






    /*
     * Row 3:
     * Status
     */
    LCD_Display_PrintLine(
            3U,
            "LCD TEST RUNNING");

}

/******************************************************************************
 *
 * Public Functions
 *
 *****************************************************************************/


/*
 * Initialize LCD test module.
 *
 */
void LCD_Test_Init(void)
{

    lcd_test_state =
            LCD_TEST_IDLE;



    lcd_test_start_tick =
            0U;



    lcd_test_displayed =
            0U;

}







/*
 * Start LCD diagnostic test.
 *
 */
void LCD_Test_Start(void)
{

    /*
     * Start new test cycle.
     */
    lcd_test_state =
            LCD_TEST_RUNNING;




    /*
     * Store start time.
     */
    lcd_test_start_tick =
            HAL_GetTick();





    /*
     * Allow display update.
     */
    lcd_test_displayed =
            0U;

}







/*
 * Periodic LCD test task.
 *
 *
 * This function must be called
 * continuously from main loop.
 *
 */
void LCD_Test_Task(void)
{

    uint32_t elapsed_time;



    /*
     * Ignore task when idle.
     */
    if(lcd_test_state == LCD_TEST_IDLE)
    {
        return;
    }






    /*
     * Ignore finished test.
     */
    if(lcd_test_state == LCD_TEST_DONE)
    {
        return;
    }







    /*
     * Calculate elapsed time.
     */
    elapsed_time =
            HAL_GetTick()
            -
            lcd_test_start_tick;








    /*
     * First execution:
     *
     * Draw LCD pattern.
     */
    if(lcd_test_displayed == 0U)
    {

        LCD_Test_DisplayPattern();



        lcd_test_displayed = 1U;

    }







    /*
     * Check test timeout.
     */
    if(elapsed_time >= LCD_TEST_DURATION_MS)
    {

        LCD_Test_Finish();

    }

}





/*
 * Finish LCD test.
 *
 */
static void LCD_Test_Finish(void)
{

    /*
     * Mark test completion.
     *
     * Screen transition is handled
     * by ScreenManager.
     *
     * LCD content is intentionally
     * preserved here.
     */
    lcd_test_state =
            LCD_TEST_DONE;

}







/*
 * Check if LCD test is active.
 *
 */
uint8_t LCD_Test_IsActive(void)
{

    if(lcd_test_state == LCD_TEST_RUNNING)
    {
        return 1U;
    }



    return 0U;

}







/*
 * Get current LCD test state.
 *
 */
LCD_TestState_t LCD_Test_GetState(void)
{

    return lcd_test_state;

}

/******************************************************************************
 *
 * Integration Notes
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
 *            |
 *            |
 *            v
 *
 *      MenuAction_LCDTest()
 *
 *            |
 *            |
 *            v
 *
 *      ScreenManager_SetScreen()
 *
 *            |
 *            |
 *            v
 *
 *      SCREEN_LCD_TEST
 *
 *            |
 *            |
 *            v
 *
 *      LCD_Test_Start()
 *
 *            |
 *            |
 *            v
 *
 *      Main Loop
 *
 *            |
 *            |
 *            v
 *
 *      LCD_Test_Task()
 *
 *            |
 *            |
 *            v
 *
 *      LCD_Display_PrintLine()
 *
 *
 *
 *
 * The LCD test module is intentionally
 * separated from menu_actions.c.
 *
 *
 * Benefits:
 *
 *      - menu_actions.c remains clean.
 *
 *      - LCD diagnostic logic is isolated.
 *
 *      - Screen transition remains
 *        under ScreenManager control.
 *
 *
 */







/******************************************************************************
 *
 * Error Handling
 *
 *****************************************************************************/


/*
 *
 * This module performs safe operation:
 *
 *
 *      - No dynamic memory.
 *
 *      - No blocking delay loop.
 *
 *      - No direct hardware access.
 *
 *      - No LCD driver dependency.
 *
 *
 *
 * If LCD hardware is unavailable:
 *
 *      lcd_display layer
 *      handles communication failure.
 *
 *
 */







/******************************************************************************
 *
 * Future Extensions
 *
 *****************************************************************************/


/*
 *
 * Possible future LCD tests:
 *
 *
 *      1- Cursor test
 *
 *          Verify cursor positioning.
 *
 *
 *      2- Backlight test
 *
 *          Verify LCD backlight control.
 *
 *
 *      3- Character generator test
 *
 *          Verify custom characters.
 *
 *
 *      4- I2C communication test
 *
 *          Verify LCD backpack.
 *
 *
 */







/******************************************************************************
 *
 * Service Mode Usage Example
 *
 *****************************************************************************/


/*
 *
 * menu_actions.c:
 *
 *
 *
 * void MenuAction_LCDTest(void)
 * {
 *
 *      ScreenManager_SetScreen(
 *              SCREEN_LCD_TEST);
 *
 * }
 *
 *
 *
 *
 * main.c:
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
 * [OK] Independent diagnostic module.
 *
 * [OK] No menu dependency.
 *
 * [OK] No button dependency.
 *
 * [OK] No I2C dependency.
 *
 * [OK] Uses lcd_display abstraction.
 *
 * [OK] Non-blocking implementation.
 *
 * [OK] Suitable for STM32F103C8T6.
 *
 * [OK] Screen transition handled
 *      by ScreenManager.
 *
 *
 */







/******************************************************************************
 *
 * End Of File Verification
 *
 *****************************************************************************/


/*
 *
 * File:
 *
 *      lcd_test.c
 *
 *
 * Version:
 *
 *      Clean Final v1.0.1
 *
 *
 *
 * Module Purpose:
 *
 *      Provide LCD diagnostic test
 *      for Service Mode.
 *
 *
 *
 * Implemented Features:
 *
 *
 *      - Start LCD test.
 *
 *      - Non-blocking execution.
 *
 *      - Automatic finish after timeout.
 *
 *      - Uses LCD abstraction layer.
 *
 *      - No direct hardware access.
 *
 *      - Preserves LCD content after finish.
 *
 *
 *
 * Architecture:
 *
 *
 *
 *              menu_items.c
 *
 *                    |
 *                    v
 *
 *              menu_actions.c
 *
 *                    |
 *                    v
 *
 *              screen_manager.c
 *
 *                    |
 *                    v
 *
 *              lcd_test.c
 *
 *                    |
 *                    v
 *
 *              lcd_display.c
 *
 *                    |
 *                    v
 *
 *              lcd_i2c.c
 *
 *
 *
 *
 * Hardware:
 *
 *
 *      MCU:
 *
 *          STM32F103C8T6
 *
 *
 *      Display:
 *
 *          HD44780 Character LCD
 *
 *
 *      Interface:
 *
 *          PCF8574 I2C Backpack
 *
 *
 *****************************************************************************/


/*
 * End of lcd_test.c
 */
