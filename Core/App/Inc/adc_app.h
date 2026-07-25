/******************************************************************************
 *
 * @file    adc_app.h
 *
 * @brief   ADC Application Layer Public Interface
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
 * Description:
 *
 *      This module provides the high level application interface for
 *      voltage acquisition.
 *
 *      It isolates the application from the STM32 HAL ADC driver.
 *
 *      Responsibilities:
 *
 *      - Read ADC channels.
 *      - Apply digital filtering.
 *      - Convert ADC counts to voltage.
 *      - Apply calibration offsets.
 *      - Provide stable voltage values to application modules.
 *
 *
 *      Used by:
 *
 *          - Live Monitor
 *          - Alarm Manager
 *          - UART Stream
 *          - Data Logger
 *          - Calibration
 *
 *
 *      This module does NOT handle:
 *
 *          - LCD rendering
 *          - Button processing
 *          - Menu navigation
 *          - Configuration storage
 *
 ******************************************************************************/

#ifndef ADC_APP_H
#define ADC_APP_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>





/*
 * ============================================================================
 * Voltage Unit
 * ============================================================================
 *
 * All voltage values returned by this module use:
 *
 *      Unit = 0.1 Volt
 *
 *
 * Examples:
 *
 *      Returned Value      Actual Voltage
 *      ---------------------------------
 *          0                   0.0 V
 *         125                 12.5 V
 *        2305               230.5 V
 *        2899               289.9 V
 *
 * This representation avoids floating point calculations.
 *
 */

typedef uint16_t Voltage_t;





/*
 * ============================================================================
 * ADC Channel Identifier
 * ============================================================================
 */

typedef enum
{

    ADC_APP_CHANNEL_VIN = 0U,

    ADC_APP_CHANNEL_VOUT

} ADC_AppChannel_t;





/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

/*
 * Initialize ADC application module.
 *
 * This function initializes all internal
 * software variables.
 *
 * Note:
 *
 * Hardware ADC initialization is performed
 * by CubeMX generated code.
 */
void ADC_App_Init(void);





/*
 * ============================================================================
 * Runtime
 * ============================================================================
 */

/*
 * Execute ADC application task.
 *
 * This function should be called periodically.
 *
 * Responsibilities:
 *
 *      - Read ADC channels.
 *      - Update digital filter.
 *      - Calculate voltages.
 *      - Apply calibration.
 */
void ADC_App_Task(void);





/*
 * ============================================================================
 * Raw ADC Data
 * ============================================================================
 */

/*
 * Get raw ADC value of Vin channel.
 *
 * Return:
 *
 *      12-bit ADC count.
 *
 * Range:
 *
 *      0 ... 4095
 */
uint16_t ADC_App_GetVinRaw(void);





/*
 * Get raw ADC value of Vout channel.
 *
 * Return:
 *
 *      12-bit ADC count.
 *
 * Range:
 *
 *      0 ... 4095
 */
uint16_t ADC_App_GetVoutRaw(void);





/*
 * Get raw ADC value of selected channel.
 *
 * @param channel
 *      ADC input channel.
 *
 * @return
 *      Raw ADC count.
 */
uint16_t ADC_App_GetRaw(
        ADC_AppChannel_t channel);





/*
 * ============================================================================
 * Measured Voltage
 * ============================================================================
 */

/*
 * Get calibrated input voltage.
 *
 * Unit:
 *
 *      0.1 Volt
 *
 * Example:
 *
 *      2305 = 230.5 V
 */
Voltage_t ADC_App_GetVin(void);





/*
 * Get calibrated output voltage.
 *
 * Unit:
 *
 *      0.1 Volt
 *
 * Example:
 *
 *      2198 = 219.8 V
 */
Voltage_t ADC_App_GetVout(void);





/*
 * Get calibrated voltage of selected channel.
 *
 * @param channel
 *      ADC input channel.
 *
 * @return
 *      Voltage in 0.1 Volt unit.
 */
Voltage_t ADC_App_GetVoltage(
        ADC_AppChannel_t channel);





/*
 * ============================================================================
 * Update Status
 * ============================================================================
 */

/*
 * Check whether new ADC data
 * is available.
 *
 * Return:
 *
 *      true
 *          New data available.
 *
 *      false
 *          No new data.
 */
bool ADC_App_IsUpdated(void);





/*
 * Clear update flag.
 *
 * Normally called after application
 * processes new measurements.
 */
void ADC_App_ClearUpdateFlag(void);





/*
 * ============================================================================
 * Utility
 * ============================================================================
 */

/*
 * Force immediate measurement update.
 *
 * Mainly intended for diagnostics
 * and service mode.
 */
void ADC_App_Refresh(void);





#ifdef __cplusplus
}
#endif

#endif /* ADC_APP_H */


/******************************************************************************
 *
 *                              END OF FILE
 *
 * File:
 *
 *      adc_app.h
 *
 * Version:
 *
 *      Clean Final v1.0.0
 *
 * Design Features:
 *
 *      - HAL independent public API
 *      - Float-free voltage interface
 *      - 0.1 Volt resolution
 *      - Dual channel support
 *      - Raw ADC access
 *      - Calibrated voltage access
 *      - Update notification support
 *      - Future compatible architecture
 *
 ******************************************************************************/
