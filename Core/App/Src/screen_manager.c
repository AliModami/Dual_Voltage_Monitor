#include "screen_manager.h"
#include "menu_renderer.h"
#include "lcd_display.h"
#include "menu_edit.h"

/*
 * Current active screen.
 */
static ScreenId_t current_screen;



/*
 * Initialize screen manager.
 */
void ScreenManager_Init(void)
{

    current_screen = SCREEN_MENU;

}



/*
 * Change active screen.
 */
void ScreenManager_SetScreen(ScreenId_t screen)
{

    current_screen = screen;

}



/*
 * Render current active screen.
 *
 * This function is called from main loop.
 *
 * Current implementation:
 *
 *      SCREEN_MENU
 *          |
 *          v
 *      Menu Renderer
 *
 *      SCREEN_LIVE_MONITOR
 *          |
 *          v
 *      Live Monitor placeholder
 */
void ScreenManager_Render(void)
{

    switch(current_screen)
    {


//        case SCREEN_MENU:
//
//            /*
//             * Menu renderer controls LCD.
//             */
//            MenuRenderer_Update();
//
//            LCD_Display_RenderMenu();
//
//            break;

    case SCREEN_MENU:


        /*
         * Check parameter edit mode.
         */
        if(MenuEdit_IsActive())
        {

            /*
             * Edit rendering will be added
             * in lcd_display.c next step.
             */
            MenuRenderer_Update();

            LCD_Display_RenderMenu();

        }
        else
        {

            /*
             * Normal menu rendering.
             */
            MenuRenderer_Update();

            LCD_Display_RenderMenu();

        }


        break;




        case SCREEN_LIVE_MONITOR:

            /*
             * Temporary placeholder.
             *
             * Live monitor rendering
             * will be implemented later.
             */

            LCD_Display_Clear();

            LCD_Display_ShowTitle(
                    "Live Monitor"
                    );

            break;



        case SCREEN_STREAM:

            break;



        case SCREEN_SETTINGS:

            break;



        default:

            current_screen = SCREEN_MENU;

            break;

    }

}


/*
 * Get current active screen.
 */
ScreenId_t ScreenManager_GetScreen(void)
{

    return current_screen;

}
