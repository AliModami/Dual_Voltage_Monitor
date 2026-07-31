/******************************************************************************
 *
 * @file    adc_app.c
 *
 * @brief   ADC Application Layer Implementation
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU:
 *
 *      STM32F103C8T6
 *
 *------------------------------------------------------------------------------
 *
 * Framework:
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      Clean Final v1.4.0
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      Hardware independent ADC acquisition module.
 *
 *
 *      Features:
 *
 *      - Dual ADC channel acquisition
 *      - Runtime channel switching
 *      - ADC calibration
 *      - Raw ADC interface
 *      - Moving average digital filter
 *      - ADC count to voltage conversion
 *      - Voltage scaling calibration
 *      - Update flag management
 *
 *
 ******************************************************************************/


#include "adc_app.h"

#include "main.h"
#include "stm32f1xx_hal.h"
#include "config.h"
#include <stdio.h>


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


/*
 * ADC conversion timeout.
 */
#define ADC_TIMEOUT_MS                    10U




/*
 * ============================================================================
 * ADC Voltage Calibration Factor
 * ============================================================================
 *
 * This factor converts filtered ADC RAW value
 * into real input voltage.
 *
 *
 * Formula:
 *
 *      Voltage(0.1V) =
 *
 *          ADC_RAW * ADC_SCALE_NUMERATOR
 *          -----------------------------
 *              ADC_SCALE_DENOMINATOR
 *
 *
 * Example:
 *
 *      ADC_RAW = 3436
 *
 *      Factor = 657 / 1000
 *
 *      Voltage:
 *
 *          3436 * 657 / 1000
 *
 *          = 2257
 *
 *      Display:
 *
 *          225.7 V
 *
 *
 * Calibration procedure:
 *
 *      1- Measure real AC voltage with a calibrated multimeter.
 *
 *      2- Compare with LCD displayed value.
 *
 *      3- Adjust ADC_SCALE_NUMERATOR:
 *
 *          Display voltage too LOW:
 *
 *              Increase ADC_SCALE_NUMERATOR
 *
 *
 *          Display voltage too HIGH:
 *
 *              Decrease ADC_SCALE_NUMERATOR
 *
 *
 *      Recommended adjustment step:
 *
 *              +/- 1
 *
 *
 * Approximate effect:
 *
 *      One step changes result by about 0.15~0.25V
 *
 *
 * Current hardware:
 *
 *      Transformer:
 *
 *          220VAC -> 6VAC
 *
 *      Divider:
 *
 *          R_TOP    = 15K
 *          R_BOTTOM = 4.7K
 *
 *      ADC Reference:
 *
 *          3.3V
 *
 *
 * Current calibrated value:
 *
 *          657
 *
 *
 * =================================   Calibration   =========================================
 *
 * if LCD = 224.8V but Voltmeter is 225.8V,
 * * change #define ADC_SCALE_NUMERATOR 657U   to    #define ADC_SCALE_NUMERATOR 660U
 *
 *
 * if LCD = 227V but Voltmeter is 225.8V,
 * * change #define ADC_SCALE_NUMERATOR 657U   to    #define ADC_SCALE_NUMERATOR 635U
 *
 * ===========================================================================================
 */


#define ADC_SCALE_NUMERATOR        624U


/*
 * Keep denominator fixed.
 *
 * Higher resolution is possible by changing
 * numerator and denominator together.
 *
 * Example:
 *
 *      657 / 1000
 *
 *      1314 / 2000
 *
 */
#define ADC_SCALE_DENOMINATOR      1000U








/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */



/*
 * Latest raw ADC values.
 */
static uint16_t g_vin_raw  = 0U;

static uint16_t g_vout_raw = 0U;





/*
 * Filter buffers.
 */
static uint16_t g_vin_filter_buffer[ADC_FILTER_SIZE];

static uint16_t g_vout_filter_buffer[ADC_FILTER_SIZE];





/*
 * Filter accumulators.
 */
static uint32_t g_vin_filter_sum  = 0U;

static uint32_t g_vout_filter_sum = 0U;





/*
 * Current filter index.
 */
static uint8_t g_filter_index = 0U;





/*
 * Filter initialization state.
 */
static bool g_filter_initialized = false;





/*
 * Converted voltages.
 *
 * Unit:
 *
 *      0.1 Volt
 */
static Voltage_t g_vin_voltage  = 0U;

static Voltage_t g_vout_voltage = 0U;





/*
 * New ADC data flag.
 */
static bool g_adc_updated = false;





/*
 * ============================================================================
 * Private Function Prototypes
 * ============================================================================
 */


static HAL_StatusTypeDef ADC_App_SelectChannel(
        uint32_t channel);



