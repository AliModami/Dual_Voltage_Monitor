/******************************************************************************
 *
 * File Name :
 *
 *      menu_items.c
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
 * Static Menu Database
 *
 * This module implements the complete static database used by the Menu
 * Framework.
 *
 * Every Menu Page and every Menu Item is statically allocated inside this file.
 *
 * The Menu Engine never creates or destroys menu objects.
 *
 * Instead, it navigates through the objects defined here by using pointers.
 *
 *------------------------------------------------------------------------------
 *
 * Responsibilities
 * =============================================================================
 *
 * This module is responsible for:
 *
 *      • Creating all menu pages
 *      • Creating all menu items
 *      • Building page hierarchy
 *      • Building linked-list relationships
 *      • Attaching callback functions
 *      • Providing the root page
 *
 *------------------------------------------------------------------------------
 *
 * This module is NOT responsible for:
 *
 *      • Navigation logic
 *      • LCD rendering
 *      • Button processing
 *      • Application execution
 *      • Runtime state management
 *
 *------------------------------------------------------------------------------
 *
 * Architecture
 * =============================================================================
 *
 *                 menu_types.h
 *                        │
 *                        ▼
 *                 menu_items.h
 *                        │
 *                        ▼
 *                 menu_items.c
 *                 (Database Layer)
 *                        │
 *                        ▼
 *                 menu_engine.c
 *               (Navigation Layer)
 *                        │
 *                        ▼
 *               menu_renderer.c
 *                 (Display Layer)
 *
 *------------------------------------------------------------------------------
 *
 * Design Philosophy
 * =============================================================================
 *
 * The Menu Database is completely static.
 *
 * Every object is allocated during compilation.
 *
 * No dynamic memory allocation is used anywhere in the framework.
 *
 * This approach provides:
 *
 *      • Predictable memory usage
 *      • Fast execution
 *      • Zero heap fragmentation
 *      • High reliability
 *      • Easy debugging
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

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "menu_items.h"

#include <stddef.h>

#include "menu_actions.h"

/******************************************************************************
 *
 * Private Function Prototypes
 *
 ******************************************************************************/

/*
 * The following functions are internal to the Database Layer.
 *
 * They are intentionally declared as static to prevent access from
 * outside this translation unit.
 */

/**
 * @brief
 *      Create all menu pages.
 *
 * @details
 *      Initializes every MenuPage object and establishes the parent
 *      page relationships used by the navigation engine.
 */
static void Menu_CreatePages(void);


/**
 * @brief
 *      Create all menu items.
 *
 * @details
 *      Initializes every MenuItem object including identifiers,
 *      titles, types, owner pages and child page references.
 *
 *      Linked-list relationships are not created here.
 */
static void Menu_CreateItems(void);


/**
 * @brief
 *      Build all linked-list relationships.
 *
 * @details
 *      Connects every MenuItem using the next and previous pointers.
 *
 *      Also initializes the firstItem, lastItem and selectedItem
 *      members of each MenuPage.
 */
static void Menu_LinkItems(void);


/**
 * @brief
 *      Attach callback functions.
 *
 * @details
 *      Associates executable menu items with their corresponding
 *      application callback functions.
 */
static void Menu_AttachCallbacks(void);


/******************************************************************************
 *
 * Global Menu Page Objects
 *
 * Every page exists for the entire lifetime of the application.
 *
 * These objects form the static hierarchy of the menu database.
 *
 ******************************************************************************/

/*
 * Root page.
 */
MenuPage_t g_mainMenuPage;


/*
 * UART stream configuration page.
 */
MenuPage_t g_streamSettingsPage;


/*
 * Alarm configuration page.
 */
MenuPage_t g_alarmSettingsPage;


/*
 * ADC calibration page.
 */
MenuPage_t g_calibrationPage;


/*
 * Hardware diagnostics and service page.
 */
MenuPage_t g_serviceModePage;


/*
 * Firmware and hardware information page.
 */
MenuPage_t g_systemInfoPage;

/******************************************************************************
 *
 * Global Menu Item Objects
 *
 * Every MenuItem is statically allocated.
 *
 * The Menu Engine navigates through these objects using the linked-list
 * relationships established during database initialization.
 *
 * No MenuItem is ever created or destroyed at runtime.
 *
 ******************************************************************************/

