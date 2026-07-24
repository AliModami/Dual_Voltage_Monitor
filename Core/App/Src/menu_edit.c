/******************************************************************************
 *
 * @file    menu_edit.c
 *
 * @brief   Menu Edit Engine Implementation
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
 *      This module manages editing of runtime configuration
 *      parameters from the menu system.
 *
 *      Supported parameters:
 *
 *      - Baud Rate
 *      - Sample Rate
 *      - Alarm Enable
 *      - Low Voltage Limit
 *      - High Voltage Limit
 *      - Alarm Mode
 *
 *
 *      This module does NOT handle:
 *
 *      - LCD rendering
 *      - Button reading
 *      - Hardware access
 *
 ******************************************************************************/

#include "menu_edit.h"
#include "config.h"





/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


/*
 * Current edit state.
 */
static bool g_edit_mode = false;



/*
 * Current edit target.
 */
static MenuEditTarget_t g_edit_target = EDIT_NONE;



/*
 * Current temporary edited value.
 */
static int32_t g_edit_value = 0;



/*
 * Previous confirmed value.
 *
 * Used for cancel operation.
 */
static int32_t g_edit_old_value = 0;







/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */


/*
 * Clamp voltage value.
 */
static int32_t MenuEdit_ClampVoltage(int32_t value)
{

    if(value < VOLTAGE_LIMIT_MIN)
    {
        value = VOLTAGE_LIMIT_MIN;
    }



    if(value > VOLTAGE_LIMIT_MAX)
    {
        value = VOLTAGE_LIMIT_MAX;
    }



    return value;

}





/*
 * Clamp sample rate value.
 */
static int32_t MenuEdit_ClampSampleRate(int32_t value)
{

    if(value < SAMPLE_RATE_MIN_MS)
    {
        value = SAMPLE_RATE_MIN_MS;
    }



    if(value > SAMPLE_RATE_MAX_MS)
    {
        value = SAMPLE_RATE_MAX_MS;
    }



    return value;

}





/*
 * Clamp baud rate value.
 */
static int32_t MenuEdit_ClampBaudRate(int32_t value)
{

    if(value < UART_BAUD_RATE_MIN)
    {
        value = UART_BAUD_RATE_MIN;
    }



    if(value > UART_BAUD_RATE_MAX)
    {
        value = UART_BAUD_RATE_MAX;
    }



    return value;

}







/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */



/*
 * Initialize edit engine.
 */
void MenuEdit_Init(void)
{

    g_edit_mode = false;


    g_edit_target = EDIT_NONE;


    g_edit_value = 0;


    g_edit_old_value = 0;

}







/*
 * Start editing selected parameter.
 */
void MenuEdit_Start(MenuEditTarget_t target)
{

    g_edit_target = target;



    switch(target)
    {

        case EDIT_BAUD_RATE:

            g_edit_value =
                    (int32_t)Config_GetBaudRate();

            break;



        case EDIT_SAMPLE_RATE:

            g_edit_value =
                    (int32_t)Config_GetSampleRate();

            break;



        case EDIT_ALARM_ENABLE:

            g_edit_value =
                    (int32_t)Config_GetAlarmEnable();

            break;



        case EDIT_LOW_VOLTAGE_LIMIT:

            g_edit_value =
                    (int32_t)Config_GetLowVoltageLimit();

            break;



        case EDIT_HIGH_VOLTAGE_LIMIT:

            g_edit_value =
                    (int32_t)Config_GetHighVoltageLimit();

            break;



        case EDIT_ALARM_MODE:

            g_edit_value =
                    (int32_t)Config_GetAlarmMode();

            break;



        default:

            g_edit_value = 0;

            break;

    }



    /*
     * Save old value.
     *
     * Used when BACK is pressed.
     */
    g_edit_old_value = g_edit_value;



    g_edit_mode = true;

}

/*
 * ============================================================================
 * Edit Confirmation / Cancel
 * ============================================================================
 */


/*
 * Confirm current edited value.
 *
 * The temporary value becomes
 * the active configuration value.
 */
void MenuEdit_Confirm(void)
{

    if(g_edit_mode == false)
    {
        return;
    }



    switch(g_edit_target)
    {

        case EDIT_BAUD_RATE:

            Config_SetBaudRate(
                    (uint32_t)g_edit_value);

            break;



        case EDIT_SAMPLE_RATE:

            Config_SetSampleRate(
                    (uint32_t)g_edit_value);

            break;



        case EDIT_ALARM_ENABLE:

            Config_SetAlarmEnable(
                    (uint8_t)g_edit_value);

            break;



        case EDIT_LOW_VOLTAGE_LIMIT:

            Config_SetLowVoltageLimit(
                    (uint16_t)g_edit_value);

            break;



        case EDIT_HIGH_VOLTAGE_LIMIT:

            Config_SetHighVoltageLimit(
                    (uint16_t)g_edit_value);

            break;



        case EDIT_ALARM_MODE:

            Config_SetAlarmMode(
                    (uint8_t)g_edit_value);

            break;



        default:

            break;

    }



    /*
     * Save confirmed value.
     */
    g_edit_old_value = g_edit_value;



    g_edit_mode = false;


    g_edit_target = EDIT_NONE;

}





