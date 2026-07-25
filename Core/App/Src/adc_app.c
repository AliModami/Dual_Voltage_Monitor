/******************************************************************************
 *
 * @file    adc_app.c
 *
 * @brief   ADC Application Layer
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
 *      Hardware independent ADC acquisition module.
 *
 *      Current Features:
 *
 *      - Dual channel acquisition
 *      - Runtime channel switching
 *      - ADC calibration
 *      - Raw ADC interface
 *      - Application task
 *      - Update flag management
 *
 ******************************************************************************/

#include "adc_app.h"

#include "main.h"
#include "stm32f1xx_hal.h"
#include "config.h"

/*
 * ============================================================================
 * External Variables
 * ============================================================================
 */

extern ADC_HandleTypeDef hadc1;

/*
 * ============================================================================
 * Private Definitions
 * ============================================================================
 */

#define ADC_TIMEOUT_MS                 10U

/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


static bool g_update_flag = false;


/*
 * Latest raw ADC samples.
 */
static uint16_t g_vin_raw  = 0U;
static uint16_t g_vout_raw = 0U;

/*
 * Converted voltage values.
 *
 * (Temporary placeholder.
 * Real conversion will be added later.)
 */
static Voltage_t g_vin_voltage  = 0U;
static Voltage_t g_vout_voltage = 0U;

/*
 * New data available flag.
 */
static bool g_adc_updated = false;

/*
 * ============================================================================
 * Private Function Prototypes
 * ============================================================================
 */

static HAL_StatusTypeDef ADC_App_SelectChannel(uint32_t channel);

static uint16_t ADC_App_ReadChannel(uint32_t channel);

/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */

/*
 * Configure ADC regular channel.
 */
static HAL_StatusTypeDef ADC_App_SelectChannel(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel      = channel;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    return HAL_ADC_ConfigChannel(
            &hadc1,
            &sConfig);
}

/*
 * Read one ADC conversion.
 */
static uint16_t ADC_App_ReadChannel(uint32_t channel)
{
    uint16_t value = 0U;

    if (ADC_App_SelectChannel(channel) != HAL_OK)
    {
        return 0U;
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return 0U;
    }

    if (HAL_ADC_PollForConversion(
            &hadc1,
            ADC_TIMEOUT_MS) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);

        return 0U;
    }

    value = (uint16_t)HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    return value;
}


/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */

/*
 * Initialize ADC application layer.
 */
void ADC_App_Init(void)
{
    /*
     * Calibrate ADC.
     */
    HAL_ADCEx_Calibration_Start(&hadc1);

    /*
     * Clear runtime variables.
     */
    g_vin_raw      = 0U;
    g_vout_raw     = 0U;

    g_vin_voltage  = 0U;
    g_vout_voltage = 0U;

    g_adc_updated  = false;
}

/*
 * Execute ADC application task.
 *
 * This function should be called periodically.
 */
void ADC_App_Task(void)
{
    /*
     * Acquire latest ADC samples.
     */
    g_vin_raw  = ADC_App_ReadChannel(ADC_CHANNEL_0);

    g_vout_raw = ADC_App_ReadChannel(ADC_CHANNEL_1);

    /*
     * ------------------------------------------------------------------------
     * Temporary conversion.
     *
     * Real ADC -> Voltage conversion,
     * averaging, scaling and calibration
     * will be implemented in the next version.
     * ------------------------------------------------------------------------
     */
    g_vin_voltage  = 0U;
    g_vout_voltage = 0U;

    /*
     * Notify application.
     */
    g_adc_updated = true;
}

/*
 * Force one immediate measurement update.
 */
void ADC_App_Refresh(void)
{
    ADC_App_Task();
}



/*
 * ============================================================================
 * Raw ADC Access Functions
 * ============================================================================
 */

/*
 * Get latest raw Vin ADC value.
 */
uint16_t ADC_App_GetVinRaw(void)
{
    return g_vin_raw;
}

/*
 * Get latest raw Vout ADC value.
 */
uint16_t ADC_App_GetVoutRaw(void)
{
    return g_vout_raw;
}

/*
 * Get latest raw ADC value of selected channel.
 */
uint16_t ADC_App_GetRaw(
        ADC_AppChannel_t channel)
{
    switch(channel)
    {
        case ADC_APP_CHANNEL_VIN:

            return g_vin_raw;

        case ADC_APP_CHANNEL_VOUT:

            return g_vout_raw;

        default:

            return 0U;
    }
}

/*
 * ============================================================================
 * Voltage Access Functions
 * ============================================================================
 */

/*
 * Get latest calculated Vin voltage.
 *
 * Unit:
 *
 *      0.1 Volt
 */
Voltage_t ADC_App_GetVin(void)
{
    return g_vin_voltage;
}

/*
 * Get latest calculated Vout voltage.
 *
 * Unit:
 *
 *      0.1 Volt
 */
Voltage_t ADC_App_GetVout(void)
{
    return g_vout_voltage;
}

/*
 * Get latest calculated voltage
 * of selected channel.
 */
Voltage_t ADC_App_GetVoltage(
        ADC_AppChannel_t channel)
{
    switch(channel)
    {
        case ADC_APP_CHANNEL_VIN:

            return g_vin_voltage;

        case ADC_APP_CHANNEL_VOUT:

            return g_vout_voltage;

        default:

            return 0U;
    }
}


/*
 * ============================================================================
 * Update Status Functions
 * ============================================================================
 */

/*
 * Check whether new ADC data
 * is available.
 */
bool ADC_App_IsUpdated(void)
{
    return g_update_flag;
}

/*
 * Clear update flag.
 */
void ADC_App_ClearUpdateFlag(void)
{
    g_update_flag = false;
}

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */



/*
 * ============================================================================
 * End Of File
 * ============================================================================
 *
 * File:
 *
 *      adc_app.c
 *
 * Version:
 *
 *      Clean Final v1.1.0
 *
 * Implemented Features:
 *
 *      - STM32 HAL based ADC interface
 *      - Runtime channel switching
 *      - ADC self calibration
 *      - Vin raw acquisition
 *      - Vout raw acquisition
 *      - Cached raw measurements
 *      - Cached voltage values
 *      - Periodic acquisition task
 *      - Update flag mechanism
 *      - Immediate refresh function
 *      - Hardware independent public API
 *
 * Current Conversion:
 *
 *      Temporary:
 *
 *          ADC Count
 *              ->
 *          Voltage (0.1V)
 *
 *      A placeholder conversion is currently used.
 *
 * Future Versions:
 *
 *      v1.2.0
 *
 *          - Moving Average Filter
 *          - ADC scaling coefficient
 *          - Vin calibration offset
 *          - Vout calibration offset
 *          - Saturation protection
 *          - Improved conversion accuracy
 *
 * Dependencies:
 *
 *      adc_app.h
 *      config.h
 *      main.h
 *      stm32f1xx_hal.h
 *
 * Compatible With:
 *
 *      STM32F103C8T6
 *      STM32 HAL
 *
 * ============================================================================
 */
