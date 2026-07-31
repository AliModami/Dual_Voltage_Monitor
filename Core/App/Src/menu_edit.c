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
 *      This module manages editing of runtime configuration parameters
 *      through the menu system.
 *
 *      Responsibilities:
 *
 *      - Start editing selected configuration item.
 *      - Store temporary edit value.
 *      - Apply or cancel changes.
 *      - Validate editable values.
 *
 *
 *      This module does NOT handle:
 *
 *      - LCD rendering.
 *      - Button hardware.
 *      - Configuration storage.
 *
 ******************************************************************************/

#include "menu_edit.h"
#include "config.h"
#include <stdio.h>


/*
 * ============================================================================
 * Private Constants
 * ============================================================================
 */


/*
 * Baud rate adjustment step.
 */
#define MENU_EDIT_BAUD_STEP        9600U


#define MENU_EDIT_OFFSET_STEP      1


/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


/*
 * Edit mode status.
 */
static bool g_edit_mode = false;



/*
 * Current edit target.
 */
static MenuEditTarget_t g_edit_target = EDIT_NONE;



/*
 * Temporary edited value.
 */
static int32_t g_edit_value = 0;



/*
 * Previous confirmed value.
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

    if(value < (int32_t)CONFIG_VOLTAGE_LIMIT_MIN)
    {

        value = CONFIG_VOLTAGE_LIMIT_MIN;

    }



    if(value > (int32_t)CONFIG_VOLTAGE_LIMIT_MAX)
    {

        value = CONFIG_VOLTAGE_LIMIT_MAX;

    }



    return value;

}





/*
 * Clamp sample rate value.
 */
static int32_t MenuEdit_ClampSampleRate(int32_t value)
{

    if(value < (int32_t)CONFIG_SAMPLE_RATE_MIN)
    {

        value = CONFIG_SAMPLE_RATE_MIN;

    }



    if(value > (int32_t)CONFIG_SAMPLE_RATE_MAX)
    {

        value = CONFIG_SAMPLE_RATE_MAX;

    }



    return value;

}





/*
 * Clamp baud rate value.
 */
static int32_t MenuEdit_ClampBaudRate(int32_t value)
{

    if(value < (int32_t)CONFIG_BAUD_RATE_MIN)
    {

        value = CONFIG_BAUD_RATE_MIN;

    }



    if(value > (int32_t)CONFIG_BAUD_RATE_MAX)
    {

        value = CONFIG_BAUD_RATE_MAX;

    }



    return value;

}


/*
 * Clamp calibration offset value.
 *
 * Unit:
 *
 *      0.1 Volt
 *
 * Range:
 *
 *      -200 ... +200
 *
 */
static int32_t MenuEdit_ClampOffset(int32_t value)
{

    if(value < CONFIG_OFFSET_MIN)
    {

        value = CONFIG_OFFSET_MIN;

    }



    if(value > CONFIG_OFFSET_MAX)
    {

        value = CONFIG_OFFSET_MAX;

    }



    return value;

}


/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */


/*
 * Initialize menu edit engine.
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


        case EDIT_INPUT_VOLTAGE_OFFSET:

            g_edit_value =
                    (int32_t)Config_GetVinOffset();

            break;


        case EDIT_OUTPUT_VOLTAGE_OFFSET:

            g_edit_value =
                    (int32_t)Config_GetVoutOffset();

            break;



        default:

            g_edit_value = 0;

            break;

    }



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
                    (ConfigAlarmMode_t)g_edit_value);

            break;



        case EDIT_INPUT_VOLTAGE_OFFSET:

            Config_SetVinOffset(
                    (int16_t)g_edit_value);



            break;


        case EDIT_OUTPUT_VOLTAGE_OFFSET:

            Config_SetVoutOffset(
                    (int16_t)g_edit_value);



            break;



        default:

            break;

    }



    /*
     * Save all confirmed menu changes
     * into internal Flash.
     */
    Config_Save();

    g_edit_old_value = g_edit_value;


    g_edit_mode = false;


    g_edit_target = EDIT_NONE;

}





/*
 * Cancel current edit operation.
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
 */
