/******************************************************************************
 *
 * @file    screen_manager.c
 *
 * @brief   Application Screen Manager Implementation
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
 *      This module controls application level screens.
 *
 *      Responsibilities:
 *
 *      - Manage active screen.
 *      - Render selected application screen.
 *      - Connect application data to display layer.
 *
 *
 *      This module does NOT handle:
 *
 *      - Button hardware.
 *      - Menu navigation.
 *      - ADC hardware access.
 *      - LCD low level driver.
 *
 ******************************************************************************/

#include "screen_manager.h"

#include "menu_renderer.h"
#include "lcd_display.h"
#include "menu_edit.h"
#include "adc_app.h"
#include "config.h"
#include <stdio.h>



/*
 * ============================================================================
 * Private Variables
 * ============================================================================
 */


/*
 * Current active screen.
 */
static ScreenId_t current_screen;



/*
 * Previous live monitor values.
 *
 * Used to avoid unnecessary LCD refresh.
 */
static Voltage_t previous_vin  = 0xFFFFU;

static Voltage_t previous_vout = 0xFFFFU;



/*
 * ============================================================================
 * Private Functions
 * ============================================================================
 */


/*
 * Render live monitor screen.
 *
 * LCD Layout:
 *
 *      Row 0:
 *          Live Monitor
 *
 *      Row 1:
 *          VIN
 *
 *      Row 2:
 *          VOUT
 *
 *      Row 3:
 *          Status
 *
 */
static void ScreenManager_RenderLiveMonitor(void)
{

    char line[21];


    Voltage_t vin;

    Voltage_t vout;



    /*
     * Read latest ADC values.
     */
    vin  = ADC_App_GetVin();

    vout = ADC_App_GetVout();




    /*
     * Update title.
     *
     * Title is written only when
     * display content changes.
     */
    if((vin  != previous_vin) ||
       (vout != previous_vout))
    {

    	LCD_Display_ShowTitle(
    	        " ***Live Monitor***");



    	snprintf(
    	        line,
    	        sizeof(line),
    	        "Vinput : %u.%u V",
    	        (unsigned int)(vin / 10U),
    	        (unsigned int)(vin % 10U));


    	LCD_Display_PrintLine(
    	        1U,
    	        line);



    	snprintf(
    	        line,
    	        sizeof(line),
    	        "Voutput: %u.%u V",
    	        (unsigned int)(vout / 10U),
    	        (unsigned int)(vout % 10U));


    	LCD_Display_PrintLine(
    	        2U,
    	        line);


    	if(Config_GetAlarmEnable())
    	{
    	    uint16_t low;
    	    uint16_t high;


    	    low =
    	        Config_GetLowVoltageLimit();


    	    high =
    	        Config_GetHighVoltageLimit();



    	    if(vout < (low * 10U))
    	    {
    	        LCD_Display_PrintLine(
    	                3U,
    	                "Status: LOW Voltage");
    	    }
    	    else if(vout > (high * 10U))
    	    {
    	        LCD_Display_PrintLine(
    	                3U,
    	                "Status: HIGH Voltage");
    	    }
    	    else
    	    {
    	        LCD_Display_PrintLine(
    	                3U,
    	                "Status: OK");
    	    }
    	}
    	else
    	{
    	    LCD_Display_PrintLine(
    	            3U,
    	            "Status: OK");
    	}



        /*
         * Store current values.
         */
        previous_vin  = vin;

        previous_vout = vout;

    }

}



/*
 * ============================================================================
 * Public Functions
 * ============================================================================
 */


/*
 * Initialize screen manager.
 */
void ScreenManager_Init(void)
{

    current_screen = SCREEN_MENU;



    previous_vin  = 0xFFFFU;

    previous_vout = 0xFFFFU;

}


/*
 * Change active screen.
 */
void ScreenManager_SetScreen(
        ScreenId_t screen)
{

    current_screen = screen;


    /*
     * Force refresh when
     * entering live monitor.
     */
    if(screen == SCREEN_LIVE_MONITOR)
    {

        previous_vin  = 0xFFFFU;

        previous_vout = 0xFFFFU;

    }

}




/*
 * Get current active screen.
 */
ScreenId_t ScreenManager_GetScreen(void)
{

    return current_screen;

}





/*
 * Render current screen.
 *
 * Called from main loop.
 */
void ScreenManager_Render(void)
{

    switch(current_screen)
    {



        /*
         * ------------------------------------------------------------
         * Menu Screen
         * ------------------------------------------------------------
         */
        case SCREEN_MENU:


            MenuRenderer_Update();

            LCD_Display_RenderMenu();


            break;





        /*
         * ------------------------------------------------------------
         * Live Monitor Screen
         * ------------------------------------------------------------
         */
        case SCREEN_LIVE_MONITOR:


            ScreenManager_RenderLiveMonitor();


            break;





        /*
         * ------------------------------------------------------------
         * Stream Screen
         * ------------------------------------------------------------
         */
        case SCREEN_STREAM:


            LCD_Display_ShowTitle(
                    "UART Stream");


            LCD_Display_PrintLine(
                    1U,
                    "UART Active");


            LCD_Display_PrintLine(
                    2U,
                    "Streaming...");


            LCD_Display_PrintLine(
                    3U,
                    "BACK = Menu");


            break;





        /*
         * ------------------------------------------------------------
         * Settings Screen
         * ------------------------------------------------------------
         */
        case SCREEN_SETTINGS:


            LCD_Display_ShowTitle(
                    "Settings");


            LCD_Display_PrintLine(
                    1U,
                    "Not Implemented");


            LCD_Display_PrintLine(
                    2U,
                    "");


            LCD_Display_PrintLine(
                    3U,
                    "BACK = Menu");


            break;





        /*
         * ------------------------------------------------------------
         * Safety fallback
         * ------------------------------------------------------------
         */
        default:


            current_screen = SCREEN_MENU;


            break;

    }

}





/******************************************************************************
 *
 *                              END OF FILE
 *
 *      screen_manager.c
 *
 *      Version:
 *
 *          v1.1.2 Clean Final
 *
 *
 * Changes:
 *
 *      v1.1.2
 *
 *          - Added real Live Monitor rendering.
 *          - Connected ADC_App_GetVin().
 *          - Connected ADC_App_GetVout().
 *          - Added LCD voltage formatting.
 *          - Reduced LCD refresh flicker.
 *          - Preserved menu architecture.
 *
 *
 * Dependencies:
 *
 *      screen_manager.h
 *      menu_renderer.h
 *      lcd_display.h
 *      menu_edit.h
 *      adc_app.h
 *
 *
 * Compatible:
 *
 *      STM32F103C8T6
 *      STM32 HAL
 *
 ******************************************************************************/
