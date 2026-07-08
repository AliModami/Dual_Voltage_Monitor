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
 *                          Boolean Compatibility
 ******************************************************************************/

/*
 * Modern embedded projects should use the C99 bool type.
 *
 * Example:
 *
 * bool alarm_enabled;
 * bool lcd_ready;
 */

typedef bool bool_t;

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
 */

typedef uint16_t adc_raw_t;

/******************************************************************************
 *                          Millivolt Type
 ******************************************************************************/

/*
 * Internal calculations often use millivolts
 * instead of floating-point values.
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
 */

typedef uint32_t counter_t;

/******************************************************************************
 *                          Index Type
 ******************************************************************************/

/*
 * Used for indexing arrays and tables.
 */

typedef uint16_t index_t;

/******************************************************************************
 *                          Size Type
 ******************************************************************************/

/*
 * Used when expressing the number of elements.
 */

//typedef uint16_t size_t16;

/******************************************************************************
 *                              End of File
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APP_TYPES_H */
