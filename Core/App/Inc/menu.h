/******************************************************************************
 * @file    menu.h
 * @brief   Menu Controller Public Interface
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *
 *      This module implements the application menu controller.
 *
 *      Responsibilities:
 *
 *          • Menu navigation
 *          • Menu state machine
 *          • Selected item handling
 *          • Scroll position handling
 *          • Enter / Back processing
 *          • Interface between Button and View layers
 *
 *      This module DOES NOT:
 *
 *          • Access LCD hardware
 *          • Access GPIO
 *          • Access UART
 *          • Access ADC
 *          • Draw anything
 *
 *-----------------------------------------------------------------------------
 * Architecture
 *
 *          button_app
 *               │
 *               ▼
 *            menu.c
 *               │
 *      ┌────────┴────────┐
 *      ▼                 ▼
 * menu_data         menu_view
 *
 ******************************************************************************/

#ifndef MENU_H
#define MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "button_app.h"

/******************************************************************************
 *                              Configuration
 ******************************************************************************/

/*
 * LCD geometry
 */

#define MENU_LCD_ROWS                 4U

#define MENU_LCD_COLUMNS             20U

/*
 * First LCD row is reserved for title.
 */

#define MENU_TITLE_ROW                0U

/*
 * Menu items are displayed beginning from row 1.
 */

#define MENU_FIRST_ITEM_ROW           1U

/*
 * Maximum visible items.
 */

#define MENU_VISIBLE_ITEMS            3U

/*
 * Maximum menu title length.
 */

#define MENU_MAX_TITLE_LENGTH        20U

/*
 * Maximum menu item text length.
 */

#define MENU_MAX_ITEM_LENGTH         20U

/*
 * Maximum number of child items inside one menu.
 */

#define MENU_MAX_ITEMS               16U

/******************************************************************************
 *                              Enumerations
 ******************************************************************************/

/**
 * @brief
 *      Application operating mode.
 */
typedef enum
{
    MENU_MODE = 0,

    MONITOR_MODE,

    STREAM_MODE,

    SETTINGS_MODE,

    SYSTEM_INFO_MODE

} Menu_Mode_t;

/**
 * @brief
 *      Menu item type.
 */
typedef enum
{
    MENU_ITEM_ACTION = 0,

    MENU_ITEM_SUBMENU

} Menu_ItemType_t;

/******************************************************************************
 *                              Data Types
 ******************************************************************************/

/**
 * @brief
 *      Forward declaration.
 */
struct Menu;

/**
 * @brief
 *      Menu callback.
 */
typedef void (*MenuAction_t)(void);

/**
 * @brief
 *      One menu item.
 */
typedef struct
{
    const char *text;

    Menu_ItemType_t type;

    MenuAction_t action;

    const struct Menu *submenu;

} MenuItem_t;

/**
 * @brief
 *      One complete menu.
 */
typedef struct Menu
{
    /*
     * Title shown on LCD row 0.
     */
    const char *title;

    /*
     * Pointer to menu items.
     */
    const MenuItem_t *items;

    /*
     * Number of items.
     */
    uint8_t item_count;

    /*
     * Parent menu.
     *
     * NULL for the root menu.
     */
    const struct Menu *parent;

} Menu_t;


/**
 * @brief
 *      Runtime navigation state.
 *
 *      current_menu
 *          Currently active menu.
 *
 *      selected_index
 *          Selected item inside current menu.
 *
 *      top_index
 *          First visible item on LCD.
 */
typedef struct
{
    const Menu_t *current_menu;

    uint8_t selected_index;

    uint8_t top_index;

} MenuState_t;


/******************************************************************************
 *                              Public Functions
 ******************************************************************************/

/**
 * @brief
 *      Initialize menu controller.
 */
void Menu_Init(void);


/**
 * @brief
 *      Periodic menu task.
 */
void Menu_Task(void);


/**
 * @brief
 *      Process one button command.
 *
 * @param command
 *      Button application command.
 */
void Menu_ProcessCommand(Button_AppCommand_t command);


/**
 * @brief
 *      Return current application mode.
 *
 * @return
 *      Current operating mode.
 */
Menu_Mode_t Menu_GetMode(void);


/**
 * @brief
 *      Return read-only menu state.
 *
 * @return
 *      Pointer to current runtime state.
 */
const MenuState_t *Menu_GetState(void);


/**
 * @brief
 *      Force renderer update.
 *
 *      Used after changing screens or
 *      returning from applications.
 */
void Menu_RequestRefresh(void);


/**
 * @brief
 *      Check whether redraw is required.
 *
 * @retval true
 *      Renderer should redraw.
 *
 * @retval false
 *      Nothing changed.
 */
bool Menu_IsRefreshRequired(void);


/**
 * @brief
 *      Clear refresh request flag.
 */
void Menu_ClearRefreshRequest(void);
/******************************************************************************
 *                      External Menu Definitions
 ******************************************************************************/

/*
 * Root menu.
 *
 * Defined in menu_data.c
 */
extern const Menu_t menu_main;


/******************************************************************************
 *                              End of File
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MENU_H */
