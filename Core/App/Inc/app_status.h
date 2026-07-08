/**
 ******************************************************************************
 * @file    app_status.h
 * @brief   Application common status definitions.
 *
 * @details
 * This file contains the common return status codes used by all application
 * modules.
 *
 * The purpose of this file is to create a unified communication language
 * between software modules.
 *
 * Example:
 * ADC_Init()      -> APP_STATUS_OK
 * LCD_Init()      -> APP_STATUS_ERROR
 * UART_SendData() -> APP_STATUS_TIMEOUT
 *
 * Module-specific states must NOT be placed here.
 * For example:
 * BUTTON_PRESSED, LCD_BUSY, ADC_READY
 * belong to their own modules.
 *
 ******************************************************************************
 * @project Dual Voltage Monitor
 * @version 1.0.0
 * @author Ali Modami
 ******************************************************************************
 */

#ifndef APP_STATUS_H
#define APP_STATUS_H


/* Includes ------------------------------------------------------------------*/

#include <stdint.h>


/* Exported types ------------------------------------------------------------*/

/**
 * @brief
 * Application general operation result.
 *
 * All application modules return this type when reporting
 * the result of an operation.
 */
typedef enum
{
    APP_STATUS_OK = 0,

    APP_STATUS_ERROR,

    APP_STATUS_BUSY,

    APP_STATUS_TIMEOUT,

    APP_STATUS_INVALID_PARAMETER,

    APP_STATUS_NOT_INITIALIZED,

    APP_STATUS_OUT_OF_RANGE,

    APP_STATUS_NOT_SUPPORTED

} app_status_t;


/* Exported constants --------------------------------------------------------*/


/* Exported macros -----------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/


#endif /* APP_STATUS_H */
