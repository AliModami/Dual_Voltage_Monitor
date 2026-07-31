#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


/*
 * Application screen identifiers.
 *
 * Each screen represents a complete application view.
 */
typedef enum
{
    SCREEN_MENU = 0,

    SCREEN_LIVE_MONITOR,

    SCREEN_STREAM,

    SCREEN_SETTINGS,

    /*
     * Service Mode
     *
     * LCD Diagnostic Screen
     */
    SCREEN_LCD_TEST

} ScreenId_t;



/*
 * Initialize screen manager.
 */
void ScreenManager_Init(void);



/*
 * Change active screen.
 */
void ScreenManager_SetScreen(ScreenId_t screen);



/*
 * Get current active screen.
 */
ScreenId_t ScreenManager_GetScreen(void);



/*
 * Render current screen.
 *
 * Called periodically from main loop.
 */
void ScreenManager_Render(void);


#ifdef __cplusplus
}
#endif


#endif /* SCREEN_MANAGER_H */
