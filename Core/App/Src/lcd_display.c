/******************************************************************************
 * @file    lcd_display.c
 * @brief   Character LCD Display Abstraction Layer Implementation
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module provides a simplified display interface
 *      for application modules.
 *
 *      It hides the low-level LCD driver API from upper layers.
 *
 *      Upper modules should use:
 *
 *          lcd_display.h
 *
 *      instead of directly accessing:
 *
 *          lcd_i2c.h
 *
 *-----------------------------------------------------------------------------
 *
 * Architecture:
 *
 *          menu_renderer.c
 *                 |
 *                 v
 *
 *          lcd_display.c
 *                 |
 *                 v
 *
 *          lcd_i2c.c
 *
 *-----------------------------------------------------------------------------
 *
 * Responsibilities:
 *
 *      This module:
 *
 *          - Forwards display requests to LCD driver
 *          - Keeps application code independent from driver API
 *
 *
 *      This module does NOT:
 *
 *          - Manage LCD hardware
 *          - Access I2C
 *          - Use HAL functions
 *          - Control application screens
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

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include "lcd_display.h"

#include "lcd_i2c.h"



/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize LCD display abstraction layer.
 *
 * @details
 *      Currently no action is required.
 *
 *      The LCD hardware initialization is performed by:
 *
 *          LCD_Init()
 *
 *      from the low-level LCD driver.
 */
void LCD_Display_Init(void)
{
    /*
     * Reserved for future display-level configuration.
     */
}



/**
 * @brief
 *      Clear complete LCD display.
 */
void LCD_Display_Clear(void)
{
    LCD_Clear();
}



/**
 * @brief
 *      Set cursor position.
 *
 * @param row
 *      LCD row number.
 *
 * @param column
 *      LCD column number.
 */
void LCD_Display_SetCursor(uint8_t row,
                           uint8_t column)
{
    LCD_SetCursor(row,
                  column);
}



/**
 * @brief
 *      Write text string.
 *
 * @param text
 *      Null terminated string.
 */
void LCD_Display_WriteText(const char *text)
{
    LCD_Print(text);
}



/**
 * @brief
 *      Write single character.
 *
 * @param character
 *      Character to display.
 */
void LCD_Display_WriteChar(char character)
{
    LCD_PrintChar(character);
}


/******************************************************************************
 *                              End of File
 ******************************************************************************/
