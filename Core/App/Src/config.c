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
 *      Runtime configuration manager.
 *
 *      This module stores and manages all adjustable parameters:
 *
 *          - UART Baud Rate
 *          - Sample Rate
 *          - Alarm Settings
 *          - Voltage Limits
 *          - Calibration Offsets
 *
 *
 *      This module does NOT handle:
 *
 *          - LCD
 *          - Menu
 *          - Flash driver implementation
 *          - ADC measurement
 *
 ******************************************************************************/

#include "config.h"
#include "stm32f1xx_hal.h"
#include <string.h>


/*
 * ============================================================================
 * Private Types
 * ============================================================================
 */


/*
 * Runtime configuration storage.
 */
typedef struct
{

    /*
     * UART communication speed.
     */
    uint32_t baud_rate;



    /*
     * ADC sample interval.
     *
     * Unit:
     *
     *      milliseconds
     */
    uint32_t sample_rate_ms;



    /*
     * Low voltage alarm limit.
     *
     * Unit:
     *
     *      Volt
     */
    uint16_t voltage_low_limit;



    /*
     * High voltage alarm limit.
     *
     * Unit:
     *
     *      Volt
     */
    uint16_t voltage_high_limit;



    /*
     * Alarm enable.
     */
    uint8_t alarm_enabled;



    /*
     * Alarm mode.
     */
    ConfigAlarmMode_t alarm_mode;



    /*
     * Stream enable.
     */
    uint8_t stream_enabled;



    /*
     * Input voltage calibration offset.
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
    int16_t vin_offset;



    /*
     * Output voltage calibration offset.
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
    int16_t vout_offset;



} ConfigData_t;


/*
 * ============================================================================
 * Flash Storage Structure
 * ============================================================================
 */

/*
 * Flash image stored in internal Flash.
 */
typedef struct
{
    uint32_t magic;

    ConfigData_t data;

} ConfigFlash_t;


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
 * Flash image buffer.
 */
static ConfigFlash_t flash_image;


/*
 * Flash memory pointer.
 */
static const ConfigFlash_t *flash_config =
    (const ConfigFlash_t *)CONFIG_FLASH_ADDRESS;



/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */



static void Config_LoadDefault(void);

static uint16_t Config_ClampVoltage(
        uint16_t value);

static void Config_ValidateVoltageLimits(void);

static uint32_t Config_ClampSampleRate(
        uint32_t value);

static uint32_t Config_ClampBaudRate(
        uint32_t value);

static int16_t Config_ClampOffset(
        int16_t value);

static ConfigAlarmMode_t Config_ValidateAlarmMode(
        ConfigAlarmMode_t mode);

static bool Config_ReadFlash(void);

static bool Config_WriteFlash(void);



/*
 * Read configuration from Flash.
 */
static bool Config_ReadFlash(void)
{

    if(flash_config->magic != CONFIG_MAGIC_NUMBER)
    {

        return false;

    }

    memcpy(&config_data,
           &flash_config->data,
           sizeof(ConfigData_t));



    config_data.baud_rate =
            Config_ClampBaudRate(
                    config_data.baud_rate);

    config_data.sample_rate_ms =
            Config_ClampSampleRate(
                    config_data.sample_rate_ms);

    config_data.voltage_low_limit =
            Config_ClampVoltage(
                    config_data.voltage_low_limit);

    config_data.voltage_high_limit =
            Config_ClampVoltage(
                    config_data.voltage_high_limit);

    Config_ValidateVoltageLimits();

    config_data.vin_offset =
            Config_ClampOffset(
                    config_data.vin_offset);

    config_data.vout_offset =
            Config_ClampOffset(
                    config_data.vout_offset);

    config_data.alarm_mode =
            Config_ValidateAlarmMode(
                    config_data.alarm_mode);

    config_data.alarm_enabled =
            (config_data.alarm_enabled != 0U) ? 1U : 0U;

    config_data.stream_enabled =
            (config_data.stream_enabled != 0U) ? 1U : 0U;





    return true;

}



/*
 * Write configuration into Flash.
 */
static bool Config_WriteFlash(void)
{

    HAL_StatusTypeDef status;

    FLASH_EraseInitTypeDef erase;

    uint32_t page_error = 0U;

    uint32_t address;

    uint32_t *source;



    flash_image.magic = CONFIG_MAGIC_NUMBER;

    memcpy(&flash_image.data,
           &config_data,
           sizeof(ConfigData_t));



    HAL_FLASH_Unlock();



    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = CONFIG_FLASH_ADDRESS;
    erase.NbPages = 1U;

    status = HAL_FLASHEx_Erase(&erase,
                               &page_error);

    if(status != HAL_OK)
    {

        HAL_FLASH_Lock();

        return false;

    }


    address = CONFIG_FLASH_ADDRESS;

    source = (uint32_t *)&flash_image;



    uint32_t words =
            (sizeof(ConfigFlash_t) + 3U) / 4U;



    for(uint32_t i = 0U;
        i < words;
        i++)
    {

        status = HAL_FLASH_Program(
                    FLASH_TYPEPROGRAM_WORD,
                    address,
                    source[i]);

        if(status != HAL_OK)
        {

            HAL_FLASH_Lock();

            return false;

        }

        address += 4U;

    }


    HAL_FLASH_Lock();

    return true;

}


/*
 * Load factory default values.
 */
static void Config_LoadDefault(void)
{

    config_data.baud_rate =
            UART_BAUD_RATE_DEFAULT;



    config_data.sample_rate_ms =
            SAMPLE_RATE_DEFAULT_MS;



    config_data.voltage_low_limit =
            CONFIG_LOW_VOLTAGE_DEFAULT;



    config_data.voltage_high_limit =
            CONFIG_HIGH_VOLTAGE_DEFAULT;



    config_data.alarm_enabled =
            CONFIG_ALARM_ENABLE_DEFAULT;



    config_data.alarm_mode =
            CONFIG_ALARM_MODE_DEFAULT;



    config_data.stream_enabled =
            CONFIG_STREAM_DEFAULT_ENABLE;



    /*
     * Calibration default:
     *
     *      No correction
     */


    config_data.vin_offset =
            CONFIG_VIN_OFFSET_DEFAULT;

    config_data.vout_offset =
            CONFIG_VOUT_OFFSET_DEFAULT;



}





/*
 * Clamp voltage limit.
 */
static uint16_t Config_ClampVoltage(
        uint16_t value)
{

    if(value < CONFIG_VOLTAGE_LIMIT_MIN)
    {

        value = CONFIG_VOLTAGE_LIMIT_MIN;

    }



    if(value > CONFIG_VOLTAGE_LIMIT_MAX)
    {

        value = CONFIG_VOLTAGE_LIMIT_MAX;

    }



    return value;

}





/*
 * Validate voltage limits.
 */
static void Config_ValidateVoltageLimits(void)
{

    if(config_data.voltage_high_limit <=
       config_data.voltage_low_limit)
    {

        config_data.voltage_high_limit =
                config_data.voltage_low_limit +
                CONFIG_VOLTAGE_STEP;



        if(config_data.voltage_high_limit >
           CONFIG_VOLTAGE_LIMIT_MAX)
        {

            config_data.voltage_high_limit =
                    CONFIG_VOLTAGE_LIMIT_MAX;



            config_data.voltage_low_limit =
                    CONFIG_VOLTAGE_LIMIT_MAX -
                    CONFIG_VOLTAGE_STEP;

        }

    }

}



/*
 * Clamp sample rate.
 */
static uint32_t Config_ClampSampleRate(
        uint32_t value)
{

    if(value < CONFIG_SAMPLE_RATE_MIN)
    {

        value = CONFIG_SAMPLE_RATE_MIN;

    }



    if(value > CONFIG_SAMPLE_RATE_MAX)
    {

        value = CONFIG_SAMPLE_RATE_MAX;

    }



    return value;

}





/*
 * Clamp baud rate.
 */
static uint32_t Config_ClampBaudRate(
        uint32_t value)
{

    if(value < CONFIG_BAUD_RATE_MIN)
    {

        value = CONFIG_BAUD_RATE_MIN;

    }



    if(value > CONFIG_BAUD_RATE_MAX)
    {

        value = CONFIG_BAUD_RATE_MAX;

    }



    return value;

}





/*
 * Clamp calibration offset.
 *
 * Internal unit:
 *
 *      0.1 Volt
 *
 *
 * Range:
 *
 *      -20.0V ... +20.0V
 *
 */
static int16_t Config_ClampOffset(
        int16_t value)
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
 * Validate alarm mode.
 */
static ConfigAlarmMode_t Config_ValidateAlarmMode(
        ConfigAlarmMode_t mode)
{

    if((mode != CONFIG_ALARM_ONCE) &&
       (mode != CONFIG_ALARM_REPEAT))
    {

        mode = CONFIG_ALARM_MODE_DEFAULT;

    }



    return mode;

}





/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */


/*
 * Initialize configuration manager.
 */



void Config_Init(void)
{

    if(Config_ReadFlash() == false)
    {

        Config_LoadDefault();

        Config_WriteFlash();

    }

    Config_ValidateVoltageLimits();

}




/*
 * Restore factory defaults.
 */


void Config_ResetDefault(void)
{

    Config_LoadDefault();

    Config_Save();

}





/*
 * ============================================================================
 * UART Baud Rate API
 * ============================================================================
 */


/*
 * Get current baud rate.
 */
uint32_t Config_GetBaudRate(void)
{

    return config_data.baud_rate;

}





/*
 * Set baud rate.
 */
void Config_SetBaudRate(
        uint32_t baud_rate)
{

    config_data.baud_rate =
            Config_ClampBaudRate(
                    baud_rate);

}





/*
 * ============================================================================
 * Sample Rate API
 * ============================================================================
 */


/*
 * Get sample interval.
 */
uint32_t Config_GetSampleRate(void)
{

    return config_data.sample_rate_ms;

}





/*
 * Set sample interval.
 */
void Config_SetSampleRate(
        uint32_t sample_rate_ms)
{

    config_data.sample_rate_ms =
            Config_ClampSampleRate(
                    sample_rate_ms);

}





/*
 * ============================================================================
 * Voltage Limit API
 * ============================================================================
 */


/*
 * Get low voltage limit.
 */
uint16_t Config_GetLowVoltageLimit(void)
{

    return config_data.voltage_low_limit;

}





/*
 * Set low voltage limit.
 */
void Config_SetLowVoltageLimit(
        uint16_t limit)
{

    config_data.voltage_low_limit =
            Config_ClampVoltage(
                    limit);



    Config_ValidateVoltageLimits();

}





/*
 * Get high voltage limit.
 */
uint16_t Config_GetHighVoltageLimit(void)
{

    return config_data.voltage_high_limit;

}





/*
 * Set high voltage limit.
 */
void Config_SetHighVoltageLimit(
        uint16_t limit)
{

    config_data.voltage_high_limit =
            Config_ClampVoltage(
                    limit);



    Config_ValidateVoltageLimits();

}


/*
 * ============================================================================
 * Alarm Mode API
 * ============================================================================
 */


/*
 * Get current alarm mode.
 */
ConfigAlarmMode_t Config_GetAlarmMode(void)
{

    return config_data.alarm_mode;

}





/*
 * Set alarm mode.
 */
void Config_SetAlarmMode(
        ConfigAlarmMode_t mode)
{

    config_data.alarm_mode =
            Config_ValidateAlarmMode(
                    mode);

}





/*
 * ============================================================================
 * Alarm Enable API
 * ============================================================================
 */


/*
 * Get alarm enable status.
 */
uint8_t Config_GetAlarmEnable(void)
{

    return config_data.alarm_enabled;

}





/*
 * Set alarm enable status.
 */
void Config_SetAlarmEnable(
        uint8_t enable)
{

    if(enable != 0U)
    {

        config_data.alarm_enabled = 1U;

    }
    else
    {

        config_data.alarm_enabled = 0U;

    }

}





/*
 * ============================================================================
 * Calibration Offset API
 * ============================================================================
 *
 * Internal unit:
 *
 *      0.1 Volt
 *
 *
 * Example:
 *
 *      15  = +1.5V
 *
 *      -25 = -2.5V
 *
 */


/*
 * Get Vin calibration offset.
 */
int16_t Config_GetVinOffset(void)
{

    return config_data.vin_offset;

}





/*
 * Set Vin calibration offset.
 */
void Config_SetVinOffset(
        int16_t offset)
{

    config_data.vin_offset =
            Config_ClampOffset(
                    offset);

}





/*
 * Get Vout calibration offset.
 */
int16_t Config_GetVoutOffset(void)
{

    return config_data.vout_offset;

}





/*
 * Set Vout calibration offset.
 */
void Config_SetVoutOffset(
        int16_t offset)
{

    config_data.vout_offset =
            Config_ClampOffset(
                    offset);

}





/*
 * ============================================================================
 * Stream Configuration API
 * ============================================================================
 */


/*
 * Get stream enable status.
 */
uint8_t Config_GetStreamEnable(void)
{

    return config_data.stream_enabled;

}





/*
 * Set stream enable status.
 */
void Config_SetStreamEnable(
        uint8_t enable)
{

    if(enable != 0U)
    {

        config_data.stream_enabled = 1U;

    }
    else
    {

        config_data.stream_enabled = 0U;

    }

}





/*
 * ============================================================================
 * Flash Storage Interface
 * ============================================================================
 */


/*
 * Save configuration.
 *
 * Flash implementation will be
 * integrated with storage driver.
 */



void Config_Save(void)
{

    (void)Config_WriteFlash();

}





/*
 * Load configuration.
 *
 * Flash implementation will be
 * integrated with storage driver.
 */



void Config_Load(void)
{

    if(Config_ReadFlash() == false)
    {

        Config_LoadDefault();

        Config_Save();

    }

    Config_ValidateVoltageLimits();

}



/*
 * ============================================================================
 * Configuration Validation Interface
 * ============================================================================
 *
 * This section intentionally contains no public
 * validation functions.
 *
 * All validation is performed internally:
 *
 *      - Voltage limits
 *      - Calibration offsets
 *      - Baud rate
 *      - Sample rate
 *      - Alarm mode
 *
 */


/*
 * ============================================================================
 * End Of File
 * ============================================================================
 *
 * File:
 *
 *      config.c
 *
 *
 * Version:
 *
 *      v2.0.2
 *
 *
 * Implemented:
 *
 *      - Runtime configuration manager
 *      - Factory default initialization
 *      - Baud rate management
 *      - Sample rate management
 *      - Alarm configuration
 *      - Voltage threshold management
 *      - Vin calibration offset
 *      - Vout calibration offset
 *      - Stream configuration
 *
 *
 * Calibration:
 *
 *      Vin Offset
 *
 *          Range:
 *              -20.0V ... +20.0V
 *
 *          Step:
 *              0.1V
 *
 *
 *      Vout Offset
 *
 *          Range:
 *              -20.0V ... +20.0V
 *
 *          Step:
 *              0.1V
 *
 *
 * Internal Representation:
 *
 *      1 unit = 0.1 Volt
 *
 *
 * Example:
 *
 *      +20.0V  ->  200
 *
 *       0.0V   ->    0
 *
 *      -20.0V  -> -200
 *
 *
 * Dependencies:
 *
 *      config.h
 *
 *
 * No dependency:
 *
 *      - LCD
 *      - Menu
 *      - UART
 *      - ADC
 *      - HAL
 *
 *
 * Target:
 *
 *      STM32F103C8T6
 *
 *
 ******************************************************************************/
