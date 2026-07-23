/******************************************************************************
 *
 * @file    lcd_display.h
 *
 * @brief   LCD Display Abstraction Layer Interface
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
 *      This module provides the application display interface.
 *
 *      Responsibilities:
 *
 *      - Initialize display layer.
 *      - Display titles.
 *      - Display text lines.
 *      - Render menu pages.
 *
 *
 *      This module does not contain:
 *
 *      - LCD hardware control
 *      - I2C communication
 *      - HD44780 commands
 *
 *
 * Architecture:
 *
 *
 *      menu_renderer
 *            |
 *            v
 *
 *      lcd_display
 *            |
 *            v
 *
 *      lcd_i2c
 *
 *
 ******************************************************************************/

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "menu_renderer.h"



/*
 * Initialize LCD display layer.
 *
 * This function initializes:
 *
 *      - LCD hardware driver
 *      - Display context
 *
 *
 * Example:
 *
 *      LCD_Display_Init(&hi2c1);
 *
 */
void LCD_Display_Init(
        I2C_HandleTypeDef *hi2c);



/*
 * Show title line.
 *
 * LCD row 0 is reserved for title.
 *
 * Example:
 *
 *      LCD_Display_ShowTitle("MAIN MENU");
 *
 */
void LCD_Display_ShowTitle(
        const char *title);



/*
 * Print text on selected LCD row.
 *
 * Row range:
 *
 *      0 ... 3
 *
 *
 * Example:
 *
 *      LCD_Display_PrintLine(
 *              1,
 *              "Live Monitor");
 *
 */
void LCD_Display_PrintLine(
        uint8_t row,
        const char *text);



/*
 * Clear complete display.
 *
 */
void LCD_Display_Clear(void);



/*
 * Render complete menu screen.
 *
 * Data source:
 *
 *      menu_renderer
 *
 *
 * Display format:
 *
 *
 *      Line 0:
 *          Page title
 *
 *      Line 1-3:
 *          Menu items
 *
 *
 * Example output:
 *
 *
 *      Main Menu
 *      >Live Monitor
 *       Start Stream
 *       Settings
 *
 */
void LCD_Display_RenderMenu(void);



/*
 * Get internal LCD handle.
 *
 * Useful for advanced application access.
 *
 */
//void *LCD_Display_GetHandle(void);



#ifdef __cplusplus
}
#endif


#endif /* LCD_DISPLAY_H */


