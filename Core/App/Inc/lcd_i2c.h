/******************************************************************************
 * @file    lcd_i2c.h
 * @brief   Character LCD I2C driver public interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file provides the public interface for controlling HD44780
 *      compatible character LCD modules connected through PCF8574 I2C
 *      expanders.
 *
 *      Supported displays:
 *
 *          - LCD 20x4
 *          - LCD 16x2
 *
 *      This module hides all low level hardware details from the application.
 *
 *      Application modules should only use the functions provided here.
 *
 *      Example:
 *
 *          Menu Renderer
 *                 |
 *                 v
 *              LCD API
 *                 |
 *                 v
 *            I2C Hardware
 *
 *-----------------------------------------------------------------------------
 * Design Rules:
 *
 *      1. Application code must never access HAL_I2C functions directly
 *         for LCD communication.
 *
 *      2. LCD timing and communication details remain private to driver.
 *
 *      3. Driver must remain independent from Menu and Display logic.
 *
 *      4. All LCD operations must be non-destructive to other modules.
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
 ******************************************************************************/



#ifndef LCD_I2C_H
#define LCD_I2C_H



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
 *                              LCD Cursor Modes
 ******************************************************************************/

/*
 * Cursor display options.
 *
 * These values are independent from LCD controller commands.
 */

typedef enum
{
    LCD_CURSOR_OFF = 0,

    LCD_CURSOR_ON,

    LCD_CURSOR_BLINK


} LCD_CursorMode_t;



/******************************************************************************
 *                              LCD Display Modes
 ******************************************************************************/

/*
 * Display state control.
 */

typedef enum
{
    LCD_DISPLAY_OFF = 0,

    LCD_DISPLAY_ON


} LCD_DisplayState_t;



/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize LCD module.
 *
 * @details
 *      This function performs:
 *
 *          - LCD initialization sequence
 *          - 4-bit communication setup
 *          - Display configuration
 *          - Backlight activation
 *
 *      I2C peripheral must already be initialized by CubeMX.
 *
 * @note
 *      Call this function once after MX_I2C1_Init().
 */
void LCD_Init(void);



/**
 * @brief
 *      Clear entire LCD display.
 *
 * @details
 *      Removes all characters and returns cursor
 *      position to the beginning.
 */
void LCD_Clear(void);



/**
 * @brief
 *      Move cursor to selected position.
 *
 * @param row
 *      LCD row number.
 *
 *      Range depends on LCD size:
 *
 *          LCD 20x4:
 *              0 ... 3
 *
 *          LCD 16x2:
 *              0 ... 1
 *
 * @param column
 *      Character position inside selected row.
 */
void LCD_SetCursor(uint8_t row,
                   uint8_t column);



/**
 * @brief
 *      Print string on LCD.
 *
 * @param text
 *      Pointer to null terminated string.
 */
void LCD_Print(const char *text);



/**
 * @brief
 *      Print single character.
 *
 * @param character
 *      Character to display.
 */
void LCD_PrintChar(char character);



/**
 * @brief
 *      Control LCD display state.
 *
 * @param state
 *      LCD_DISPLAY_ON or LCD_DISPLAY_OFF
 */
void LCD_DisplayControl(LCD_DisplayState_t state);



/**
 * @brief
 *      Control cursor visibility.
 *
 * @param mode
 *      Cursor display mode.
 */
void LCD_CursorControl(LCD_CursorMode_t mode);



/**
 * @brief
 *      Enable LCD backlight.
 */
void LCD_BacklightOn(void);



/**
 * @brief
 *      Disable LCD backlight.
 */
void LCD_BacklightOff(void);



/**
 * @brief
 *      Print formatted integer value.
 *
 * @param value
 *      Integer value to display.
 *
 * @details
 *      This helper function avoids repeated
 *      sprintf usage in application modules.
 */
void LCD_PrintInt(int32_t value);



#ifdef __cplusplus
}
#endif



#endif /* LCD_I2C_H */
