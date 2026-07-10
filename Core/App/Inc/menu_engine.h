/******************************************************************************
 * @file    menu_engine.h
 * @brief   Menu Navigation Engine Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This file defines the public interface of the menu engine layer.
 *
 *
 *      The menu engine is responsible for navigation logic only.
 *
 *
 *      Responsibilities:
 *
 *          - Store current menu state.
 *          - Move selection up and down.
 *          - Enter submenu.
 *          - Return to parent menu.
 *          - Execute selected menu action.
 *
 *
 *      This module DOES NOT:
 *
 *          - Read buttons directly.
 *          - Control LCD.
 *          - Draw menu graphics.
 *          - Access hardware peripherals.
 *
 *
 *-----------------------------------------------------------------------------
 * Architecture:
 *
 *
 *          Button Layer
 *
 *                |
 *                v
 *
 *          Menu Controller
 *
 *                |
 *                v
 *
 *          menu_engine.c
 *
 *                |
 *       +--------+--------+
 *       |                 |
 *       v                 v
 *
 *   menu_data.c     menu_renderer.c
 *
 *
 *-----------------------------------------------------------------------------
 * Design Goals:
 *
 *      1. Navigation logic must be independent from display.
 *
 *      2. Menu structure must be configurable without
 *         changing navigation algorithms.
 *
 *      3. Future input devices can reuse this engine:
 *
 *              - Push buttons
 *              - Rotary encoder
 *              - Touch screen
 *
 *
 *-----------------------------------------------------------------------------
 * Author :
 *      Ali Modami
 *
 * Version :
 *      1.0.0
 *
 * Change History :
 *
 *      1.0.0
 *          Initial Plan A architecture version.
 *
 ******************************************************************************/



#ifndef MENU_ENGINE_H
#define MENU_ENGINE_H



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>


#include "menu.h"

#include "menu_data.h"

#include "button_app.h"





/******************************************************************************
 *                         Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu engine.
 *
 * @details
 *
 *      Loads root menu and clears
 *      navigation indexes.
 *
 */
void MenuEngine_Init(void);





/******************************************************************************
 *                         Navigation Functions
 ******************************************************************************/

/**
 * @brief
 *      Move selection to next item.
 *
 */
void MenuEngine_MoveNext(void);





/**
 * @brief
 *      Move selection to previous item.
 *
 */
void MenuEngine_MovePrevious(void);





/**
 * @brief
 *      Enter selected menu item.
 *
 * @details
 *
 *      If selected item contains submenu,
 *      engine enters that submenu.
 *
 *      Otherwise associated action
 *      will be executed.
 *
 */
void MenuEngine_Enter(void);





/**
 * @brief
 *      Return to parent menu.
 *
 */
void MenuEngine_Back(void);





/******************************************************************************
 *                         Command Processing
 ******************************************************************************/

/**
 * @brief
 *      Process application button command.
 *
 * @param command
 *
 *      Logical button command.
 *
 */
void MenuEngine_ProcessCommand(
        Button_AppCommand_t command);





/******************************************************************************
 *                         State Access
 ******************************************************************************/

/**
 * @brief
 *      Get current menu.
 *
 * @return
 *
 *      Pointer to active menu.
 *
 */
const Menu_t *MenuEngine_GetCurrentMenu(void);





/**
 * @brief
 *      Get selected item index.
 *
 * @return
 *
 *      Current selection index.
 *
 */
uint8_t MenuEngine_GetSelectedIndex(void);





/**
 * @brief
 *      Get selected menu item.
 *
 * @return
 *
 *      Pointer to selected item.
 *
 */
const MenuItem_t *MenuEngine_GetSelectedItem(void);





/**
 * @brief
 *      Get top visible item index.
 *
 * @return
 *
 *      First item displayed on LCD.
 *
 */
uint8_t MenuEngine_GetTopIndex(void);





/**
 * @brief
 *      Check if menu changed.
 *
 * @return
 *
 *      true:
 *          Renderer must refresh.
 *
 *      false:
 *          No update required.
 *
 */
bool MenuEngine_IsChanged(void);





/**
 * @brief
 *      Clear change flag.
 *
 */
void MenuEngine_ClearChangedFlag(void);





#endif /* MENU_ENGINE_H */
