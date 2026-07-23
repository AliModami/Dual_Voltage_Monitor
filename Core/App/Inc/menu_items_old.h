/******************************************************************************
 *
 * @file    menu_items.h
 *
 * @brief   Menu Database Interface For Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * Project :
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU :
 *
 *      STM32F103C8T6
 *
 *------------------------------------------------------------------------------
 *
 * Framework :
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Description :
 *
 *      This file defines the public interface of the menu database layer.
 *
 *      Responsibilities:
 *
 *          - Menu page identifiers
 *          - Menu item identifiers
 *          - Static menu object access
 *          - Menu initialization interface
 *
 *------------------------------------------------------------------------------
 *
 * Architecture :
 *
 *
 *                 +----------------+
 *                 | menu_items.c   |
 *                 | Menu Database  |
 *                 +-------+--------+
 *                         |
 *                         |
 *          +--------------+--------------+
 *          |                             |
 *          v                             v
 *
 *     menu_engine.c              menu_renderer.c
 *
 *
 *------------------------------------------------------------------------------
 *
 * Design Rules :
 *
 *      1.
 *      Menu database owns only static menu structure.
 *
 *
 *      2.
 *      Menu engine owns runtime navigation.
 *
 *
 *      3.
 *      Renderer owns display formatting.
 *
 *
 *      4.
 *      Application data is accessed through callbacks.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Version :
 *
 *      2.0.2
 *
 ******************************************************************************/

#ifndef MENU_ITEMS_H
#define MENU_ITEMS_H



/******************************************************************************
 * Includes
 ******************************************************************************/

#include "menu_types.h"



/******************************************************************************
 * C++ Compatibility
 ******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 * Menu Page Identifiers
 ******************************************************************************/

/**
 * @brief
 *      Unique identifiers for menu pages.
 *
 * @details
 *      Each static MenuPage_t object created in
 *      menu_items.c owns one identifier.
 */

typedef enum
{

    /*
     * Root application menu.
     */
    MENU_PAGE_MAIN = 0U,



    /*
     * Live voltage monitoring page.
     *
     * Displays:
     *
     *      Vin
     *      Vout
     *      Status
     */
    MENU_PAGE_LIVE_MONITOR,



    /*
     * UART streaming configuration page.
     */
    MENU_PAGE_STREAM_SETTINGS,



    /*
     * Alarm configuration page.
     */
    MENU_PAGE_ALARM_SETTINGS,



    /*
     * ADC calibration page.
     */
    MENU_PAGE_CALIBRATION,



    /*
     * Basic service and diagnostic page.
     */
    MENU_PAGE_SERVICE_MODE,



    /*
     * Advanced service functions page.
     *
     * Contains:
     *
     *      Factory Calibration
     *      Restore Default
     */
    MENU_PAGE_SERVICE_ADVANCED,



    /*
     * Firmware information page.
     */
    MENU_PAGE_SYSTEM_INFO,



    /*
     * Number of available pages.
     */
    MENU_PAGE_COUNT


} MenuPageId_t;



/******************************************************************************
 *
 * End Of Part 1/4
 *
 ******************************************************************************/

/******************************************************************************
 * Menu Item Identifiers
 ******************************************************************************/

/**
 * @brief
 *      Unique identifiers for menu items.
 *
 * @details
 *      Used by:
 *
 *          - Menu Engine
 *          - Application callbacks
 *          - Configuration manager
 */

typedef enum
{

    /*
     * Invalid item.
     */
    MENU_ITEM_NONE = 0U,



    /**************************************************************************
     * Main Menu Items
     **************************************************************************/



    /*
     * Opens live monitoring page.
     */
    MENU_ITEM_LIVE_MONITOR,



    /*
     * Starts UART data stream.
     */
    MENU_ITEM_START_STREAM,



    /*
     * Opens stream settings.
     */
    MENU_ITEM_STREAM_SETTINGS,



    /*
     * Opens alarm settings.
     */
    MENU_ITEM_ALARM_SETTINGS,



    /*
     * Opens calibration page.
     */
    MENU_ITEM_CALIBRATION,



    /*
     * Opens service mode.
     */
    MENU_ITEM_SERVICE_MODE,



    /*
     * Opens system information.
     */
    MENU_ITEM_SYSTEM_INFO,





    /**************************************************************************
     * Stream Settings Items
     **************************************************************************/



    /*
     * UART baud rate selection.
     */
    MENU_ITEM_BAUD_RATE,



    /*
     * Stream sample interval.
     */
    MENU_ITEM_SAMPLE_RATE,





    /**************************************************************************
     * Alarm Settings Items
     **************************************************************************/



    /*
     * Enable / disable alarm.
     */
    MENU_ITEM_ALARM_ENABLE,



    /*
     * Low voltage threshold.
     */
    MENU_ITEM_LOW_VOLTAGE_LIMIT,



    /*
     * High voltage threshold.
     */
    MENU_ITEM_HIGH_VOLTAGE_LIMIT,





    /**************************************************************************
     * Calibration Items
     **************************************************************************/



    /*
     * Input voltage calibration.
     */
    MENU_ITEM_INPUT_CALIBRATION,



    /*
     * Output voltage calibration.
     */
    MENU_ITEM_OUTPUT_CALIBRATION,





    /**************************************************************************
     * Service Items
     **************************************************************************/



    /*
     * Button diagnostic test.
     */
    MENU_ITEM_BUTTON_TEST,



    /*
     * Buzzer diagnostic test.
     */
    MENU_ITEM_BUZZER_TEST,



    /*
     * LCD diagnostic test.
     */
    MENU_ITEM_LCD_TEST,



    /*
     * Opens advanced service functions.
     *
     * Page:
     *
     *      MENU_PAGE_SERVICE_ADVANCED
     */
    MENU_ITEM_SERVICE_ADVANCED,



    /*
     * Factory calibration.
     *
     * Moved to:
     *
     *      Service Advanced Page
     */
    MENU_ITEM_FACTORY_CALIBRATION,



    /*
     * Restore default configuration.
     *
     * Moved to:
     *
     *      Service Advanced Page
     */
    MENU_ITEM_RESTORE_DEFAULT,





    /**************************************************************************
     * System Information Items
     **************************************************************************/



    /*
     * Firmware version display.
     */
    MENU_ITEM_FIRMWARE_VERSION,



    /*
     * Build information display.
     */
    MENU_ITEM_BUILD_INFORMATION,



    /*
     * Number of menu items.
     */
    MENU_ITEM_COUNT



} MenuItemId_t;