/******************************************************************************
 * Main Menu Items
 ******************************************************************************/

/*
 * Opens the real-time voltage monitoring screen.
 */
MenuItem_t g_liveMonitorItem;


/*
 * Starts UART data streaming.
 */
MenuItem_t g_startStreamItem;


/*
 * Opens the Stream Settings submenu.
 */
MenuItem_t g_streamSettingsItem;


/*
 * Opens the Alarm Settings submenu.
 */
MenuItem_t g_alarmSettingsItem;


/*
 * Opens the Calibration submenu.
 */
MenuItem_t g_calibrationItem;


/*
 * Opens the Service Mode submenu.
 */
MenuItem_t g_serviceModeItem;


/*
 * Opens the System Information page.
 */
MenuItem_t g_systemInfoItem;


/******************************************************************************
 * Stream Settings Page Items
 ******************************************************************************/

/*
 * UART baud rate configuration.
 */
MenuItem_t g_baudRateItem;


/*
 * ADC sampling interval configuration.
 */
MenuItem_t g_sampleRateItem;


/******************************************************************************
 * Alarm Settings Page Items
 ******************************************************************************/

/*
 * Alarm enable/disable option.
 */
MenuItem_t g_alarmEnableItem;


/*
 * Low voltage threshold configuration.
 */
MenuItem_t g_lowVoltageItem;


/*
 * High voltage threshold configuration.
 */
MenuItem_t g_highVoltageItem;


/*
 * Alarm operating mode selection.
 */
MenuItem_t g_alarmModeItem;
/******************************************************************************
 *
 * Calibration Page Items
 *
 ******************************************************************************/

/*
 * Input voltage calibration procedure.
 */
MenuItem_t g_inputCalibrationItem;


/*
 * Output voltage calibration procedure.
 */
MenuItem_t g_outputCalibrationItem;


/*
 * Restore factory calibration values.
 */
MenuItem_t g_factoryCalibrationItem;


/******************************************************************************
 *
 * Service Mode Page Items
 *
 ******************************************************************************/

/*
 * Push-button diagnostic test.
 */
MenuItem_t g_buttonTestItem;


/*
 * Buzzer diagnostic test.
 */
MenuItem_t g_buzzerTestItem;


/*
 * LCD diagnostic test.
 */
MenuItem_t g_lcdTestItem;


/*
 * Restore all configuration parameters to factory defaults.
 */
MenuItem_t g_restoreDefaultItem;


/******************************************************************************
 *
 * Menu_CreatePages()
 *
 ******************************************************************************/

/**
 * @brief
 *      Create and initialize every MenuPage object.
 *
 * @details
 *      This function initializes the complete page hierarchy used by
 *      the Menu Framework.
 *
 *      During this stage:
 *
 *          • Page identifiers are assigned.
 *          • Page titles are assigned.
 *          • Parent page relationships are established.
 *          • Navigation pointers are initialized.
 *          • Runtime pointers remain NULL until the database is linked.
 *
 * @note
 *      Linked-list construction is performed later by
 *      Menu_LinkItems().
 */

