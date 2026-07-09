/******************************************************************************
 * @file    lcd_display.h
 * @brief   Character LCD Display Abstraction Layer Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module provides a simple display interface for application
 *      modules that need to write information on the character LCD.
 *
 *      The purpose of this layer is to separate application rendering logic
 *      from the low-level LCD driver implementation.
 *
 *      Application modules should NOT directly include:
 *
 *          lcd_i2c.h
 *
 *      Instead, they should use this interface.
 *
 *-----------------------------------------------------------------------------
 *
 * Architecture:
 *
 *          Application Layer
 *
 *              |
 *              v
 *
 *          Menu Renderer
 *
 *              |
 *              v
 *
 *          lcd_display
 *
 *              |
 *              v
 *
 *          lcd_i2c Driver
 *
 *              |
 *              v
 *
 *          I2C Hardware
 *
 *-----------------------------------------------------------------------------
 *
 * Responsibilities:
 *
 *      This module provides:
 *
 *          - Clear display
 *          - Move cursor
 *          - Write text
 *          - Write single character
 *
 *
 *      This module does NOT:
 *
 *          - Handle menu logic
 *          - Process buttons
 *          - Format application data
 *          - Access HAL directly
 *          - Communicate with I2C directly
 *
 *-----------------------------------------------------------------------------
 *
 * Design Rules:
 *
 *      Rule 1
 *      -------
 *      Application code must not depend on LCD driver details.
 *
 *
 *      Rule 2
 *      -------
 *      Text generation belongs to application modules.
 *
 *
 *      Rule 3
 *      -------
 *      Low-level LCD communication belongs to lcd_i2c driver.
 *
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & ChatGPT
 *
 * Version :
 *      1.0.0
 *
 * Created :
 *      2026-07-09
 *
 * Change History :
 *
 *      1.0.0
 *          Initial implementation.
 *
 ******************************************************************************/

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H


#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>



/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize LCD display abstraction layer.
 *
 * @details
 *      This function does not initialize hardware.
 *
 *      Hardware initialization remains inside:
 *
 *          LCD_Init()
 *
 *      from lcd_i2c driver.
 *
 *      This function is reserved for future display-level
 *      configuration.
 */
void LCD_Display_Init(void);



/**
 * @brief
 *      Clear complete LCD display.
 *
 * @details
 *      Removes all characters from the display.
 */
void LCD_Display_Clear(void);



/**
 * @brief
 *      Set LCD cursor position.
 *
 * @param row
 *      LCD row number.
 *
 * @param column
 *      LCD column number.
 */
void LCD_Display_SetCursor(uint8_t row,
                           uint8_t column);



/**
 * @brief
 *      Write text string to LCD.
 *
 * @param text
 *      Null terminated string.
 */
void LCD_Display_WriteText(const char *text);



/**
 * @brief
 *      Write single character to LCD.
 *
 * @param character
 *      Character to display.
 */
void LCD_Display_WriteChar(char character);



#ifdef __cplusplus
}
#endif


#endif /* LCD_DISPLAY_H */
