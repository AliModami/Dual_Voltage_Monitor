/**
 ******************************************************************************
 * @file    app_defines.h
 * @brief   Application global definitions.
 *
 * @details
 * This file contains definitions that are shared across the whole application.
 *
 * Only truly global definitions should be placed here.
 *
 * Module-specific configurations must be placed in their own configuration
 * files.
 *
 * Example:
 *
 * ADC settings  -> adc_config.h
 * LCD settings  -> lcd_config.h
 * UART settings -> uart_config.h
 *
 * This separation keeps the project scalable and prevents configuration files
 * from becoming mixed together.
 *
 ******************************************************************************
 * @project Dual Voltage Monitor
 * @version 1.0.0
 * @author Ali Modami
 ******************************************************************************
 */

#ifndef APP_DEFINES_H
#define APP_DEFINES_H


/* Includes ------------------------------------------------------------------*/


/* Project information -------------------------------------------------------*/

/**
 * @brief Application name.
 */
#define PROJECT_NAME                 "Dual Voltage Monitor"


/**
 * @brief Current firmware version.
 *
 * Version format:
 * Major.Minor.Patch
 */
#define PROJECT_VERSION              "1.0.0"



/* Application general settings ----------------------------------------------*/

/**
 * @brief
 * Enable or disable debug features.
 *
 * 1 = Debug enabled
 * 0 = Debug disabled
 */
#define APP_DEBUG_ENABLE             1



/* Common mathematical definitions --------------------------------------------*/

#define APP_TRUE                     1U

#define APP_FALSE                    0U



/* Common limits --------------------------------------------------------------*/

#define APP_MAX_NAME_LENGTH          32U



#endif /* APP_DEFINES_H */