/*
 * Cancel current edit operation.
 *
 * Previous value is restored.
 */
void MenuEdit_Cancel(void)
{

    if(g_edit_mode == false)
    {
        return;
    }



    g_edit_value = g_edit_old_value;



    g_edit_mode = false;


    g_edit_target = EDIT_NONE;

}







/*
 * Enter edit mode.
 */
void MenuEdit_Enter(void)
{

    g_edit_mode = true;

}





/*
 * Exit edit mode.
 */
void MenuEdit_Exit(void)
{

    g_edit_mode = false;


    g_edit_target = EDIT_NONE;

}







/*
 * ============================================================================
 * Value Modification
 * ============================================================================
 */



/*
 * Increase edited value.
 *
 * UP button is mapped here.
 */
void MenuEdit_Increment(void)
{

    if(g_edit_mode == false)
    {
        return;
    }



    switch(g_edit_target)
    {


        /*
         * Baud rate:
         *
         * 9600 step
         */
        case EDIT_BAUD_RATE:

            g_edit_value += 9600;


            g_edit_value =
                    MenuEdit_ClampBaudRate(
                            g_edit_value);

            break;





        /*
         * Sample rate:
         *
         * 100ms step
         */
        case EDIT_SAMPLE_RATE:

            g_edit_value += SAMPLE_RATE_STEP_MS;


            g_edit_value =
                    MenuEdit_ClampSampleRate(
                            g_edit_value);

            break;





        /*
         * Alarm enable:
         *
         * 0 -> 1
         */
        case EDIT_ALARM_ENABLE:

            g_edit_value = 1;

            break;





        /*
         * Voltage threshold:
         *
         * 5V step
         */
        case EDIT_LOW_VOLTAGE_LIMIT:

        case EDIT_HIGH_VOLTAGE_LIMIT:

            g_edit_value += VOLTAGE_LIMIT_STEP;


            g_edit_value =
                    MenuEdit_ClampVoltage(
                            g_edit_value);

            break;





        /*
         * Alarm mode:
         *
         * Once -> Repeat
         */
        case EDIT_ALARM_MODE:

            if(g_edit_value < CONFIG_ALARM_REPEAT)
            {
                g_edit_value++;
            }

            break;



        default:

            break;

    }

}






/*
 * Decrease edited value.
 *
 * DOWN button is mapped here.
 */
void MenuEdit_Decrement(void)
{

    if(g_edit_mode == false)
    {
        return;
    }



    switch(g_edit_target)
    {


        /*
         * Baud rate:
         *
         * 9600 step
         */
        case EDIT_BAUD_RATE:

            g_edit_value -= 9600;


            g_edit_value =
                    MenuEdit_ClampBaudRate(
                            g_edit_value);

            break;





        /*
         * Sample rate:
         *
         * 100ms step
         */
        case EDIT_SAMPLE_RATE:

            g_edit_value -= SAMPLE_RATE_STEP_MS;


            g_edit_value =
                    MenuEdit_ClampSampleRate(
                            g_edit_value);

            break;





        /*
         * Alarm enable:
         *
         * 1 -> 0
         */
        case EDIT_ALARM_ENABLE:

            g_edit_value = 0;

            break;





        /*
         * Voltage threshold:
         *
         * 5V step
         */
        case EDIT_LOW_VOLTAGE_LIMIT:

        case EDIT_HIGH_VOLTAGE_LIMIT:

            g_edit_value -= VOLTAGE_LIMIT_STEP;


            g_edit_value =
                    MenuEdit_ClampVoltage(
                            g_edit_value);

            break;





        /*
         * Alarm mode:
         *
         * Repeat -> Once
         */
        case EDIT_ALARM_MODE:

            if(g_edit_value > CONFIG_ALARM_ONCE)
            {
                g_edit_value--;
            }

            break;



        default:

            break;

    }

}


/*
 * ============================================================================
 * Status Information API
 * ============================================================================
 */


/*
 * Check edit mode status.
 *
 * Returns:
 *
 *      true  : editing active
 *      false : normal menu operation
 */
bool MenuEdit_IsActive(void)
{

    return g_edit_mode;

}





/*
 * Get current edit target.
 */
MenuEditTarget_t MenuEdit_GetTarget(void)
{

    return g_edit_target;

}





/*
 * Get current temporary edited value.
 *
 * Used by:
 *
 *      - menu_renderer.c
 *      - LCD display layer
 *
 * Example:
 *
 *      LCD_ShowNumber(
 *              MenuEdit_GetValue());
 */
int32_t MenuEdit_GetValue(void)
{

    return g_edit_value;

}





/******************************************************************************
 *
 *                              END OF FILE
 *
 ******************************************************************************/