static void Menu_CreatePages(void)
{

    /**********************************************************************
     *
     * Main Menu Page
     *
     **********************************************************************/

    g_mainMenuPage.id =
            MENU_PAGE_MAIN;

    g_mainMenuPage.title =
            "Main Menu";

    g_mainMenuPage.firstItem =
            NULL;

    g_mainMenuPage.lastItem =
            NULL;

    g_mainMenuPage.selectedItem =
            NULL;

    /*
     * Root page has no parent.
     */
    g_mainMenuPage.parentPage =
            NULL;

    g_mainMenuPage.itemCount =
            0;



    /**********************************************************************
     *
     * Stream Settings Page
     *
     **********************************************************************/

    g_streamSettingsPage.id =
            MENU_PAGE_STREAM;

    g_streamSettingsPage.title =
            "Stream Settings";

    g_streamSettingsPage.firstItem =
            NULL;

    g_streamSettingsPage.lastItem =
            NULL;

    g_streamSettingsPage.selectedItem =
            NULL;

    g_streamSettingsPage.parentPage =
            &g_mainMenuPage;

    g_streamSettingsPage.itemCount =
            0;
    /**********************************************************************
     *
     * Alarm Settings Page
     *
     **********************************************************************/

    g_alarmSettingsPage.id =
            MENU_PAGE_ALARM;

    g_alarmSettingsPage.title =
            "Alarm Settings";

    g_alarmSettingsPage.firstItem =
            NULL;

    g_alarmSettingsPage.lastItem =
            NULL;

    g_alarmSettingsPage.selectedItem =
            NULL;

    g_alarmSettingsPage.parentPage =
            &g_mainMenuPage;

    g_alarmSettingsPage.itemCount =
            0;



    /**********************************************************************
     *
     * Calibration Page
     *
     **********************************************************************/

    g_calibrationPage.id =
            MENU_PAGE_CALIBRATION;

    g_calibrationPage.title =
            "Calibration";

    g_calibrationPage.firstItem =
            NULL;

    g_calibrationPage.lastItem =
            NULL;

    g_calibrationPage.selectedItem =
            NULL;

    g_calibrationPage.parentPage =
            &g_mainMenuPage;

    g_calibrationPage.itemCount =
            0;



    /**********************************************************************
     *
     * Service Mode Page
     *
     **********************************************************************/

    g_serviceModePage.id =
            MENU_PAGE_SERVICE;

    g_serviceModePage.title =
            "Service Mode";

    g_serviceModePage.firstItem =
            NULL;

    g_serviceModePage.lastItem =
            NULL;

    g_serviceModePage.selectedItem =
            NULL;

    g_serviceModePage.parentPage =
            &g_mainMenuPage;

    g_serviceModePage.itemCount =
            0;



    /**********************************************************************
     *
     * System Information Page
     *
     **********************************************************************/

    g_systemInfoPage.id =
            MENU_PAGE_SYSTEM_INFO;

    g_systemInfoPage.title =
            "System Information";

    g_systemInfoPage.firstItem =
            NULL;

    g_systemInfoPage.lastItem =
            NULL;

    g_systemInfoPage.selectedItem =
            NULL;

    g_systemInfoPage.parentPage =
            &g_mainMenuPage;

    g_systemInfoPage.itemCount =
            0;

}
/******************************************************************************
 *
 * Menu_CreateItems()
 *
 ******************************************************************************/

/**
 * @brief
 *      Create every MenuItem object used by the menu database.
 *
 * @details
 *      This function initializes the static properties of each menu item.
 *
 *      The following members are configured:
 *
 *          • Item identifier
 *          • Display title
 *          • Item type
 *          • Owner page
 *          • Child page relationship
 *          • Navigation pointers (initialized to NULL)
 *          • Callback pointer (initialized to NULL)
 *
 *      Linked-list relationships are intentionally not created here.
 *      They are established later by Menu_LinkItems().
 */