void MenuEdit_Increment(void)
{

    if(g_edit_mode == false)
    {

        return;

    }



    switch(g_edit_target)
    {

        case EDIT_BAUD_RATE:

            g_edit_value += MENU_EDIT_BAUD_STEP;


            g_edit_value =
                    MenuEdit_ClampBaudRate(
                            g_edit_value);

            break;



        case EDIT_SAMPLE_RATE:

            g_edit_value += SAMPLE_RATE_STEP_MS;


            g_edit_value =
                    MenuEdit_ClampSampleRate(
                            g_edit_value);

            break;



        case EDIT_ALARM_ENABLE:

            g_edit_value = 1;

            break;



        case EDIT_LOW_VOLTAGE_LIMIT:

        case EDIT_HIGH_VOLTAGE_LIMIT:

            g_edit_value += CONFIG_VOLTAGE_STEP;


            g_edit_value =
                    MenuEdit_ClampVoltage(
                            g_edit_value);

            break;



        case EDIT_ALARM_MODE:

            if(g_edit_value < CONFIG_ALARM_REPEAT)
            {

                g_edit_value++;

            }

            break;

        case EDIT_INPUT_VOLTAGE_OFFSET:

        case EDIT_OUTPUT_VOLTAGE_OFFSET:

            g_edit_value += CONFIG_OFFSET_STEP;


            g_edit_value =
                    MenuEdit_ClampOffset(
                            g_edit_value);

            break;


        default:

            break;

    }

}


/*
 * Decrease edited value.
 */
void MenuEdit_Decrement(void)
{

    if(g_edit_mode == false)
    {

        return;

    }



    switch(g_edit_target)
    {

        case EDIT_BAUD_RATE:

            g_edit_value -= MENU_EDIT_BAUD_STEP;


            g_edit_value =
                    MenuEdit_ClampBaudRate(
                            g_edit_value);

            break;



        case EDIT_SAMPLE_RATE:

            g_edit_value -= SAMPLE_RATE_STEP_MS;


            g_edit_value =
                    MenuEdit_ClampSampleRate(
                            g_edit_value);

            break;



        case EDIT_ALARM_ENABLE:

            g_edit_value = 0;

            break;



        case EDIT_LOW_VOLTAGE_LIMIT:

        case EDIT_HIGH_VOLTAGE_LIMIT:

            g_edit_value -= CONFIG_VOLTAGE_STEP;


            g_edit_value =
                    MenuEdit_ClampVoltage(
                            g_edit_value);

            break;



        case EDIT_ALARM_MODE:

            if(g_edit_value > CONFIG_ALARM_ONCE)
            {

                g_edit_value--;

            }

            break;


        case EDIT_INPUT_VOLTAGE_OFFSET:

        case EDIT_OUTPUT_VOLTAGE_OFFSET:

            g_edit_value -= CONFIG_OFFSET_STEP;


            g_edit_value =
                    MenuEdit_ClampOffset(
                            g_edit_value);

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
 * Get current edited value.
 */
int32_t MenuEdit_GetValue(void)
{

    return g_edit_value;

}



void MenuEdit_GetDisplayString(
        char *buffer,
        uint16_t size)
{

    if((buffer == NULL) || (size == 0U))
    {
        return;
    }

    switch(g_edit_target)
    {

        case EDIT_BAUD_RATE:

            snprintf(buffer,
                     size,
                     "%lu",
                     (unsigned long)g_edit_value);

            break;



        case EDIT_SAMPLE_RATE:

            snprintf(buffer,
                     size,
                     "%ld ms",
                     (long)g_edit_value);

            break;



        case EDIT_ALARM_ENABLE:

            snprintf(buffer,
                     size,
                     "%s",
                     (g_edit_value != 0) ? "ON" : "OFF");

            break;



        case EDIT_ALARM_MODE:

            snprintf(buffer,
                     size,
                     "%s",
                     (g_edit_value == CONFIG_ALARM_ONCE) ?
                     "ONCE" :
                     "REPEAT");

            break;



        case EDIT_LOW_VOLTAGE_LIMIT:

        case EDIT_HIGH_VOLTAGE_LIMIT:

            snprintf(buffer,
                     size,
                     "%ld V",
                     (long)g_edit_value);

            break;



        case EDIT_INPUT_VOLTAGE_OFFSET:

        case EDIT_OUTPUT_VOLTAGE_OFFSET:
        {
            long value = (long)g_edit_value;

            char sign = '+';

            if(value < 0)
            {
                sign = '-';
                value = -value;
            }

            snprintf(buffer,
                     size,
                     "%c%ld.%01ldV",
                     sign,
                     value / 10,
                     value % 10);

            break;
        }



        default:

            buffer[0] = '\0';

            break;

    }

}

/******************************************************************************
 *
 *                              END OF FILE
 *
 ******************************************************************************/

