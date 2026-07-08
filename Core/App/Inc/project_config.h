/******************************************************************************
 * @file    project_config.h
 * @brief   Global project configuration file
 *
 *----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * MCU :
 *      STM32F103C8T6 (Blue Pill)
 *
 * Author :
 *      Ali Modami
 *
 * Description :
 *      This file contains global project definitions.
 *
 *      The purpose of this file is to collect common project information
 *      in one location, so future maintenance becomes much easier.
 *
 *      Every application module may include this file instead of defining
 *      duplicated values in multiple places.
 *
 *----------------------------------------------------------------------------
 * Change History
 *
 * Version | Date       | Description
 * --------+------------+----------------------------------------------
 * 1.0.0   | 2026-07-08 | Initial project configuration template
 *
 ******************************************************************************/

#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*==========================================================================*/
/*                              Project Info                                */
/*==========================================================================*/

#define PROJECT_NAME               "Dual Voltage Monitor"

#define PROJECT_VERSION_MAJOR      1
#define PROJECT_VERSION_MINOR      0
#define PROJECT_VERSION_PATCH      0

/*==========================================================================*/
/*                           Future Configuration                           */
/*==========================================================================*/

/*
 * Future project-wide configuration values will be placed here.
 *
 * Examples:
 *
 * LCD Type
 * ADC Filter Size
 * UART Baudrate
 * Flash Address
 * Sample Time
 * Debug Enable
 * etc...
 */

/*==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* PROJECT_CONFIG_H */