static void Menu_CreateItems(void)
{

    /**********************************************************************
     *
     * Main Menu Items
     *
     **********************************************************************/


    /*
     * Live Monitor
     *
     * Opens the real-time voltage monitoring screen.
     */

    g_liveMonitorItem.id =
            MENU_ITEM_ID_LIVE_MONITOR;

    g_liveMonitorItem.title =
            "Live Monitor";

    g_liveMonitorItem.type =
            MENU_ITEM_ACTION;

    g_liveMonitorItem.ownerPage =
            &g_mainMenuPage;

    g_liveMonitorItem.childPage =
            NULL;

    g_liveMonitorItem.next =
            NULL;

    g_liveMonitorItem.previous =
            NULL;

    g_liveMonitorItem.enterCallback =
            NULL;



    /*
     * Start Stream
     *
     * Starts UART data streaming.
     */

    g_startStreamItem.id =
            MENU_ITEM_ID_START_STREAM;

    g_startStreamItem.title =
            "Start Stream";

    g_startStreamItem.type =
            MENU_ITEM_ACTION;

    g_startStreamItem.ownerPage =
            &g_mainMenuPage;

    g_startStreamItem.childPage =
            NULL;

    g_startStreamItem.next =
            NULL;

    g_startStreamItem.previous =
            NULL;

    g_startStreamItem.enterCallback =
            NULL;
    /*
     * Stream Settings
     *
     * Opens the Stream Settings submenu.
     */

    g_streamSettingsItem.id =
            MENU_ITEM_ID_STREAM_SETTINGS;

    g_streamSettingsItem.title =
            "Stream Settings";

    g_streamSettingsItem.type =
            MENU_ITEM_SUBMENU;

    g_streamSettingsItem.ownerPage =
            &g_mainMenuPage;

    g_streamSettingsItem.childPage =
            &g_streamSettingsPage;

    g_streamSettingsItem.next =
            NULL;

    g_streamSettingsItem.previous =
            NULL;

    g_streamSettingsItem.enterCallback =
            NULL;



    /*
     * Alarm Settings
     *
     * Opens the Alarm Settings submenu.
     */

    g_alarmSettingsItem.id =
            MENU_ITEM_ID_ALARM_SETTINGS;

    g_alarmSettingsItem.title =
            "Alarm Settings";

    g_alarmSettingsItem.type =
            MENU_ITEM_SUBMENU;

    g_alarmSettingsItem.ownerPage =
            &g_mainMenuPage;

    g_alarmSettingsItem.childPage =
            &g_alarmSettingsPage;

    g_alarmSettingsItem.next =
            NULL;

    g_alarmSettingsItem.previous =
            NULL;

    g_alarmSettingsItem.enterCallback =
            NULL;



    /*
     * Calibration
     *
     * Opens the calibration submenu.
     */

    g_calibrationItem.id =
            MENU_ITEM_ID_CALIBRATION;

    g_calibrationItem.title =
            "Calibration";

    g_calibrationItem.type =
            MENU_ITEM_SUBMENU;

    g_calibrationItem.ownerPage =
            &g_mainMenuPage;

    g_calibrationItem.childPage =
            &g_calibrationPage;

    g_calibrationItem.next =
            NULL;

    g_calibrationItem.previous =
            NULL;

    g_calibrationItem.enterCallback =
            NULL;



    /*
     * Service Mode
     *
     * Opens the hardware diagnostic submenu.
     */

    g_serviceModeItem.id =
            MENU_ITEM_ID_SERVICE_MODE;

    g_serviceModeItem.title =
            "Service Mode";

    g_serviceModeItem.type =
            MENU_ITEM_SUBMENU;

    g_serviceModeItem.ownerPage =
            &g_mainMenuPage;

    g_serviceModeItem.childPage =
            &g_serviceModePage;

    g_serviceModeItem.next =
            NULL;

    g_serviceModeItem.previous =
            NULL;

    g_serviceModeItem.enterCallback =
            NULL;
    /*
     * System Information
     *
     * Opens the System Information page.
     */

    g_systemInfoItem.id =
            MENU_ITEM_ID_SYSTEM_INFO;

    g_systemInfoItem.title =
            "System Information";

    g_systemInfoItem.type =
            MENU_ITEM_SUBMENU;

    g_systemInfoItem.ownerPage =
            &g_mainMenuPage;

    g_systemInfoItem.childPage =
            &g_systemInfoPage;

    g_systemInfoItem.next =
            NULL;

    g_systemInfoItem.previous =
            NULL;

    g_systemInfoItem.enterCallback =
            NULL;



    /**********************************************************************
     *
     * Stream Settings Page Items
     *
     **********************************************************************/


    /*
     * Baud Rate
     *
     * Configures the UART communication speed.
     */

    g_baudRateItem.id =
            MENU_ITEM_ID_BAUD_RATE;

    g_baudRateItem.title =
            "Baud Rate";

    g_baudRateItem.type =
            MENU_ITEM_VALUE;

    g_baudRateItem.ownerPage =
            &g_streamSettingsPage;

    g_baudRateItem.childPage =
            NULL;

    g_baudRateItem.next =
            NULL;

    g_baudRateItem.previous =
            NULL;

    g_baudRateItem.enterCallback =
            NULL;



    /*
     * Sample Rate
     *
     * Configures the sampling interval used by the measurement engine.
     */

    g_sampleRateItem.id =
            MENU_ITEM_ID_SAMPLE_RATE;

    g_sampleRateItem.title =
            "Sample Rate";

    g_sampleRateItem.type =
            MENU_ITEM_VALUE;

    g_sampleRateItem.ownerPage =
            &g_streamSettingsPage;

    g_sampleRateItem.childPage =
            NULL;

    g_sampleRateItem.next =
            NULL;

    g_sampleRateItem.previous =
            NULL;

    g_sampleRateItem.enterCallback =
            NULL;
    /**********************************************************************
     *
     * Alarm Settings Page Items
     *
     **********************************************************************/


    /*
     * Alarm Enable
     *
     * Enables or disables the voltage alarm system.
     */

    g_alarmEnableItem.id =
            MENU_ITEM_ID_ALARM_ENABLE;

    g_alarmEnableItem.title =
            "Alarm Enable";

    g_alarmEnableItem.type =
            MENU_ITEM_VALUE;

    g_alarmEnableItem.ownerPage =
            &g_alarmSettingsPage;

    g_alarmEnableItem.childPage =
            NULL;

    g_alarmEnableItem.next =
            NULL;

    g_alarmEnableItem.previous =
            NULL;

    g_alarmEnableItem.enterCallback =
            NULL;



    /*
     * Low Voltage
     *
     * Configures the low voltage alarm threshold.
     */

    g_lowVoltageItem.id =
            MENU_ITEM_ID_LOW_VOLTAGE;

    g_lowVoltageItem.title =
            "Low Voltage";

    g_lowVoltageItem.type =
            MENU_ITEM_VALUE;

    g_lowVoltageItem.ownerPage =
            &g_alarmSettingsPage;

    g_lowVoltageItem.childPage =
            NULL;

    g_lowVoltageItem.next =
            NULL;

    g_lowVoltageItem.previous =
            NULL;

    g_lowVoltageItem.enterCallback =
            NULL;



    /*
     * High Voltage
     *
     * Configures the high voltage alarm threshold.
     */

    g_highVoltageItem.id =
            MENU_ITEM_ID_HIGH_VOLTAGE;

    g_highVoltageItem.title =
            "High Voltage";

    g_highVoltageItem.type =
            MENU_ITEM_VALUE;

    g_highVoltageItem.ownerPage =
            &g_alarmSettingsPage;

    g_highVoltageItem.childPage =
            NULL;

    g_highVoltageItem.next =
            NULL;

    g_highVoltageItem.previous =
            NULL;

    g_highVoltageItem.enterCallback =
            NULL;



    /*
     * Alarm Mode
     *
     * Selects the operating mode of the alarm system.
     *
     * Supported modes:
     *
     *      • Once
     *      • Repeat
     */

    g_alarmModeItem.id =
            MENU_ITEM_ID_ALARM_MODE;

    g_alarmModeItem.title =
            "Alarm Mode";

    g_alarmModeItem.type =
            MENU_ITEM_VALUE;

    g_alarmModeItem.ownerPage =
            &g_alarmSettingsPage;

    g_alarmModeItem.childPage =
            NULL;

    g_alarmModeItem.next =
            NULL;

    g_alarmModeItem.previous =
            NULL;

    g_alarmModeItem.enterCallback =
            NULL;

    /**********************************************************************
     *
     * Calibration Page Items
     *
     **********************************************************************/


    /*
     * Input Calibration
     *
     * Starts the calibration procedure for the input voltage
     * measurement channel.
     */

    g_inputCalibrationItem.id =
            MENU_ITEM_ID_INPUT_CALIBRATION;

    g_inputCalibrationItem.title =
            "Input Calibration";

    g_inputCalibrationItem.type =
            MENU_ITEM_ACTION;

    g_inputCalibrationItem.ownerPage =
            &g_calibrationPage;

    g_inputCalibrationItem.childPage =
            NULL;

    g_inputCalibrationItem.next =
            NULL;

    g_inputCalibrationItem.previous =
            NULL;

    g_inputCalibrationItem.enterCallback =
            NULL;



    /*
     * Output Calibration
     *
     * Starts the calibration procedure for the output voltage
     * measurement channel.
     */

    g_outputCalibrationItem.id =
            MENU_ITEM_ID_OUTPUT_CALIBRATION;

    g_outputCalibrationItem.title =
            "Output Calibration";

    g_outputCalibrationItem.type =
            MENU_ITEM_ACTION;

    g_outputCalibrationItem.ownerPage =
            &g_calibrationPage;

    g_outputCalibrationItem.childPage =
            NULL;

    g_outputCalibrationItem.next =
            NULL;

    g_outputCalibrationItem.previous =
            NULL;

    g_outputCalibrationItem.enterCallback =
            NULL;



    /*
     * Factory Calibration
     *
     * Restores the factory calibration constants.
     */

    g_factoryCalibrationItem.id =
            MENU_ITEM_ID_FACTORY_CALIBRATION;

    g_factoryCalibrationItem.title =
            "Factory Calibration";

    g_factoryCalibrationItem.type =
            MENU_ITEM_ACTION;

    g_factoryCalibrationItem.ownerPage =
            &g_calibrationPage;

    g_factoryCalibrationItem.childPage =
            NULL;

    g_factoryCalibrationItem.next =
            NULL;

    g_factoryCalibrationItem.previous =
            NULL;

    g_factoryCalibrationItem.enterCallback =
            NULL;
    /**********************************************************************
     *
     * Service Mode Page Items
     *
     **********************************************************************/


    /*
     * Button Test
     *
     * Starts the push-button diagnostic routine.
     */

    g_buttonTestItem.id =
            MENU_ITEM_ID_BUTTON_TEST;

    g_buttonTestItem.title =
            "Button Test";

    g_buttonTestItem.type =
            MENU_ITEM_ACTION;

    g_buttonTestItem.ownerPage =
            &g_serviceModePage;

    g_buttonTestItem.childPage =
            NULL;

    g_buttonTestItem.next =
            NULL;

    g_buttonTestItem.previous =
            NULL;

    g_buttonTestItem.enterCallback =
            NULL;



    /*
     * Buzzer Test
     *
     * Starts the buzzer diagnostic routine.
     */

    g_buzzerTestItem.id =
            MENU_ITEM_ID_BUZZER_TEST;

    g_buzzerTestItem.title =
            "Buzzer Test";

    g_buzzerTestItem.type =
            MENU_ITEM_ACTION;

    g_buzzerTestItem.ownerPage =
            &g_serviceModePage;

    g_buzzerTestItem.childPage =
            NULL;

    g_buzzerTestItem.next =
            NULL;

    g_buzzerTestItem.previous =
            NULL;

    g_buzzerTestItem.enterCallback =
            NULL;



    /*
     * LCD Test
     *
     * Starts the LCD diagnostic routine.
     */

    g_lcdTestItem.id =
            MENU_ITEM_ID_LCD_TEST;

    g_lcdTestItem.title =
            "LCD Test";

    g_lcdTestItem.type =
            MENU_ITEM_ACTION;

    g_lcdTestItem.ownerPage =
            &g_serviceModePage;

    g_lcdTestItem.childPage =
            NULL;

    g_lcdTestItem.next =
            NULL;

    g_lcdTestItem.previous =
            NULL;

    g_lcdTestItem.enterCallback =
            NULL;



    /*
     * Restore Default
     *
     * Restores all user configuration parameters to their
     * factory default values.
     */

    g_restoreDefaultItem.id =
            MENU_ITEM_ID_RESTORE_DEFAULT;

    g_restoreDefaultItem.title =
            "Restore Default";

    g_restoreDefaultItem.type =
            MENU_ITEM_ACTION;

    g_restoreDefaultItem.ownerPage =
            &g_serviceModePage;

    g_restoreDefaultItem.childPage =
            NULL;

    g_restoreDefaultItem.next =
            NULL;

    g_restoreDefaultItem.previous =
            NULL;

    g_restoreDefaultItem.enterCallback =
            NULL;

}
/******************************************************************************
 *
 * Menu_LinkItems()
 *
 ******************************************************************************/

