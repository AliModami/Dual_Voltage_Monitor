#ifndef MENU_ACTIONS_H
#define MENU_ACTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "menu_types.h"


void Menu_Action_LiveMonitor(MenuItem_t *item);


void Menu_Action_StartStream(MenuItem_t *item);


void Menu_Action_SystemInfo(MenuItem_t *item);


void Menu_Action_Calibration(MenuItem_t *item);


void Menu_Action_ServiceMode(MenuItem_t *item);



#ifdef __cplusplus
}
#endif


#endif /* MENU_ACTIONS_H */
