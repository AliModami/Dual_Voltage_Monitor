/******************************************************************************
 * @file    app_types.h
 * @brief   Common application data types
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file defines common data types that may be shared between multiple
 *      application modules.
 *
 *      Design Rules
 *      ------------
 *      1. Only generic project-wide types belong here.
 *      2. Module-specific types must remain inside their own module.
 *      3. No global variables are allowed.
 *      4. No function implementations are allowed.
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 * Created :
 *      2026-07-08
 ******************************************************************************/


#ifndef APP_TYPES_H
#define APP_TYPES_H


#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 *                          Boolean Type Information
 ******************************************************************************/

/*
 * Modern embedded C projects should use the standard C99 boolean type.
 *
 * The <stdbool.h> header provides:
 *
 *      bool
 *      true
 *      false
 *
 * Example:
 *
 *      bool alarm_enabled;
 *      bool lcd_ready;
 *
 * We intentionally do not create another boolean type here.
 * Using the standard type keeps the project compatible with other
 * C libraries and prevents unnecessary duplicate definitions.
 */


/******************************************************************************
 *                          Voltage Type
 ******************************************************************************/

/*
 * Represents a voltage value in volts.
 *
 * We intentionally use float because:
 *
 *      220.5 V
 *      198.7 V
 *      231.2 V
 *
 * are all valid values in this project.
 *
 * Using a meaningful type name improves code readability.
 *
 * Example:
 *
 *      voltage_t input_voltage;
 */

typedef float voltage_t;


/******************************************************************************
 *                          Time Type
 ******************************************************************************/

/*
 * Time in milliseconds.
 *
 * This type is used throughout the project for:
 *
 *  - Delays
 *  - Timeouts
 *  - Sampling period
 *  - Refresh intervals
 */

typedef uint32_t time_ms_t;


/******************************************************************************
 *                          ADC Raw Type
 ******************************************************************************/

/*
 * Raw ADC conversion value.
 *
 * STM32F103 ADC:
 *
 *      0 ... 4095
 *
 * This type represents the direct value returned by ADC hardware before
 * conversion into engineering units.
 */

typedef uint16_t adc_raw_t;


/******************************************************************************
 *                          Millivolt Type
 ******************************************************************************/

/*
 * Internal calculations often use millivolts
 * instead of floating-point values.
 *
 * Example:
 *
 *      230000 mV = 230 V
 *
 * This type is useful when integer calculations are preferred.
 */

typedef uint32_t millivolt_t;


/******************************************************************************
 *                          Percentage Type
 ******************************************************************************/

/*
 * Percentage value.
 *
 * Range:
 *
 *      0 ... 100
 */

typedef uint8_t percent_t;


/******************************************************************************
 *                          Counter Type
 ******************************************************************************/

/*
 * Generic software counter.
 *
 * Used for:
 *
 *  - Sample counters
 *  - Event counters
 *  - Statistics
 */

typedef uint32_t counter_t;


/******************************************************************************
 *                          Index Type
 ******************************************************************************/

/*
 * Used for indexing arrays and tables.
 *
 * Keeping a separate type improves readability when an integer value
 * represents a position rather than a measurement.
 */

typedef uint16_t index_t;


/******************************************************************************
 *                              End of File
 ******************************************************************************/


#ifdef __cplusplus
}
#endif


#endif /* APP_TYPES_H */
