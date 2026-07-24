/******************************************************************************
 *
 * @file    config.c
 *
 * @brief   Application Configuration Runtime Manager
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
 *      This module manages runtime configuration values.
 *
 *      Responsibilities:
 *
 *      - Initialize configuration values.
 *      - Provide default configuration.
 *      - Provide runtime access API.
 *      - Validate adjustable parameters.
 *
 *
 *      This module does NOT handle:
 *
 *      - Flash hardware access.
 *      - Menu processing.
 *      - LCD display.
 *      - UART communication.
 *
 ******************************************************************************/

#include "config.h"



/*
 * ============================================================================
 * Private Types
 * ============================================================================
 */


/*
 * Alarm operation mode.
 *
 * Once:
 *
 *      Alarm triggers one time.
 *
 *
 * Repeat:
 *
 *      Alarm continues while
 *      fault condition exists.
 */




/*
 * Runtime configuration structure.
 *
 * All adjustable application parameters
 * are stored here.
 */
typedef struct
{

    /*
     * UART baud rate.
     */
    uint32_t baud_rate;



    /*
     * ADC sampling interval.
     *
     * Unit:
     *
     *      milliseconds
     */
    uint32_t sample_rate_ms;



    /*
     * Low voltage alarm threshold.
     *
     * Unit:
     *
     *      Volt
     */
    uint16_t voltage_low_limit;



    /*
     * High voltage alarm threshold.
     *
     * Unit:
     *
     *      Volt
     */
    uint16_t voltage_high_limit;



    /*
     * Alarm enable state.
     */
    uint8_t alarm_enabled;



    /*
     * Alarm operation mode.
     */
    ConfigAlarmMode_t alarm_mode;



    /*
     * Stream enable state.
     */
    uint8_t stream_enabled;



} ConfigData_t;





/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


/*
 * Current runtime configuration.
 */
static ConfigData_t config_data;





/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */


/*
 * Load factory default values.
 *
 * Used by:
 *
 * - First startup.
 * - Restore Default command.
 * - Invalid configuration data.
 */
static void Config_LoadDefault(void)
{

    config_data.baud_rate =
            UART_BAUD_RATE_DEFAULT;



    config_data.sample_rate_ms =
            SAMPLE_RATE_DEFAULT_MS;



    config_data.voltage_low_limit =
            VOLTAGE_LOW_LIMIT_DEFAULT;



    config_data.voltage_high_limit =
            VOLTAGE_HIGH_LIMIT_DEFAULT;



    config_data.alarm_enabled =
            CONFIG_BUZZER_DEFAULT_ENABLE;



    config_data.alarm_mode =
            CONFIG_ALARM_REPEAT;



    config_data.stream_enabled =
            CONFIG_STREAM_DEFAULT_ENABLE;



}

/*
 * Check voltage limit relationship.
 *
 * Rule:
 *
 *      High voltage must always
 *      be greater than low voltage.
 */
static void Config_ValidateVoltageLimits(void)
{

    if(config_data.voltage_high_limit <=
       config_data.voltage_low_limit)
    {

        config_data.voltage_high_limit =
                config_data.voltage_low_limit +
                VOLTAGE_LIMIT_STEP;


        if(config_data.voltage_high_limit >
           VOLTAGE_LIMIT_MAX)
        {

            config_data.voltage_high_limit =
                    VOLTAGE_LIMIT_MAX;


            config_data.voltage_low_limit =
                    VOLTAGE_LIMIT_MAX -
                    VOLTAGE_LIMIT_STEP;

        }

    }

}





/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */


/*
 * Initialize configuration manager.
 */
void Config_Init(void)
{

    Config_LoadDefault();

}





/*
 * Restore factory configuration.
 */
void Config_ResetDefault(void)
{

    Config_LoadDefault();

}





/*
 * Get complete configuration object.
 *
 * Read-only access only.
 */
const void *Config_Get(void)
{

    return &config_data;

}


/*
 * ============================================================================
 * Baud Rate API
 * ============================================================================
 */


/*
 * Get current UART baud rate.
 */
uint32_t Config_GetBaudRate(void)
{

    return config_data.baud_rate;

}





/*
 * Set UART baud rate.
 *
 * Value is limited according to
 * project configuration.
 */
void Config_SetBaudRate(uint32_t baud_rate)
{

    if(baud_rate < UART_BAUD_RATE_MIN)
    {

        baud_rate = UART_BAUD_RATE_MIN;

    }



    if(baud_rate > UART_BAUD_RATE_MAX)
    {

        baud_rate = UART_BAUD_RATE_MAX;

    }



    config_data.baud_rate = baud_rate;

}