/**
 * @brief
 *      Build all linked-list relationships between MenuItem objects.
 *
 * @details
 *      This function constructs the navigation database used by the
 *      Menu Engine.
 *
 *      During this stage:
 *
 *          • next pointers are assigned.
 *          • previous pointers are assigned.
 *          • firstItem is assigned.
 *          • lastItem is assigned.
 *          • itemCount is calculated.
 *
 *      After this function completes, every page owns a complete
 *      doubly-linked list of MenuItem objects.
 *
 * @note
 *      The Menu Engine performs navigation exclusively through these
 *      linked-list pointers.
 */

static void Menu_LinkItems(void)
{

    /**********************************************************************
     *
     * Main Menu
     *
     **********************************************************************/

    g_mainMenuPage.firstItem =
            &g_liveMonitorItem;


    g_liveMonitorItem.next =
            &g_startStreamItem;


    g_startStreamItem.previous =
            &g_liveMonitorItem;


    g_startStreamItem.next =
            &g_streamSettingsItem;


    g_streamSettingsItem.previous =
            &g_startStreamItem;


    g_streamSettingsItem.next =
            &g_alarmSettingsItem;


    g_alarmSettingsItem.previous =
            &g_streamSettingsItem;


    g_alarmSettingsItem.next =
            &g_calibrationItem;


    g_calibrationItem.previous =
            &g_alarmSettingsItem;


    g_calibrationItem.next =
            &g_serviceModeItem;


    g_serviceModeItem.previous =
            &g_calibrationItem;


    g_serviceModeItem.next =
            &g_systemInfoItem;


    g_systemInfoItem.previous =
            &g_serviceModeItem;


    g_systemInfoItem.next =
            NULL;


    g_mainMenuPage.lastItem =
            &g_systemInfoItem;


    g_mainMenuPage.itemCount =
            7;



    /**********************************************************************
     *
     * Stream Settings Page
     *
     **********************************************************************/

    g_streamSettingsPage.firstItem =
            &g_baudRateItem;


    g_baudRateItem.next =
            &g_sampleRateItem;


    g_sampleRateItem.previous =
            &g_baudRateItem;


    g_sampleRateItem.next =
            NULL;


    g_streamSettingsPage.lastItem =
            &g_sampleRateItem;


    g_streamSettingsPage.itemCount =
            2;

    /**********************************************************************
     *
     * Alarm Settings Page
     *
     **********************************************************************/

    g_alarmSettingsPage.firstItem =
            &g_alarmEnableItem;


    g_alarmEnableItem.next =
            &g_lowVoltageItem;


    g_lowVoltageItem.previous =
            &g_alarmEnableItem;


    g_lowVoltageItem.next =
            &g_highVoltageItem;


    g_highVoltageItem.previous =
            &g_lowVoltageItem;


    g_highVoltageItem.next =
            &g_alarmModeItem;


    g_alarmModeItem.previous =
            &g_highVoltageItem;


    g_alarmModeItem.next =
            NULL;


    g_alarmSettingsPage.lastItem =
            &g_alarmModeItem;


    g_alarmSettingsPage.itemCount =
            4;



    /**********************************************************************
     *
     * Calibration Page
     *
     **********************************************************************/

    g_calibrationPage.firstItem =
            &g_inputCalibrationItem;


    g_inputCalibrationItem.next =
            &g_outputCalibrationItem;


    g_outputCalibrationItem.previous =
            &g_inputCalibrationItem;


    g_outputCalibrationItem.next =
            &g_factoryCalibrationItem;


    g_factoryCalibrationItem.previous =
            &g_outputCalibrationItem;


    g_factoryCalibrationItem.next =
            NULL;


    g_calibrationPage.lastItem =
            &g_factoryCalibrationItem;


    g_calibrationPage.itemCount =
            3;



    /**********************************************************************
     *
     * Service Mode Page
     *
     **********************************************************************/

    g_serviceModePage.firstItem =
            &g_buttonTestItem;


    g_buttonTestItem.next =
            &g_buzzerTestItem;


    g_buzzerTestItem.previous =
            &g_buttonTestItem;


    g_buzzerTestItem.next =
            &g_lcdTestItem;


    g_lcdTestItem.previous =
            &g_buzzerTestItem;


    g_lcdTestItem.next =
            &g_restoreDefaultItem;


    g_restoreDefaultItem.previous =
            &g_lcdTestItem;


    g_restoreDefaultItem.next =
            NULL;


    g_serviceModePage.lastItem =
            &g_restoreDefaultItem;


    g_serviceModePage.itemCount =
            4;

    /**********************************************************************
     *
     * Default Selected Items
     *
     **********************************************************************
     *
     * Each page begins with its first menu item selected.
     *
     * The Menu Engine uses these pointers as the initial cursor
     * position whenever a page becomes active.
     *
     **********************************************************************/

    g_mainMenuPage.selectedItem =
            g_mainMenuPage.firstItem;


    g_streamSettingsPage.selectedItem =
            g_streamSettingsPage.firstItem;


    g_alarmSettingsPage.selectedItem =
            g_alarmSettingsPage.firstItem;


    g_calibrationPage.selectedItem =
            g_calibrationPage.firstItem;


    g_serviceModePage.selectedItem =
            g_serviceModePage.firstItem;


    /*
     * The System Information page currently contains no navigable items.
     *
     * Keep the selected item pointer cleared until items are added
     * in a future revision.
     */
    g_systemInfoPage.selectedItem =
            NULL;

}


