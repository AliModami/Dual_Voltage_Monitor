/******************************************************************************
 *
 * @file    menu_edit.c
 *
 * @brief   Menu Edit Engine
 *
 ******************************************************************************/

#include "menu_edit.h"

/*----------------------------------------------------------
 * Private Variables
 *---------------------------------------------------------*/

static bool g_EditMode = false;

/*----------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------*/

void MenuEdit_Init(void)
{
    g_EditMode = false;
}

void MenuEdit_Enter(void)
{
    g_EditMode = true;
}

void MenuEdit_Exit(void)
{
    g_EditMode = false;
}

bool MenuEdit_IsActive(void)
{
    return g_EditMode;
}/*
 * menu_edit.c
 *
 *  Created on: Jul 24, 2026
 *      Author: A_Modami
 */