/*
 * ============================================================================
 * Sample Rate API
 * ============================================================================
 */


/*
 * Get ADC sample interval.
 *
 * Unit:
 *
 *      milliseconds
 */
uint32_t Config_GetSampleRate(void)
{

    return config_data.sample_rate_ms;

}





/*
 * Set ADC sample interval.
 */
void Config_SetSampleRate(uint32_t sample_rate_ms)
{

    if(sample_rate_ms < SAMPLE_RATE_MIN_MS)
    {

        sample_rate_ms = SAMPLE_RATE_MIN_MS;

    }



    if(sample_rate_ms > SAMPLE_RATE_MAX_MS)
    {

        sample_rate_ms = SAMPLE_RATE_MAX_MS;

    }



    config_data.sample_rate_ms =
            sample_rate_ms;

}





/*
 * ============================================================================
 * Voltage Threshold API
 * ============================================================================
 */


/*
 * Get low voltage threshold.
 *
 * Unit:
 *
 *      Volt
 */
uint16_t Config_GetLowVoltageLimit(void)
{

    return config_data.voltage_low_limit;

}





/*
 * Set low voltage threshold.
 */
void Config_SetLowVoltageLimit(uint16_t limit)
{

    if(limit < VOLTAGE_LIMIT_MIN)
    {

        limit = VOLTAGE_LIMIT_MIN;

    }



    if(limit > VOLTAGE_LIMIT_MAX)
    {

        limit = VOLTAGE_LIMIT_MAX;

    }



    config_data.voltage_low_limit =
            limit;



    Config_ValidateVoltageLimits();

}





/*
 * Get high voltage threshold.
 *
 * Unit:
 *
 *      Volt
 */
uint16_t Config_GetHighVoltageLimit(void)
{

    return config_data.voltage_high_limit;

}





/*
 * Set high voltage threshold.
 */
void Config_SetHighVoltageLimit(uint16_t limit)
{

    if(limit < VOLTAGE_LIMIT_MIN)
    {

        limit = VOLTAGE_LIMIT_MIN;

    }



    if(limit > VOLTAGE_LIMIT_MAX)
    {

        limit = VOLTAGE_LIMIT_MAX;

    }



    config_data.voltage_high_limit =
            limit;



    Config_ValidateVoltageLimits();

}





/*
 * ============================================================================
 * Alarm Mode API
 * ============================================================================
 */


/*
 * Get alarm mode.
 */
uint8_t Config_GetAlarmMode(void)
{

    return (uint8_t)config_data.alarm_mode;

}





/*
 * Set alarm mode.
 */
void Config_SetAlarmMode(uint8_t mode)
{

    if(mode > CONFIG_ALARM_REPEAT)
    {

        mode = CONFIG_ALARM_REPEAT;

    }



    config_data.alarm_mode =
            (ConfigAlarmMode_t)mode;

}





/*
 * ============================================================================
 * Alarm Enable API
 * ============================================================================
 */


/*
 * Get alarm enable state.
 */
uint8_t Config_GetAlarmEnable(void)
{

    return config_data.alarm_enabled;

}





/*
 * Set alarm enable state.
 */
void Config_SetAlarmEnable(uint8_t enable)
{

    config_data.alarm_enabled =
            (enable != 0U) ? 1U : 0U;

}





/*
 * ============================================================================
 * Stream API
 * ============================================================================
 */


/*
 * Get stream state.
 */
uint8_t Config_GetStreamEnable(void)
{

    return config_data.stream_enabled;

}





/*
 * Set stream state.
 */
void Config_SetStreamEnable(uint8_t enable)
{

    config_data.stream_enabled =
            (enable != 0U) ? 1U : 0U;

}





/*
 * ============================================================================
 * Storage API
 * ============================================================================
 */


/*
 * Save configuration.
 *
 * Flash implementation will be added
 * after configuration structure is stable.
 */
void Config_Save(void)
{

    /*
     * TODO:
     *
     * Implement STM32 Flash write.
     */

}





/*
 * Load configuration.
 *
 * Flash implementation will be added
 * after configuration structure is stable.
 */
void Config_Load(void)
{

    /*
     * TODO:
     *
     * Implement STM32 Flash read.
     */

}





/******************************************************************************
 *
 *                              END OF FILE
 *
 ******************************************************************************/
