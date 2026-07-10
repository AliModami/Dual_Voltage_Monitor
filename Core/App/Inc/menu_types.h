/******************************************************************************
 * @file    menu_types.h
 * @brief   Common Menu Framework Type Definitions
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Description :
 *      This file contains all common data types used by the menu framework.
 *
 *      The purpose of this file is to keep menu-related type definitions
 *      independent from:
 *
 *          - LCD hardware
 *          - Button hardware
 *          - Application services
 *          - Specific menu actions
 *
 *      The menu framework is divided into several independent modules:
 *
 *          menu_types.h
 *                  |
 *                  +---- menu_data.c
 *                  |
 *                  +---- menu_engine.c
 *                  |
 *                  +---- menu_renderer.c
 *                  |
 *                  +---- menu_controller.c
 *
 *      By keeping common structures here, each module communicates through
 *      stable interfaces instead of accessing internal data directly.
 *
 *-----------------------------------------------------------------------------
 * Design Goals :
 *
 *      1. Keep menu framework reusable for future projects.
 *
 *      2. Avoid hardware dependency inside menu logic.
 *
 *      3. Allow unlimited menu depth using tree architecture.
 *
 *      4. Separate menu data from menu processing.
 *
 *      5. Support different renderers in the future.
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
 *          Initial architecture version.
 *
 ******************************************************************************/

#ifndef MENU_TYPES_H
#define MENU_TYPES_H



/******************************************************************************
 *                              Include Files
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>



/******************************************************************************
 *                              Forward Declaration
 ******************************************************************************/

/*
 * Forward declaration of MenuItem structure.
 *
 * The structure contains a pointer to itself.
 * Therefore, we must declare the type before
 * using internal pointers.
 */

typedef struct MenuItem MenuItem_t;



/******************************************************************************
 *                              Menu Modes
 ******************************************************************************/

/**
 * @brief
 *      Application operating modes.
 *
 * @details
 *      The menu framework itself only controls navigation.
 *
 *      Actual application screens use these modes
 *      to decide what should be displayed.
 *
 */
typedef enum
{
    MENU_MODE = 0,

    MONITOR_MODE,

    STREAM_MODE,

    SETTINGS_MODE,

    SYSTEM_INFO_MODE

} Menu_Mode_t;



/******************************************************************************
 *                              Menu Action Type
 ******************************************************************************/

/**
 * @brief
 *      Function pointer type for menu actions.
 *
 * @details
 *
 *      Each menu item can optionally execute
 *      a function when ENTER is pressed.
 *
 *      Example:
 *
 *          Live Monitor
 *                  |
 *                  +----> Screen_OpenMonitor()
 *
 */
typedef void (*MenuAction_t)(void);



/******************************************************************************
 *                              Menu Item Structure
 ******************************************************************************/

/**
 * @brief
 *      Single menu node definition.
 *
 * @details
 *
 *      The menu system uses a tree structure.
 *
 *
 *                    Main Menu
 *                        |
 *          +-------------+-------------+
 *          |             |             |
 *     Monitor       Settings      System Info
 *
 *
 *      Each item knows:
 *
 *          - its parent
 *          - its children
 *          - neighbour items
 *          - optional action
 *
 *      This allows:
 *
 *          - UP/DOWN navigation
 *          - ENTER execution
 *          - BACK navigation
 *
 */
struct MenuItem
{

    /**
     * Display text shown on LCD.
     *
     * Example:
     *
     *      "Live Monitor"
     */
    const char *name;



    /**
     * Function executed after ENTER.
     *
     * NULL means:
     *
     *      This item is only a container.
     */
    MenuAction_t action;



    /**
     * Parent menu pointer.
     *
     * Used when BACK button is pressed.
     */
    const MenuItem_t *parent;



    /**
     * First child menu item.
     *
     * NULL means:
     *
     *      This item has no submenu.
     */
    const MenuItem_t *child;



    /**
     * Next item in the same menu level.
     *
     * Used for DOWN navigation.
     */
    const MenuItem_t *next;



    /**
     * Previous item in the same menu level.
     *
     * Used for UP navigation.
     */
    const MenuItem_t *previous;



    /**
     * Unique identifier of menu item.
     *
     * This prevents comparing menu items
     * by text strings.
     */
    uint16_t id;

};



#endif /* MENU_TYPES_H */
