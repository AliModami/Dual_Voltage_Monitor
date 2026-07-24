/******************************************************************************
 *
 * @file    menu_edit.h
 *
 * @brief   Menu Edit Engine Public Interface
 *
 ******************************************************************************/

#ifndef MENU_EDIT_H
#define MENU_EDIT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

/*----------------------------------------------------------
 * Public API
 *---------------------------------------------------------*/

void MenuEdit_Init(void);

void MenuEdit_Enter(void);

void MenuEdit_Exit(void);

bool MenuEdit_IsActive(void);

#ifdef __cplusplus
}
#endif

#endif /* MENU_EDIT_H */
