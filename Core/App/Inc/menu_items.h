/******************************************************************************
 *
 * File Name :
 *
 *      menu_items.h
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
 * Description
 * =============================================================================
 *
 * Static Menu Database Interface.
 *
 * This module provides the public interface to the Menu Database.
 *
 * The Menu Database is responsible for creating and maintaining the
 * complete static hierarchy of:
 *
 *      • Menu Pages
 *      • Menu Items
 *      • Navigation relationships
 *      • Callback assignments
 *
 * No navigation logic is implemented here.
 *
 * No LCD rendering is implemented here.
 *
 *------------------------------------------------------------------------------
 *
 * Architecture
 * =============================================================================
 *
 *                   +----------------------+
 *                   |    menu_items.h      |
 *                   +----------------------+
 *
 *                     Database Interface
 *
 *                             │
 *                             ▼
 *
 *                   +----------------------+
 *                   |    menu_items.c      |
 *                   +----------------------+
 *
 *                     Static Database
 *
 *                             │
 *                             ▼
 *
 *                   +----------------------+
 *                   |   menu_engine.c      |
 *                   +----------------------+
 *
 *------------------------------------------------------------------------------
 *
 * Responsibilities
 * =============================================================================
 *
 * This module SHALL:
 *
 *      • Create menu pages
 *      • Create menu items
 *      • Build page hierarchy
 *      • Build linked lists
 *      • Attach callback functions
 *
 * This module SHALL NOT:
 *
 *      • Process buttons
 *      • Navigate menus
 *      • Draw on LCD
 *      • Execute application logic
 *
 *------------------------------------------------------------------------------
 *
 * Dependencies
 * =============================================================================
 *
 *      menu_types.h
 *
 *------------------------------------------------------------------------------
 *
 * Author :
 *
 *      Ali Modami
 *
 *------------------------------------------------------------------------------
 *
 * Version :
 *
 *      2.0.0
 *
 ******************************************************************************/

