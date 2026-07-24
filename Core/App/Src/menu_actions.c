#include "menu_actions.h"
#include "menu_edit.h"
#include "screen_manager.h"



void MenuAction_LiveMonitor(void)
{
    ScreenManager_SetScreen(SCREEN_LIVE_MONITOR);
}



void MenuAction_StartStream(void)
{
    ScreenManager_SetScreen(SCREEN_STREAM);
}



void MenuAction_BaudRate(void)
{
    /*
     * Start editing Baud Rate value.
     */
    MenuEdit_Start(EDIT_BAUD_RATE);
}



void MenuAction_SampleRate(void)
{
    /*
     * Start editing Sample Rate value.
     */
    MenuEdit_Start(EDIT_SAMPLE_RATE);
}
