/******************************************************************************
 *
 * @file    menu_items.h
 *
 * @brief   Page Based Menu Data Model
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor - Page Based Menu Prototype
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file defines the menu data structures.
 *
 *      Design rules:
 *
 *      - Menu data is separated from menu control logic.
 *      - No LCD dependency.
 *      - No button dependency.
 *      - No scrolling concept.
 *      - Every menu page contains a maximum of three visible items.
 *
 ******************************************************************************/

#ifndef MENU_ITEMS_H
#define MENU_ITEMS_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>



/*
 * Maximum number of items displayed on one LCD page.
 *
 * LCD 20x4:
 *
 * Line 0 : Page title
 * Line 1-3 : Three menu items
 */
#define MENU_PAGE_VISIBLE_ITEMS     3U



/*
 * Invalid page identifier.
 *
 * Used when an item has no child page.
 */
#define MENU_INVALID_PAGE           0xFFU



/*
 * Menu item type.
 *
 * Determines the behavior after ENTER.
 */
typedef enum
{
    /*
     * Execute application callback.
     */
    MENU_ITEM_ACTION = 0,


    /*
     * Open child menu page.
     */
    MENU_ITEM_SUBMENU


} MenuItemType_t;



/*
 * Menu page identifier.
 *
 * Each page represents one LCD display page.
 */
typedef enum
{

    /*
     * Main menu pages.
     */
    MENU_PAGE_MAIN_0 = 0,
    MENU_PAGE_MAIN_1,
    MENU_PAGE_MAIN_2,


    /*
     * Stream settings.
     */
    MENU_PAGE_STREAM_SETTINGS,


    /*
     * Alarm settings.
     */
    MENU_PAGE_ALARM_SETTINGS_0,
    MENU_PAGE_ALARM_SETTINGS_1,


    /*
     * Calibration.
     */
    MENU_PAGE_CALIBRATION,


    /*
     * Service mode.
     */
    MENU_PAGE_SERVICE_0,
    MENU_PAGE_SERVICE_1,


    /*
     * System information.
     */
    MENU_PAGE_SYSTEM_INFO,


    MENU_PAGE_COUNT


} MenuPageId_t;



/*
 * Menu action callback.
 *
 * Example:
 *
 * static void StartStream_Action(void)
 * {
 *      UART_Start();
 * }
 */
typedef void (*MenuAction_t)(void);



/*
 * Menu item definition.
 *
 * Each item belongs to one Page.
 *
 * Example:
 *
 * Stream Settings
 *
 *      Baud Rate
 *      Sample Rate
 *
 */
typedef struct
{

    /*
     * Text displayed on LCD.
     */
    const char *text;


    /*
     * Item behavior.
     */
    MenuItemType_t type;


    /*
     * Child page.
     *
     * Valid only when type is MENU_ITEM_SUBMENU.
     */
    MenuPageId_t child_page;


    /*
     * Function executed for action items.
     */
    MenuAction_t action;


} MenuItem_t;



/*
 * Menu page definition.
 *
 * A page contains maximum three visible items.
 *
 * Example:
 *
 * MAIN MENU PAGE 0
 *
 * > Live Monitor
 *   Start Stream
 *   Stream Settings
 *
 */
typedef struct
{

    /*
     * Page title.
     */
    const char *title;


    /*
     * Parent page.
     *
     * Used by BACK operation.
     */
    MenuPageId_t parent_page;


    /*
     * Menu items displayed on this page.
     */
    const MenuItem_t *items;


    /*
     * Number of items in this page.
     */
    uint8_t item_count;


    /*
     * Next page.
     *
 * Used for page based navigation.
 *
 * Each page represents one LCD screen.
     *
     * Example:
     *
     * MAIN MENU PAGE 0
     * MAIN MENU PAGE 1
     */
    MenuPageId_t next_page;


    /*
     * Previous page.
     */
    MenuPageId_t previous_page;


} MenuPage_t;



/*
 * Get page information.
 *
 * Used by menu_controller.
 */
const MenuPage_t *MenuItems_GetPage(MenuPageId_t page_id);



#ifdef __cplusplus
}
#endif


#endif /* MENU_ITEMS_H */