/******************************************************************************
 *
 * Menu_AttachCallbacks()
 *
 ******************************************************************************/

/**
 * @brief
 *      Attach callback functions to executable menu items.
 *
 * @details
 *      This function connects Action-type menu items with their
 *      corresponding application callback functions.
 *
 *      The Menu Engine simply invokes the callback assigned to the
 *      selected MenuItem.
 *
 *      Callback execution logic is intentionally separated from the
 *      menu database to keep responsibilities independent.
 */

static void Menu_AttachCallbacks(void)
{

    /**********************************************************************
     *
     * Main Menu Actions
     *
     **********************************************************************/

    /*
     * Live Monitor
     */
    g_liveMonitorItem.enterCallback =
            Menu_Action_LiveMonitor;



    /*
     * Start Stream
     */
    g_startStreamItem.enterCallback =
            Menu_Action_StartStream;



    /*
     * System Information
     */
    g_systemInfoItem.enterCallback =
            Menu_Action_SystemInfo;

    /**********************************************************************
     *
     * Calibration Actions
     *
     **********************************************************************/

    /*
     * Input Calibration
     */
    g_inputCalibrationItem.enterCallback =
            Menu_Action_Calibration;


    /*
     * Output Calibration
     */
    g_outputCalibrationItem.enterCallback =
            Menu_Action_Calibration;


    /*
     * Factory Calibration
     */
    g_factoryCalibrationItem.enterCallback =
            Menu_Action_Calibration;



    /**********************************************************************
     *
     * Service Mode Actions
     *
     **********************************************************************/

    /*
     * Button Test
     */
    g_buttonTestItem.enterCallback =
            Menu_Action_ServiceMode;


    /*
     * Buzzer Test
     */
    g_buzzerTestItem.enterCallback =
            Menu_Action_ServiceMode;


    /*
     * LCD Test
     */
    g_lcdTestItem.enterCallback =
            Menu_Action_ServiceMode;


    /*
     * Restore Default
     */
    g_restoreDefaultItem.enterCallback =
            Menu_Action_ServiceMode;

}