static uint16_t ADC_App_ReadChannel(
        uint32_t channel);



static uint16_t ADC_App_FilterVin(
        uint16_t sample);



static uint16_t ADC_App_FilterVout(
        uint16_t sample);



static Voltage_t ADC_App_ConvertToVoltage(
        uint16_t adc_value);




/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */



/*
 * Configure ADC channel.
 */
static HAL_StatusTypeDef ADC_App_SelectChannel(
        uint32_t channel)
{

    ADC_ChannelConfTypeDef sConfig = {0};



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
static uint16_t ADC_App_ReadChannel(
        uint32_t channel)
{

    uint16_t value = 0U;



    if(ADC_App_SelectChannel(channel) != HAL_OK)
    {
        return 0U;
    }




    if(HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return 0U;
    }




    if(HAL_ADC_PollForConversion(
            &hadc1,
            ADC_TIMEOUT_MS) != HAL_OK)
    {

        HAL_ADC_Stop(&hadc1);

        return 0U;
    }





    value =
        (uint16_t)HAL_ADC_GetValue(&hadc1);





    HAL_ADC_Stop(&hadc1);




    return value;

}


/*
 * ============================================================================
 * Moving Average Filter - VIN
 * ============================================================================
 *
 * Add new sample and remove oldest sample.
 *
 * This reduces ADC noise and 100Hz ripple effect.
 *
 */
static uint16_t ADC_App_FilterVin(
        uint16_t sample)
{

    uint16_t old_value;



    /*
     * First call:
     *
     * Initialize all samples
     * with current value.
     */
    if(g_filter_initialized == false)
    {

        for(uint8_t i = 0U;
            i < ADC_FILTER_SIZE;
            i++)
        {

            g_vin_filter_buffer[i] = sample;

        }


        g_vin_filter_sum =
                (uint32_t)sample *
                ADC_FILTER_SIZE;


        return sample;

    }





    /*
     * Remove oldest sample.
     */
    old_value =
        g_vin_filter_buffer[g_filter_index];



    g_vin_filter_sum -= old_value;



    /*
     * Insert new sample.
     */
    g_vin_filter_buffer[g_filter_index] =
            sample;



    g_vin_filter_sum += sample;




    /*
     * Calculate average.
     */
    return
        (uint16_t)
        (g_vin_filter_sum /
         ADC_FILTER_SIZE);

}







/*
 * ============================================================================
 * Moving Average Filter - VOUT
 * ============================================================================
 */
static uint16_t ADC_App_FilterVout(
        uint16_t sample)
{

    uint16_t old_value;



    if(g_filter_initialized == false)
    {

        for(uint8_t i = 0U;
            i < ADC_FILTER_SIZE;
            i++)
        {

            g_vout_filter_buffer[i] = sample;

        }



        g_vout_filter_sum =
                (uint32_t)sample *
                ADC_FILTER_SIZE;



        return sample;

    }




    old_value =
        g_vout_filter_buffer[g_filter_index];



    g_vout_filter_sum -= old_value;



    g_vout_filter_buffer[g_filter_index] =
            sample;



    g_vout_filter_sum += sample;



    return
        (uint16_t)
        (g_vout_filter_sum /
         ADC_FILTER_SIZE);

}








/*
 * ============================================================================
 * ADC Count To Voltage Conversion
 * ============================================================================
 *
 * Input:
 *
 *      Filtered ADC value
 *
 *
 * Output:
 *
 *      Voltage in 0.1V unit
 *
 *
 * Example:
 *
 *      ADC = 3704
 *
 *      Result:
 *
 *          2260
 *
 *      Display:
 *
 *          226.0V
 *
 */
static Voltage_t ADC_App_ConvertToVoltage(
        uint16_t adc_value)
{

    uint32_t voltage;



    voltage =
        ((uint32_t)adc_value *
         ADC_SCALE_NUMERATOR)
        /
        ADC_SCALE_DENOMINATOR;



    return
        (Voltage_t)voltage;

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

    g_vin_raw  = 0U;

    g_vout_raw = 0U;



    g_vin_voltage  = 0U;

    g_vout_voltage = 0U;



    g_vin_filter_sum  = 0U;

    g_vout_filter_sum = 0U;



    g_filter_index = 0U;



    g_filter_initialized = false;



    g_adc_updated = false;



}





/*
 * ADC application task.
 *
 * Called continuously from main loop.
 *
 */
void ADC_App_Task(void)
{

    uint16_t vin_sample;

    uint16_t vout_sample;



    uint16_t vin_average;

    uint16_t vout_average;





    /*
     * Read ADC channels.
     *
     * PA0:
     *
     *      VIN
     *
     *
     * PA1:
     *
     *      VOUT
     */
    vin_sample =
        ADC_App_ReadChannel(
                ADC_CHANNEL_0);

    printf("VIN_RAW=%u\r\n", vin_sample);


    vout_sample =
        ADC_App_ReadChannel(
                ADC_CHANNEL_1);

    printf("VOUT_RAW=%u\r\n", vout_sample);






    /*
     * Apply digital filtering.
     */
    vin_average =
        ADC_App_FilterVin(
                vin_sample);



    vout_average =
        ADC_App_FilterVout(
                vout_sample);







    /*
     * Store filtered RAW values.
     */
    g_vin_raw =
            vin_average;



    g_vout_raw =
            vout_average;







    g_vin_voltage =
        ADC_App_ConvertToVoltage(vin_average);

    g_vout_voltage =
        ADC_App_ConvertToVoltage(vout_average);

    /* Apply software calibration */

    g_vin_voltage += Config_GetVinOffset();

    g_vout_voltage += Config_GetVoutOffset();

    /* Prevent negative voltage */

    if((int32_t)g_vin_voltage < 0)
    {
        g_vin_voltage = 0;
    }

    if((int32_t)g_vout_voltage < 0)
    {
        g_vout_voltage = 0;
    }






    /*
     * Move circular buffer index.
     */
    g_filter_index++;



    if(g_filter_index >= ADC_FILTER_SIZE)
    {

        g_filter_index = 0U;

        g_filter_initialized = true;

    }




    /*
     * Notify application.
     */
    g_adc_updated = true;

}


/*
 * ============================================================================
 * ADC Data Access Functions
 * ============================================================================
 */


/*
 * Get filtered VIN voltage.
 *
 * Return:
 *
 *      Voltage in 0.1V unit
 *
 * Example:
 *
 *      2260
 *
 *      means:
 *
 *      226.0V
 *
 */
Voltage_t ADC_App_GetVin(void)
{

    return g_vin_voltage;

}







/*
 * Get filtered VOUT voltage.
 *
 * Return:
 *
 *      Voltage in 0.1V unit
 */
Voltage_t ADC_App_GetVout(void)
{

    return g_vout_voltage;

}







/*
 * Get filtered VIN RAW ADC value.
 */
uint16_t ADC_App_GetVinRaw(void)
{

    return g_vin_raw;

}







/*
 * Get filtered VOUT RAW ADC value.
 */
uint16_t ADC_App_GetVoutRaw(void)
{

    return g_vout_raw;

}







/*
 * Check if new ADC data is available.
 */
bool ADC_App_IsUpdated(void)
{

    return g_adc_updated;

}







/*
 * Clear ADC update flag.
 */
void ADC_App_ClearUpdateFlag(void)
{

    g_adc_updated = false;

}








/*
 * ============================================================================
 * Calibration Support
 * ============================================================================
 */


/*
 * Apply VIN software calibration offset.
 *
 * Offset unit:
 *
 *      0.1V
 *
 *
 * Example:
 *
 *      Offset +5
 *
 *      means:
 *
 *      +0.5V correction
 *
 */
void ADC_App_SetVinOffset(
        int16_t offset)
{

    Config_SetVinOffset(offset);

}







/*
 * Apply VOUT software calibration offset.
 */
void ADC_App_SetVoutOffset(
        int16_t offset)
{

    Config_SetVoutOffset(offset);

}







/*
 * ============================================================================
 * Debug Helper Functions
 * ============================================================================
 */


/*
 * Return ADC filter size.
 *
 * Used for diagnostic display.
 */
uint8_t ADC_App_GetFilterSize(void)
{

    return ADC_FILTER_SIZE;

}








/*
 * ============================================================================
 * End Of File
 * ============================================================================
 *
 * adc_app.c
 *
 * Version:
 *
 *      Clean Final v1.4.0
 *
 *
 * Implemented:
 *
 *      - Dual channel ADC acquisition
 *      - PA0 VIN measurement
 *      - PA1 VOUT measurement
 *      - 16 sample moving average filter
 *      - Calibration scale:
 *
 *              610 / 1000
 *
 *      - Stable LCD voltage display
 *      - RAW diagnostic output
 *      - Software offset support
 *
 *
 * Validation target:
 *
 *      Real input:
 *
 *          226.0 VAC
 *
 *      ADC RAW:
 *
 *          3704
 *
 *      Expected display:
 *
 *          226.0V
 *
 *
 * Hardware:
 *
 *      STM32F103C8T6
 *
 *      ADC1
 *
 *      3.3V reference
 *
 ******************************************************************************/

