/******************************************************************************
 *
 * @file    config.h
 *
 * @brief   Application Configuration Definitions
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 ******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H


#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include <stdbool.h>



/*
 * ============================================================================
 * Project Information
 * ============================================================================
 */

#define PROJECT_NAME                "Dual Voltage Monitor"


#define FIRMWARE_VERSION_MAJOR      1U
#define FIRMWARE_VERSION_MINOR      0U
#define FIRMWARE_VERSION_PATCH      3U





/*
 * ============================================================================
 * LCD Configuration
 * ============================================================================
 */

#define LCD_DEFAULT_COLUMNS         20U
#define LCD_DEFAULT_ROWS            4U

#define LCD_I2C_ADDRESS_DEFAULT     0x27U





/*
 * ============================================================================
 * Menu Configuration
 * ============================================================================
 */

#define MENU_VISIBLE_ITEMS          3U





/*
 * ============================================================================
 * UART Configuration
 * ============================================================================
 */

#define UART_BAUD_RATE_DEFAULT      9600U

#define UART_BAUD_RATE_MIN          9600U
#define UART_BAUD_RATE_MAX          115200U





/*
 * ============================================================================
 * Sample Rate Configuration
 * ============================================================================
 */

#define SAMPLE_RATE_DEFAULT_MS      100U

#define SAMPLE_RATE_MIN_MS          100U
#define SAMPLE_RATE_MAX_MS          2000U

#define SAMPLE_RATE_STEP_MS         100U





/*
 * ============================================================================
 * Voltage Threshold Configuration
 * ============================================================================
 */

#define VOLTAGE_LOW_LIMIT_DEFAULT   180U

#define VOLTAGE_HIGH_LIMIT_DEFAULT  260U


#define VOLTAGE_LIMIT_MIN           0U
#define VOLTAGE_LIMIT_MAX           300U

#define VOLTAGE_LIMIT_STEP          5U





/*
 * ============================================================================
 * Alarm Configuration
 * ============================================================================
 */


/*
 * Alarm operation mode.
 *
 * ONCE:
 *      Generate alarm once.
 *
 * REPEAT:
 *      Repeat alarm while fault exists.
 */
typedef enum
{

    CONFIG_ALARM_ONCE = 0U,

    CONFIG_ALARM_REPEAT

} ConfigAlarmMode_t;



#define ALARM_ENABLE_DEFAULT        1U

#define ALARM_MODE_DEFAULT          CONFIG_ALARM_REPEAT



/*
 * ============================================================================
 * ADC Configuration
 * ============================================================================
 */

#define ADC_FILTER_SIZE             8U





/*
 * ============================================================================
 * Flash Configuration
 * ============================================================================
 */

#define CONFIG_FLASH_ADDRESS        0x0800FC00U

#define CONFIG_MAGIC_NUMBER         0x12345678U


/*
 * ============================================================================
 * Alarm Configuration
 * ============================================================================
 */


/*
 * Alarm enable default state.
 */
#define CONFIG_ALARM_ENABLE_DEFAULT       1U



/*
 * Alarm mode definition.
 *
 * Supported modes:
 *
 *      Once:
 *          Alarm triggers one time.
 *
 *      Repeat:
 *          Alarm repeats while condition exists.
 */
#define CONFIG_ALARM_ONCE                 0U

#define CONFIG_ALARM_REPEAT               1U


/*
 * Default alarm mode.
 */
#define CONFIG_ALARM_MODE_DEFAULT         CONFIG_ALARM_ONCE





/*
 * ============================================================================
 * Voltage Threshold Configuration
 * ============================================================================
 */


/*
 * Voltage alarm limits.
 *
 * Unit:
 *
 *      Volt
 */
#define CONFIG_LOW_VOLTAGE_DEFAULT        180U

#define CONFIG_HIGH_VOLTAGE_DEFAULT       260U



/*
 * Allowed voltage range.
 */
#define CONFIG_VOLTAGE_LIMIT_MIN          0U

#define CONFIG_VOLTAGE_LIMIT_MAX          300U


/*
 * Adjustment step.
 */
#define CONFIG_VOLTAGE_STEP               5U





/*
 * ============================================================================
 * Configuration Validation
 * ============================================================================
 */


/*
 * Runtime configuration validation limits.
 */
#define CONFIG_SAMPLE_RATE_MIN             SAMPLE_RATE_MIN_MS

#define CONFIG_SAMPLE_RATE_MAX             SAMPLE_RATE_MAX_MS


#define CONFIG_BAUD_RATE_MIN               UART_BAUD_RATE_MIN

#define CONFIG_BAUD_RATE_MAX               UART_BAUD_RATE_MAX





/*
 * ============================================================================
 * Feature Defaults
 * ============================================================================
 */


/*
 * Stream feature.
 */
#define CONFIG_STREAM_DEFAULT_ENABLE       0U



/*
 * Buzzer feature.
 */
#define CONFIG_BUZZER_DEFAULT_ENABLE       1U





/*
 * ============================================================================
 * End Of Public Configuration Definitions
 * ============================================================================
 */


#ifdef __cplusplus
}
#endif

/*
 * Runtime Configuration API
 */


/* Baud Rate */
uint32_t Config_GetBaudRate(void);
void Config_SetBaudRate(uint32_t baud_rate);


/* Sample Rate */
uint32_t Config_GetSampleRate(void);
void Config_SetSampleRate(uint32_t sample_rate_ms);


/* Alarm Enable */
uint8_t Config_GetAlarmEnable(void);
void Config_SetAlarmEnable(uint8_t enable);


/* Voltage Thresholds */
uint16_t Config_GetLowVoltageLimit(void);
void Config_SetLowVoltageLimit(uint16_t limit);


uint16_t Config_GetHighVoltageLimit(void);
void Config_SetHighVoltageLimit(uint16_t limit);


/* Alarm Mode */
uint8_t Config_GetAlarmMode(void);
void Config_SetAlarmMode(uint8_t mode);


/* Stream Enable */
uint8_t Config_GetStreamEnable(void);
void Config_SetStreamEnable(uint8_t enable);

#endif /* CONFIG_H */


/******************************************************************************
 *
 *                              END OF FILE
 *
 ******************************************************************************/
