/******************************************************************************
 * @file    project_config.h
 * @brief   Global Project Configuration
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Target MCU :
 *      STM32F103C8T6 (Blue Pill)
 *
 * Framework :
 *      STM32 HAL
 *
 * IDE :
 *      STM32CubeIDE
 *
 * Description :
 *      This file contains project-wide configuration values that are shared
 *      between all software modules.
 *
 *      IMPORTANT DESIGN RULES
 *      ----------------------
 *      1. Never place application variables here.
 *      2. Never place function implementations here.
 *      3. Never include unnecessary header files.
 *      4. Only global compile-time configuration belongs here.
 *      5. Every configurable constant should have a meaningful comment.
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
 *
 * Change Log
 *-----------------------------------------------------------------------------
 * Version  Date         Description
 *-----------------------------------------------------------------------------
 * 1.0.0    2026-07-08   Initial version.
 *
 ******************************************************************************/

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

/*
 * stdint.h provides fixed-width integer types such as uint8_t, uint16_t,
 * uint32_t, etc.
 *
 * Embedded software should always use fixed-width integer types instead of
 * compiler-dependent types like int or long whenever exact size matters.
 */
#include <stdint.h>

/******************************************************************************
 *                        Project Identification
 ******************************************************************************/

/*
 * These strings are used only for displaying software information.
 * They do NOT affect program behavior.
 */

#define PROJECT_NAME                "Dual Voltage Monitor"
#define PROJECT_SHORT_NAME          "DVM"

#define PROJECT_AUTHOR              "Ali Modami"

#define PROJECT_COMPANY             "Personal Project"

#define PROJECT_VERSION_MAJOR       1U
#define PROJECT_VERSION_MINOR       0U
#define PROJECT_VERSION_PATCH       0U

/******************************************************************************
 *                        Hardware Information
 ******************************************************************************/

/*
 * CPU Frequency
 *
 * The STM32F103C8T6 is configured to run at 72 MHz using:
 *
 * HSE = 8 MHz
 * PLL = x9
 */

#define CPU_CLOCK_HZ                72000000UL

/******************************************************************************
 *                         ADC Configuration
 ******************************************************************************/

/*
 * ADC Reference Voltage
 *
 * The Blue Pill ADC measures voltages relative to VDDA.
 * Normally VDDA = 3.300V.
 */

#define ADC_REFERENCE_VOLTAGE_MV    3300U

/*
 * ADC Resolution
 *
 * STM32F103 ADC = 12-bit
 */

#define ADC_RESOLUTION_BITS         12U

/*
 * Maximum ADC Value
 *
 * 12-bit ADC:
 *
 * 0 .... 4095
 */

#define ADC_MAX_VALUE               4095U

/******************************************************************************
 *                        Voltage Measurement
 ******************************************************************************/

/*
 * Expected operating range.
 *
 * These values are NOT alarm limits.
 * They simply describe the design range.
 */

#define INPUT_VOLTAGE_MIN_V         0.0f
#define INPUT_VOLTAGE_MAX_V         300.0f

#define OUTPUT_VOLTAGE_MIN_V        0.0f
#define OUTPUT_VOLTAGE_MAX_V        300.0f

/******************************************************************************
 *                        LCD Configuration
 ******************************************************************************/

/*
 * LCD Type
 *
 * Current hardware:
 *
 * 20 columns
 * 4 rows
 */

#define LCD_COLUMNS                 20U
#define LCD_ROWS                    4U

/******************************************************************************
 *                        Button Configuration
 ******************************************************************************/

/*
 * Number of physical keys.
 */

#define BUTTON_COUNT                4U

/******************************************************************************
 *                        Timing Configuration
 ******************************************************************************/

/*
 * Default system update period.
 *
 * Most software modules can execute their periodic tasks using this interval.
 *
 * Unit:
 *      milliseconds
 */

#define SYSTEM_TICK_PERIOD_MS       10U

/******************************************************************************
 *                        Monitor Refresh
 ******************************************************************************/

/*
 * LCD refresh period.
 *
 * Updating the LCD too frequently causes visible flickering.
 */

#define LCD_REFRESH_PERIOD_MS       200U

/******************************************************************************
 *                        Buzzer Configuration
 ******************************************************************************/

/*
 * Default buzzer beep duration.
 */

#define BUZZER_DEFAULT_BEEP_MS      80U

/******************************************************************************
 *                        UART Configuration
 ******************************************************************************/

/*
 * Default UART baud rate.
 *
 * This value may later become user-configurable.
 */

#define UART_DEFAULT_BAUDRATE       115200UL

/******************************************************************************
 *                        Stream Configuration
 ******************************************************************************/

/*
 * Default sampling interval.
 *
 * Unit:
 *      milliseconds
 */

#define STREAM_DEFAULT_PERIOD_MS    100U

/******************************************************************************
 *                        Boolean Definitions
 *
 * These constants improve readability when writing configuration values.
 ******************************************************************************/

#ifndef TRUE
#define TRUE                        (1U)
#endif

#ifndef FALSE
#define FALSE                       (0U)
#endif

/******************************************************************************
 *                      Utility Macros
 ******************************************************************************/

/*
 * ARRAY_SIZE()
 *
 * Returns the number of elements inside a static array.
 *
 * Example:
 *
 * int data[10];
 *
 * ARRAY_SIZE(data) == 10
 */

#define ARRAY_SIZE(x) \
    (sizeof(x) / sizeof((x)[0]))

/*
 * UNUSED()
 *
 * Prevents compiler warnings for intentionally unused parameters.
 */

#define UNUSED(x) \
    ((void)(x))

/******************************************************************************
 *                          Future Extension Area
 *
 * New project-wide configuration values should be added here.
 *
 * Examples:
 *
 * - Flash addresses
 * - Calibration constants
 * - Default thresholds
 * - Timeout values
 * - Menu settings
 *
 ******************************************************************************/

/******************************************************************************
 *                              End of File
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PROJECT_CONFIG_H */