#ifndef MENU_ITEMS_H
#define MENU_ITEMS_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "menu_types.h"

 /******************************************************************************
  *
  * Global Menu Page Objects
  *
  * These page objects represent the static menu hierarchy.
  *
  * All page instances are allocated inside menu_items.c.
  *
  * The Menu Engine only stores pointers to these objects and never
  * creates or destroys them.
  *
  ******************************************************************************/

 /*
  * Root page.
  */
 extern MenuPage_t g_mainMenuPage;

 /*
  * UART streaming configuration page.
  */
 extern MenuPage_t g_streamSettingsPage;

 /*
  * Alarm configuration page.
  */
 extern MenuPage_t g_alarmSettingsPage;

 /*
  * ADC calibration page.
  */
 extern MenuPage_t g_calibrationPage;

 /*
  * Hardware diagnostics and service page.
  */
 extern MenuPage_t g_serviceModePage;

 /*
  * Firmware and hardware information page.
  */
 extern MenuPage_t g_systemInfoPage;


 /******************************************************************************
  *
  * Global Menu Item Objects
  *
  * Every MenuItem is created once and exists for the entire lifetime
  * of the application.
  *
  * MenuEngine navigates through these objects using the linked-list
  * pointers defined inside each MenuItem.
  *
  ******************************************************************************/

 /******************************************************************************
  *
  * Main Menu Items
  *
  ******************************************************************************/

 /*
  * Opens the real-time voltage monitor.
  */
 extern MenuItem_t g_liveMonitorItem;

 /*
  * Starts UART data streaming.
  */
 extern MenuItem_t g_startStreamItem;

 /*
  * Opens the Stream Settings page.
  */
 extern MenuItem_t g_streamSettingsItem;

 /*
  * Opens the Alarm Settings page.
  */
 extern MenuItem_t g_alarmSettingsItem;

 /*
  * Opens the Calibration page.
  */
 extern MenuItem_t g_calibrationItem;

 /*
  * Opens the Service Mode page.
  */
 extern MenuItem_t g_serviceModeItem;

 /*
  * Opens the System Information page.
  */
 extern MenuItem_t g_systemInfoItem;


 /******************************************************************************
  *
  * Stream Settings Page Items
  *
  ******************************************************************************/

 /*
  * UART communication speed.
  */
 extern MenuItem_t g_baudRateItem;

 /*
  * Sampling interval configuration.
  */
 extern MenuItem_t g_sampleRateItem;


 /******************************************************************************
  *
  * Alarm Settings Page Items
  *
  ******************************************************************************/

 /*
  * Enables or disables the alarm system.
  */
 extern MenuItem_t g_alarmEnableItem;

 /*
  * Low voltage threshold.
  */
 extern MenuItem_t g_lowVoltageItem;

 /*
  * High voltage threshold.
  */
 extern MenuItem_t g_highVoltageItem;

 /*
  * Alarm operating mode.
  */
 extern MenuItem_t g_alarmModeItem;

 /******************************************************************************
  *
  * Calibration Page Items
  *
  ******************************************************************************/

 /*
  * Input voltage calibration procedure.
  */
 extern MenuItem_t g_inputCalibrationItem;

 /*
  * Output voltage calibration procedure.
  */
 extern MenuItem_t g_outputCalibrationItem;

 /*
  * Restores factory calibration values.
  */
 extern MenuItem_t g_factoryCalibrationItem;


 /******************************************************************************
  *
  * Service Mode Page Items
  *
  ******************************************************************************/

 /*
  * Push-button diagnostic test.
  */
 extern MenuItem_t g_buttonTestItem;

 /*
  * Buzzer diagnostic test.
  */
 extern MenuItem_t g_buzzerTestItem;

 /*
  * LCD diagnostic test.
  */
 extern MenuItem_t g_lcdTestItem;

 /*
  * Restores all settings to factory defaults.
  */
 extern MenuItem_t g_restoreDefaultItem;


 /******************************************************************************
  *
  * Database Initialization API
  *
  ******************************************************************************/

 /**
  * @brief
  *      Initialize the complete static menu database.
  *
  * @details
  *      This function creates the entire menu hierarchy and prepares
  *      all menu pages and menu items before the Menu Engine starts.
  *
  *      Initialization sequence:
  *
  *          1. Create menu pages
  *          2. Create menu items
  *          3. Build linked-list relationships
  *          4. Attach callback functions
  *
  * @note
  *      This function shall be called exactly once during
  *      system initialization.
  */
 void Menu_ItemsInit(void);

 /******************************************************************************
  *
  * Public Database Access API
  *
  ******************************************************************************/

 /**
  * @brief
  *      Get the root menu page.
  *
  * @return
  *      Pointer to the Main Menu page.
  *
  * @details
  *      This function provides the standard entry point into the
  *      menu database.
  *
  *      The Menu Engine calls this function during initialization
  *      and whenever a complete navigation reset is required.
  */
 MenuPage_t *Menu_GetMainPage(void);


 /******************************************************************************
  *
  * Page Access Functions
  *
  * These helper functions provide direct access to individual pages.
  *
  * They simplify future extensions and eliminate the need for other
  * modules to access global page objects directly.
  *
  ******************************************************************************/

 /**
  * @brief
  *      Get the Stream Settings page.
  *
  * @return
  *      Pointer to Stream Settings page.
  */
 MenuPage_t *Menu_GetStreamSettingsPage(void);

 /**
  * @brief
  *      Get the Alarm Settings page.
  *
  * @return
  *      Pointer to Alarm Settings page.
  */
 MenuPage_t *Menu_GetAlarmSettingsPage(void);

 /**
  * @brief
  *      Get the Calibration page.
  *
  * @return
  *      Pointer to Calibration page.
  */
 MenuPage_t *Menu_GetCalibrationPage(void);

 /**
  * @brief
  *      Get the Service Mode page.
  *
  * @return
  *      Pointer to Service Mode page.
  */
 MenuPage_t *Menu_GetServiceModePage(void);


 /**
  * @brief
  *      Get the System Information page.
  *
  * @return
  *      Pointer to System Information page.
  */
 MenuPage_t *Menu_GetSystemInfoPage(void);


 /******************************************************************************
  *
  * Design Notes
  *
  ******************************************************************************/

 /*
  * The Menu Database is completely static.
  *
  * All menu objects are allocated at compile time.
  *
  * No dynamic memory allocation is used anywhere in the framework.
  */


 /*
  * Navigation ownership
  * --------------------
  *
  * Database Layer
  *      Owns menu objects.
  *
  * Engine Layer
  *      Owns navigation state.
  *
  * Renderer Layer
  *      Owns display output.
  *
  * Action Layer
  *      Owns application behavior.
  */


 /*
  * This separation allows every module to evolve independently while
  * maintaining a stable public interface.
  */

 /******************************************************************************
  *
  * Module Usage
  *
  ******************************************************************************/

 /*
  * Typical initialization sequence
  * ===============================
  *
  *      Menu_ItemsInit();
  *      MenuEngine_Init();
  *      MenuRenderer_Init();
  *
  * After initialization, the Menu Engine accesses the database only
  * through the public interface declared in this header.
  */


 /*
  * Typical navigation flow
  * =======================
  *
  *      Menu_ItemsInit()
  *              │
  *              ▼
  *
  *      Menu_GetMainPage()
  *              │
  *              ▼
  *
  *      MenuEngine_Init()
  *              │
  *              ▼
  *
  *      MenuEngine_ProcessEvent()
  *              │
  *              ▼
  *
  *      MenuRenderer_Update()
  *
  */


 /******************************************************************************
  *
  * Coding Rules
  *
  ******************************************************************************/

 /*
  * This module intentionally exposes only:
  *
  *      • Database initialization
  *      • Page access functions
  *      • Global menu object declarations
  *
  * Internal helper functions remain private inside menu_items.c.
  *
  * This minimizes coupling between the Database Layer and the
  * remaining layers of the Menu Framework.
  */

 /******************************************************************************
  *
  * Revision History
  *
  ******************************************************************************/

 /*
  * Version 2.0.0
  * ----------------------------------------------------------------------------
  * Initial architecture based on the new layered Menu Framework.
  *
  * Features:
  *
  *      - Static menu database
  *      - Page-oriented navigation
  *      - Doubly linked list navigation
  *      - Callback-based action dispatching
  *      - Separation of Database, Engine, Renderer and Action layers
  *
  * No dynamic memory allocation is used.
  *
  */


 /******************************************************************************
  *
  * C++ Compatibility
  *
  ******************************************************************************/

 #ifdef __cplusplus
 }
 #endif


 /******************************************************************************
  *
  * End of File
  *
  ******************************************************************************/

 #endif /* MENU_ITEMS_H */
