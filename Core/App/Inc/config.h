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
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file contains all global configuration definitions used by the
 *      application.
 *
 *      All configurable parameters are centralized here.
 *
 *      Version:
 *
 *          config.h v2.0.3
 *
 *      Added:
 *
 *          - Vin Offset Calibration
 *          - Vout Offset Calibration
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


#define PROJECT_NAME                       "Dual Voltage Monitor"


#define FIRMWARE_VERSION_MAJOR             2U
#define FIRMWARE_VERSION_MINOR             0U
#define FIRMWARE_VERSION_PATCH             3U





/*
 * ============================================================================
 * LCD Configuration
 * ============================================================================
 */


#define LCD_DEFAULT_COLUMNS                20U

#define LCD_DEFAULT_ROWS                   4U

#define LCD_I2C_ADDRESS_DEFAULT            0x27U





/*
 * ============================================================================
 * Menu Configuration
 * ============================================================================
 */


#define MENU_VISIBLE_ITEMS                 3U





/*
 * ============================================================================
 * UART Configuration
 * ============================================================================
 */


#define UART_BAUD_RATE_MIN                 9600U

#define UART_BAUD_RATE_DEFAULT             9600U

#define UART_BAUD_RATE_MAX                 115200U





/*
 * ============================================================================
 * Sample Rate Configuration
 * ============================================================================
 */


#define SAMPLE_RATE_MIN_MS                 100U

#define SAMPLE_RATE_DEFAULT_MS             100U

#define SAMPLE_RATE_MAX_MS                 2000U


#define SAMPLE_RATE_STEP_MS                100U





/*
 * ============================================================================
 * ADC Configuration
 * ============================================================================
 */


#define ADC_FILTER_SIZE                    8U





/*
 * ============================================================================
 * Flash Configuration
 * ============================================================================
 */


#define CONFIG_FLASH_ADDRESS               0x0800FC00UL

#define CONFIG_MAGIC_NUMBER                0x12345678UL





/*
 * ============================================================================
 * Alarm Configuration
 * ============================================================================
 */


typedef enum
{

    CONFIG_ALARM_ONCE = 0U,


    CONFIG_ALARM_REPEAT


} ConfigAlarmMode_t;



#define CONFIG_ALARM_ENABLE_DEFAULT        1U


#define CONFIG_ALARM_MODE_DEFAULT          CONFIG_ALARM_REPEAT





/*
 * ============================================================================
 * Voltage Threshold Configuration
 * ============================================================================
 */


#define CONFIG_LOW_VOLTAGE_DEFAULT         180U


#define CONFIG_HIGH_VOLTAGE_DEFAULT        260U



#define CONFIG_VOLTAGE_LIMIT_MIN           0U


#define CONFIG_VOLTAGE_LIMIT_MAX           300U


#define CONFIG_VOLTAGE_STEP                5U





/*
 * ============================================================================
 * Voltage Calibration Offset Configuration
 * ============================================================================
 *
 * Unit:
 *
 *      0.1 Volt
 *
 *
 * Storage format:
 *
 *      -200 ... +200
 *
 *
 * Examples:
 *
 *      +12.5V  -> 125
 *
 *      -3.2V   -> -32
 *
 */


#define CONFIG_OFFSET_MIN                  (-200)


#define CONFIG_OFFSET_MAX                  (200)


#define CONFIG_OFFSET_STEP                 (1)



#define CONFIG_VIN_OFFSET_DEFAULT          0


#define CONFIG_VOUT_OFFSET_DEFAULT         0



/*
 * ============================================================================
 * Validation Limits
 * ============================================================================
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


#define CONFIG_STREAM_DEFAULT_ENABLE       0U


#define CONFIG_BUZZER_DEFAULT_ENABLE       1U





/*
* ============================================================================
* Runtime Configuration API
* ============================================================================
*/


/*
* Configuration Manager
*/
void Config_Init(void);


void Config_ResetDefault(void);





/*
* ============================================================================
* Storage Interface
* ============================================================================
*/


void Config_Save(void);


void Config_Load(void);





/*
* ============================================================================
* Baud Rate API
* ============================================================================
*/


uint32_t Config_GetBaudRate(void);


void Config_SetBaudRate(uint32_t baud_rate);





/*
* ============================================================================
* Sample Rate API
* ============================================================================
*/


uint32_t Config_GetSampleRate(void);


void Config_SetSampleRate(uint32_t sample_rate_ms);





/*
* ============================================================================
* Alarm Enable API
* ============================================================================
*/


uint8_t Config_GetAlarmEnable(void);


void Config_SetAlarmEnable(uint8_t enable);





/*
* ============================================================================
* Voltage Threshold API
* ============================================================================
*/


uint16_t Config_GetLowVoltageLimit(void);


void Config_SetLowVoltageLimit(uint16_t limit);



uint16_t Config_GetHighVoltageLimit(void);


void Config_SetHighVoltageLimit(uint16_t limit);





/*
* ============================================================================
* Alarm Mode API
* ============================================================================
*/


ConfigAlarmMode_t Config_GetAlarmMode(void);


void Config_SetAlarmMode(ConfigAlarmMode_t mode);





/*
* ============================================================================
* Voltage Calibration Offset API
* ============================================================================
*
* Unit:
*
*      0.1 Volt
*
*
* Example:
*
*      Stored value:
*
*          25
*
*      Display:
*
*          +2.5V
*
*/


int16_t Config_GetVinOffset(void);


void Config_SetVinOffset(int16_t offset);



int16_t Config_GetVoutOffset(void);


void Config_SetVoutOffset(int16_t offset);





/*
* ============================================================================
* Stream Feature API
* ============================================================================
*/


uint8_t Config_GetStreamEnable(void);


void Config_SetStreamEnable(uint8_t enable);





#ifdef __cplusplus
}
#endif


#endif /* CONFIG_H */





/******************************************************************************
*
*                              END OF FILE
*
*      File:
*
*          config.h
*
*      Version:
*
*          v2.0.3
*
*      Added:
*
*          - Vin Offset calibration
*          - Vout Offset calibration
*
*      Offset Range:
*
*          -20.0V ... +20.0V
*
*      Offset Step:
*
*          0.1V
*
******************************************************************************/