/******************************************************************************
 *
 * Menu_ItemsInit()
 *
 ******************************************************************************/

/**
 * @brief
 *      Initialize the complete static menu database.
 *
 * @details
 *      The initialization sequence is intentionally fixed.
 *
 *      Initialization order:
 *
 *          1. Create all pages.
 *          2. Create all menu items.
 *          3. Build linked-list relationships.
 *          4. Attach callback functions.
 *
 *      After successful completion of this function, the menu database
 *      is fully constructed and ready for use by the Menu Engine.
 */

void Menu_ItemsInit(void)
{

    /*
     * Create all menu pages.
     */
    Menu_CreatePages();


    /*
     * Create every menu item.
     */
    Menu_CreateItems();


    /*
     * Build navigation links.
     */
    Menu_LinkItems();


    /*
     * Attach callback functions.
     */
    Menu_AttachCallbacks();

}

/******************************************************************************
 *
 * Menu_GetMainPage()
 *
 ******************************************************************************/

/**
 * @brief
 *      Return the root page of the menu database.
 *
 * @return
 *      Pointer to the Main Menu page.
 *
 * @details
 *      This function provides the standard entry point into the
 *      static menu database.
 *
 *      The Menu Engine calls this function during initialization
 *      and whenever a complete navigation reset is required.
 *
 * @note
 *      Ownership of the returned object remains inside the
 *      menu database. The caller must not modify or free it.
 */

MenuPage_t *Menu_GetMainPage(void)
{

    return &g_mainMenuPage;

}


/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