/******************************************************************************
 *
 * End Of Part 2/4
 *
 ******************************************************************************/

/******************************************************************************
 * Public Initialization Function
 ******************************************************************************/

/**
 * @brief
 *      Initializes complete static menu database.
 *
 * @return
 *      MENU_RESULT_OK when initialization succeeds.
 */
MenuResult_t MenuItems_Init(void);



/******************************************************************************
 * Public Access Functions
 ******************************************************************************/

/**
 * @brief
 *      Returns pointer to requested menu page.
 *
 * @param id
 *      Page identifier.
 *
 * @return
 *      Pointer to MenuPage_t object.
 */
MenuPage_t *MenuItems_GetPage(
        MenuPageId_t id);




/**
 * @brief
 *      Returns pointer to requested menu item.
 *
 * @param id
 *      Item identifier.
 *
 * @return
 *      Pointer to MenuItem_t object.
 */
MenuItem_t *MenuItems_GetItem(
        MenuItemId_t id);




/******************************************************************************
 * Public Menu Root Access
 ******************************************************************************/

/**
 * @brief
 *      Returns the root menu page.
 *
 * @details
 *
 *      The root page is the first page displayed
 *      after system startup.
 *
 * @return
 *      Pointer to Main Menu page.
 */
MenuPage_t *MenuItems_GetRootPage(void);




/******************************************************************************
 * Menu Database Information
 ******************************************************************************/

/**
 * @brief
 *      Returns menu database version string.
 *
 * @return
 *      Constant version text.
 */
const char *MenuItems_GetVersion(void);




/******************************************************************************
 * Menu Database Design Notes
 ******************************************************************************/

/*

The menu database layer stores only static information.


Ownership model:


        +--------------------------------+
        |        Application Data        |
        |                                |
        | ADC / UART / Settings          |
        +----------------+---------------+
                         |
                         |
                         v


        +--------------------------------+
        |        Menu Database           |
        |                                |
        | MenuItem_t                      |
        | MenuPage_t                      |
        +----------------+---------------+
                         |
                         |
                         v


        +--------------------------------+
        |         Menu Engine            |
        |                                |
        | Runtime navigation state       |
        +--------------------------------+



Important rules:


1.
Menu database does not allocate memory dynamically.



2.
Menu database does not communicate with hardware.



3.
Menu items contain references to application data only.



4.
Menu engine controls runtime state.



5.
Renderer controls display output.



6.
Application modules are connected through callbacks.



*/



/******************************************************************************
 *
 * End Of Part 3/4
 *
 ******************************************************************************/

/******************************************************************************
 * Menu Database Usage Example
 ******************************************************************************/

/*

Example:


    MenuPage_t *page;


    page = MenuItems_GetRootPage();


    if(page != NULL)
    {
        MenuEngine_SetPage(page);
    }



*/



/******************************************************************************
 * C Compatibility Closing
 ******************************************************************************/

#ifdef __cplusplus
}
#endif



#endif /* MENU_ITEMS_H */




/******************************************************************************
 *
 * Revision History
 *
 ******************************************************************************

Version 2.0.2

Date:
2026-07-22


Changes:


    - Added advanced service page support.


    - Added:

          MENU_PAGE_SERVICE_ADVANCED


    - Added:

          MENU_ITEM_SERVICE_ADVANCED


    - Separated advanced service functions
      from basic diagnostic page.


    - Preserved compatibility with:

          menu_types.h v2.0.0

          menu_engine.c v2.1.0


    - Preserved static allocation architecture.


    - Preserved callback driven design.


    - No renderer dependency added.


 ******************************************************************************/



/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
