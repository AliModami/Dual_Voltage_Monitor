/******************************************************************************
 *
 * @file    menu_items.c
 *
 * @brief   Static Menu Database Implementation
 *          For Dual Voltage Monitor
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
 *      This module contains the static menu database.
 *
 *      Responsibilities:
 *
 *          - Create menu pages.
 *          - Create menu items.
 *          - Build menu hierarchy.
 *          - Provide menu object access.
 *
 *      This module does not handle:
 *
 *          - LCD rendering.
 *          - Button processing.
 *          - Navigation algorithm.
 *          - Application data processing.
 *
 *------------------------------------------------------------------------------
 *
 * Version :
 *
 *      2.0.4
 *
 *
 * Changes:
 *
 *      - Restored missing initialization functions.
 *
 *      - Restored complete MenuItems_GetItem()
 *        lookup table.
 *
 *      - Fixed missing item access paths.
 *
 ******************************************************************************/

#include "menu_items.h"



/******************************************************************************
 * Private Constants
 ******************************************************************************/

#define MENU_ITEMS_VERSION_STRING    "2.0.4"



/******************************************************************************
 * Static Menu Pages
 ******************************************************************************/

static MenuPage_t g_mainMenuPage;

static MenuPage_t g_liveMonitorPage;

static MenuPage_t g_streamSettingsPage;

static MenuPage_t g_alarmSettingsPage;

static MenuPage_t g_calibrationPage;

static MenuPage_t g_serviceModePage;

static MenuPage_t g_serviceAdvancedPage;

static MenuPage_t g_systemInfoPage;



/******************************************************************************
 * Static Menu Items
 ******************************************************************************/

/*
 * Main menu items
 */

static MenuItem_t g_itemLiveMonitor;

static MenuItem_t g_itemStartStream;

static MenuItem_t g_itemStreamSettings;

static MenuItem_t g_itemAlarmSettings;

static MenuItem_t g_itemCalibration;

static MenuItem_t g_itemServiceMode;

static MenuItem_t g_itemSystemInfo;



/*
 * Stream settings items
 */

static MenuItem_t g_itemBaudRate;

static MenuItem_t g_itemSampleRate;



/*
 * Alarm settings items
 */

static MenuItem_t g_itemAlarmEnable;

static MenuItem_t g_itemLowVoltageLimit;

static MenuItem_t g_itemHighVoltageLimit;



/*
 * Calibration items
 */

static MenuItem_t g_itemInputCalibration;

static MenuItem_t g_itemOutputCalibration;



/*
 * Service mode items
 */

static MenuItem_t g_itemButtonTest;

static MenuItem_t g_itemBuzzerTest;

static MenuItem_t g_itemLcdTest;

static MenuItem_t g_itemServiceAdvanced;



/*
 * Advanced service items
 */

static MenuItem_t g_itemFactoryCalibration;

static MenuItem_t g_itemRestoreDefault;



/*
 * System information items
 */

static MenuItem_t g_itemFirmwareVersion;

static MenuItem_t g_itemBuildInformation;



/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void MenuItems_InitPages(void);

static void MenuItems_InitItems(void);

static void MenuItems_InitMainItems(void);

static void MenuItems_InitStreamItems(void);

static void MenuItems_InitAlarmItems(void);

static void MenuItems_InitCalibrationItems(void);

static void MenuItems_InitServiceItems(void);

static void MenuItems_InitServiceAdvancedItems(void);

static void MenuItems_InitSystemInfoItems(void);



static void MenuItems_BuildMainTree(void);

static void MenuItems_BuildStreamTree(void);

static void MenuItems_BuildAlarmTree(void);

static void MenuItems_BuildCalibrationTree(void);

static void MenuItems_BuildServiceTree(void);

static void MenuItems_BuildServiceAdvancedTree(void);

static void MenuItems_BuildSystemInfoTree(void);



static void MenuItems_ClearItem(
        MenuItem_t *item);


static void MenuItems_ClearPage(
        MenuPage_t *page);


static void MenuItems_LinkItems(
        MenuItem_t *first,
        MenuItem_t *second);


static void MenuItems_AssignPageItems(
        MenuPage_t *page,
        MenuItem_t *firstItem,
        uint16_t count);



/******************************************************************************
 *
 * End Of Part 1/6
 *
 ******************************************************************************/
/******************************************************************************
 * Page Helper Functions
 ******************************************************************************/

static void MenuItems_ClearPage(
        MenuPage_t *page)
{

    if(page == NULL)
    {
        return;
    }


    page->id =
            0U;


    page->title =
            NULL;


    page->type =
            MENU_PAGE_STANDARD;


    page->firstItem =
            NULL;


    page->lastItem =
            NULL;


    page->selectedItem =
            NULL;


    page->itemCount =
            0U;


    page->selectedIndex =
            0U;


    page->visibleRows =
            MENU_DEFAULT_VISIBLE_ROWS;


    page->parentPage =
            NULL;


    page->previousPage =
            NULL;


    page->onEnter =
            NULL;


    page->onExit =
            NULL;


    page->redrawRequired =
            true;


    page->userData =
            NULL;

}



/******************************************************************************
 * Page Initialization
 ******************************************************************************/

static void MenuItems_InitPages(void)
{

    MenuItems_ClearPage(
            &g_mainMenuPage);


    MenuItems_ClearPage(
            &g_liveMonitorPage);


    MenuItems_ClearPage(
            &g_streamSettingsPage);


    MenuItems_ClearPage(
            &g_alarmSettingsPage);


    MenuItems_ClearPage(
            &g_calibrationPage);


    MenuItems_ClearPage(
            &g_serviceModePage);


    MenuItems_ClearPage(
            &g_serviceAdvancedPage);


    MenuItems_ClearPage(
            &g_systemInfoPage);



    /*
     * Main Menu
     */

    g_mainMenuPage.id =
            MENU_PAGE_MAIN;


    g_mainMenuPage.title =
            "Main Menu";


    g_mainMenuPage.type =
            MENU_PAGE_STANDARD;



    /*
     * Live Monitor
     */

    g_liveMonitorPage.id =
            MENU_PAGE_LIVE_MONITOR;


    g_liveMonitorPage.title =
            "Live Monitor";


    g_liveMonitorPage.type =
            MENU_PAGE_MONITOR;


    g_liveMonitorPage.parentPage =
            &g_mainMenuPage;



    /*
     * Stream Settings
     */

    g_streamSettingsPage.id =
            MENU_PAGE_STREAM_SETTINGS;


    g_streamSettingsPage.title =
            "Stream Settings";


    g_streamSettingsPage.type =
            MENU_PAGE_SETTINGS;


    g_streamSettingsPage.parentPage =
            &g_mainMenuPage;



    /*
     * Alarm Settings
     */

    g_alarmSettingsPage.id =
            MENU_PAGE_ALARM_SETTINGS;


    g_alarmSettingsPage.title =
            "Alarm Settings";


    g_alarmSettingsPage.type =
            MENU_PAGE_SETTINGS;


    g_alarmSettingsPage.parentPage =
            &g_mainMenuPage;



    /*
     * Calibration
     */

    g_calibrationPage.id =
            MENU_PAGE_CALIBRATION;


    g_calibrationPage.title =
            "Calibration";


    g_calibrationPage.type =
            MENU_PAGE_SETTINGS;


    g_calibrationPage.parentPage =
            &g_mainMenuPage;



    /*
     * Service Mode
     */

    g_serviceModePage.id =
            MENU_PAGE_SERVICE_MODE;


    g_serviceModePage.title =
            "Service Mode";


    g_serviceModePage.type =
            MENU_PAGE_SERVICE;


    g_serviceModePage.parentPage =
            &g_mainMenuPage;



    /*
     * Advanced Service
     */

    g_serviceAdvancedPage.id =
            MENU_PAGE_SERVICE_ADVANCED;


    g_serviceAdvancedPage.title =
            "Advanced Service";


    g_serviceAdvancedPage.type =
            MENU_PAGE_SERVICE;


    g_serviceAdvancedPage.parentPage =
            &g_serviceModePage;



    /*
     * System Information
     */

    g_systemInfoPage.id =
            MENU_PAGE_SYSTEM_INFO;


    g_systemInfoPage.title =
            "System Info";


    g_systemInfoPage.type =
            MENU_PAGE_INFORMATION;


    g_systemInfoPage.parentPage =
            &g_mainMenuPage;

}



/******************************************************************************
 *
 * End Of Part 2/6
 *
 ******************************************************************************/
/******************************************************************************
 * Menu Item Helper Functions
 ******************************************************************************/

static void MenuItems_ClearItem(
        MenuItem_t *item)
{

    if(item == NULL)
    {
        return;
    }


    item->id =
            MENU_ITEM_NONE;


    item->title =
            NULL;


    item->description =
            NULL;


    item->type =
            MENU_ITEM_NORMAL;


    item->visibility =
            MENU_VISIBLE;



    /*
     * Tree navigation
     */

    item->parent =
            NULL;


    item->child =
            NULL;


    item->next =
            NULL;


    item->previous =
            NULL;


    item->childPage =
            NULL;



    /*
     * Value handling
     */

    item->data =
            NULL;


    item->dataType =
            MENU_EDIT_TYPE_NONE;


    item->getter =
            NULL;


    item->setter =
            NULL;


    item->validator =
            NULL;



    /*
     * Callbacks
     */

    item->onEnter =
            NULL;


    item->onSelect =
            NULL;


    item->onExit =
            NULL;


    /*
     * Compatibility callback.
     */

    item->enterCallback =
            NULL;



    /*
     * User extension
     */

    item->userData =
            NULL;

}



/******************************************************************************
 * Main Menu Item Initialization
 ******************************************************************************/

static void MenuItems_InitMainItems(void)
{

    /*
     * Live Monitor
     */

    MenuItems_ClearItem(
            &g_itemLiveMonitor);


    g_itemLiveMonitor.id =
            MENU_ITEM_LIVE_MONITOR;


    g_itemLiveMonitor.title =
            "Live Monitor";


    g_itemLiveMonitor.description =
            "Display voltage status";


    g_itemLiveMonitor.type =
            MENU_ITEM_SUBMENU;


    g_itemLiveMonitor.childPage =
            &g_liveMonitorPage;



    /*
     * Start Stream
     */

    MenuItems_ClearItem(
            &g_itemStartStream);


    g_itemStartStream.id =
            MENU_ITEM_START_STREAM;


    g_itemStartStream.title =
            "Start Stream";


    g_itemStartStream.description =
            "UART data streaming";


    g_itemStartStream.type =
            MENU_ITEM_ACTION;



    /*
     * Stream Settings
     */

    MenuItems_ClearItem(
            &g_itemStreamSettings);


    g_itemStreamSettings.id =
            MENU_ITEM_STREAM_SETTINGS;


    g_itemStreamSettings.title =
            "Stream Settings";


    g_itemStreamSettings.description =
            "UART configuration";


    g_itemStreamSettings.type =
            MENU_ITEM_SUBMENU;


    g_itemStreamSettings.childPage =
            &g_streamSettingsPage;



    /*
     * Alarm Settings
     */

    MenuItems_ClearItem(
            &g_itemAlarmSettings);


    g_itemAlarmSettings.id =
            MENU_ITEM_ALARM_SETTINGS;


    g_itemAlarmSettings.title =
            "Alarm Settings";


    g_itemAlarmSettings.description =
            "Alarm configuration";


    g_itemAlarmSettings.type =
            MENU_ITEM_SUBMENU;


    g_itemAlarmSettings.childPage =
            &g_alarmSettingsPage;



    /*
     * Calibration
     */

    MenuItems_ClearItem(
            &g_itemCalibration);


    g_itemCalibration.id =
            MENU_ITEM_CALIBRATION;


    g_itemCalibration.title =
            "Calibration";


    g_itemCalibration.description =
            "Voltage calibration";


    g_itemCalibration.type =
            MENU_ITEM_SUBMENU;


    g_itemCalibration.childPage =
            &g_calibrationPage;



    /*
     * Service Mode
     */

    MenuItems_ClearItem(
            &g_itemServiceMode);


    g_itemServiceMode.id =
            MENU_ITEM_SERVICE_MODE;


    g_itemServiceMode.title =
            "Service Mode";


    g_itemServiceMode.description =
            "Diagnostics";


    g_itemServiceMode.type =
            MENU_ITEM_SUBMENU;


    g_itemServiceMode.childPage =
            &g_serviceModePage;



    /*
     * System Information
     */

    MenuItems_ClearItem(
            &g_itemSystemInfo);


    g_itemSystemInfo.id =
            MENU_ITEM_SYSTEM_INFO;


    g_itemSystemInfo.title =
            "System Info";


    g_itemSystemInfo.description =
            "System information";


    g_itemSystemInfo.type =
            MENU_ITEM_SUBMENU;


    g_itemSystemInfo.childPage =
            &g_systemInfoPage;

}



/******************************************************************************
 * Stream Settings Item Initialization
 ******************************************************************************/

static void MenuItems_InitStreamItems(void)
{

    /*
     * Baud Rate
     */

    MenuItems_ClearItem(
            &g_itemBaudRate);


    g_itemBaudRate.id =
            MENU_ITEM_BAUD_RATE;


    g_itemBaudRate.title =
            "Baud Rate";


    g_itemBaudRate.description =
            "UART speed";


    g_itemBaudRate.type =
            MENU_ITEM_EDIT;


    g_itemBaudRate.dataType =
            MENU_EDIT_TYPE_NONE;



    /*
     * Sample Rate
     */

    MenuItems_ClearItem(
            &g_itemSampleRate);


    g_itemSampleRate.id =
            MENU_ITEM_SAMPLE_RATE;


    g_itemSampleRate.title =
            "Sample Rate";


    g_itemSampleRate.description =
            "Sampling interval";


    g_itemSampleRate.type =
            MENU_ITEM_EDIT;


    g_itemSampleRate.dataType =
            MENU_EDIT_TYPE_NONE;

}



/******************************************************************************
 *
 * End Of Part 3/6
 *
 ******************************************************************************/

/******************************************************************************
 * Alarm Settings Item Initialization
 ******************************************************************************/

static void MenuItems_InitAlarmItems(void)
{

    /*
     * Alarm Enable
     */

    MenuItems_ClearItem(
            &g_itemAlarmEnable);


    g_itemAlarmEnable.id =
            MENU_ITEM_ALARM_ENABLE;


    g_itemAlarmEnable.title =
            "Alarm Enable";


    g_itemAlarmEnable.description =
            "Enable alarm system";


    g_itemAlarmEnable.type =
            MENU_ITEM_EDIT;


    g_itemAlarmEnable.dataType =
            MENU_EDIT_TYPE_NONE;



    /*
     * Low Voltage Limit
     */

    MenuItems_ClearItem(
            &g_itemLowVoltageLimit);


    g_itemLowVoltageLimit.id =
            MENU_ITEM_LOW_VOLTAGE_LIMIT;


    g_itemLowVoltageLimit.title =
            "Low Voltage";


    g_itemLowVoltageLimit.description =
            "Low voltage threshold";


    g_itemLowVoltageLimit.type =
            MENU_ITEM_EDIT;


    g_itemLowVoltageLimit.dataType =
            MENU_EDIT_TYPE_NONE;



    /*
     * High Voltage Limit
     */

    MenuItems_ClearItem(
            &g_itemHighVoltageLimit);


    g_itemHighVoltageLimit.id =
            MENU_ITEM_HIGH_VOLTAGE_LIMIT;


    g_itemHighVoltageLimit.title =
            "High Voltage";


    g_itemHighVoltageLimit.description =
            "High voltage threshold";


    g_itemHighVoltageLimit.type =
            MENU_ITEM_EDIT;


    g_itemHighVoltageLimit.dataType =
            MENU_EDIT_TYPE_NONE;

}



/******************************************************************************
 * Calibration Item Initialization
 ******************************************************************************/

static void MenuItems_InitCalibrationItems(void)
{

    /*
     * Input Calibration
     */

    MenuItems_ClearItem(
            &g_itemInputCalibration);


    g_itemInputCalibration.id =
            MENU_ITEM_INPUT_CALIBRATION;


    g_itemInputCalibration.title =
            "Input Calibration";


    g_itemInputCalibration.description =
            "Calibrate input voltage";


    g_itemInputCalibration.type =
            MENU_ITEM_ACTION;



    /*
     * Output Calibration
     */

    MenuItems_ClearItem(
            &g_itemOutputCalibration);


    g_itemOutputCalibration.id =
            MENU_ITEM_OUTPUT_CALIBRATION;


    g_itemOutputCalibration.title =
            "Output Calibration";


    g_itemOutputCalibration.description =
            "Calibrate output voltage";


    g_itemOutputCalibration.type =
            MENU_ITEM_ACTION;

}



/******************************************************************************
 * Service Mode Item Initialization
 ******************************************************************************/

static void MenuItems_InitServiceItems(void)
{

    /*
     * Button Test
     */

    MenuItems_ClearItem(
            &g_itemButtonTest);


    g_itemButtonTest.id =
            MENU_ITEM_BUTTON_TEST;


    g_itemButtonTest.title =
            "Button Test";


    g_itemButtonTest.description =
            "Test input buttons";


    g_itemButtonTest.type =
            MENU_ITEM_ACTION;



    /*
     * Buzzer Test
     */

    MenuItems_ClearItem(
            &g_itemBuzzerTest);


    g_itemBuzzerTest.id =
            MENU_ITEM_BUZZER_TEST;


    g_itemBuzzerTest.title =
            "Buzzer Test";


    g_itemBuzzerTest.description =
            "Test alarm buzzer";


    g_itemBuzzerTest.type =
            MENU_ITEM_ACTION;



    /*
     * LCD Test
     */

    MenuItems_ClearItem(
            &g_itemLcdTest);


    g_itemLcdTest.id =
            MENU_ITEM_LCD_TEST;


    g_itemLcdTest.title =
            "LCD Test";


    g_itemLcdTest.description =
            "Test display module";


    g_itemLcdTest.type =
            MENU_ITEM_ACTION;



    /*
     * Advanced Service
     */

    MenuItems_ClearItem(
            &g_itemServiceAdvanced);


    g_itemServiceAdvanced.id =
            MENU_ITEM_SERVICE_ADVANCED;


    g_itemServiceAdvanced.title =
            "Advanced Service";


    g_itemServiceAdvanced.description =
            "Advanced functions";


    g_itemServiceAdvanced.type =
            MENU_ITEM_SUBMENU;


    g_itemServiceAdvanced.childPage =
            &g_serviceAdvancedPage;

}



/******************************************************************************
 *
 * End Of Part 4/6
 *
 ******************************************************************************/
/******************************************************************************
 * Advanced Service Item Initialization
 ******************************************************************************/

static void MenuItems_InitServiceAdvancedItems(void)
{

    /*
     * Factory Calibration
     */

    MenuItems_ClearItem(
            &g_itemFactoryCalibration);


    g_itemFactoryCalibration.id =
            MENU_ITEM_FACTORY_CALIBRATION;


    g_itemFactoryCalibration.title =
            "Factory Calibration";


    g_itemFactoryCalibration.description =
            "Factory adjustment";


    g_itemFactoryCalibration.type =
            MENU_ITEM_ACTION;



    /*
     * Restore Default
     */

    MenuItems_ClearItem(
            &g_itemRestoreDefault);


    g_itemRestoreDefault.id =
            MENU_ITEM_RESTORE_DEFAULT;


    g_itemRestoreDefault.title =
            "Restore Default";


    g_itemRestoreDefault.description =
            "Reset configuration";


    g_itemRestoreDefault.type =
            MENU_ITEM_ACTION;

}



/******************************************************************************
 * System Information Item Initialization
 ******************************************************************************/

static void MenuItems_InitSystemInfoItems(void)
{

    /*
     * Firmware Version
     */

    MenuItems_ClearItem(
            &g_itemFirmwareVersion);


    g_itemFirmwareVersion.id =
            MENU_ITEM_FIRMWARE_VERSION;


    g_itemFirmwareVersion.title =
            "Firmware Version";


    g_itemFirmwareVersion.description =
            "Display firmware version";


    g_itemFirmwareVersion.type =
            MENU_ITEM_INFO;



    /*
     * Build Information
     */

    MenuItems_ClearItem(
            &g_itemBuildInformation);


    g_itemBuildInformation.id =
            MENU_ITEM_BUILD_INFORMATION;


    g_itemBuildInformation.title =
            "Build Information";


    g_itemBuildInformation.description =
            "Compiler information";


    g_itemBuildInformation.type =
            MENU_ITEM_INFO;

}



/******************************************************************************
 * Master Item Initialization
 ******************************************************************************/

static void MenuItems_InitItems(void)
{

    /*
     * Main menu items
     */

    MenuItems_InitMainItems();



    /*
     * Stream settings items
     */

    MenuItems_InitStreamItems();



    /*
     * Alarm settings items
     */

    MenuItems_InitAlarmItems();



    /*
     * Calibration items
     */

    MenuItems_InitCalibrationItems();



    /*
     * Service mode items
     */

    MenuItems_InitServiceItems();



    /*
     * Advanced service items
     */

    MenuItems_InitServiceAdvancedItems();



    /*
     * System information items
     */

    MenuItems_InitSystemInfoItems();

}



/******************************************************************************
 * Menu Tree Helper Functions
 ******************************************************************************/

static void MenuItems_LinkItems(
        MenuItem_t *first,
        MenuItem_t *second)
{

    if((first == NULL) ||
       (second == NULL))
    {
        return;
    }


    first->next =
            second;


    second->previous =
            first;

}



/******************************************************************************
 * Assign Items To Page
 ******************************************************************************/

static void MenuItems_AssignPageItems(
        MenuPage_t *page,
        MenuItem_t *firstItem,
        uint16_t count)
{

    if(page == NULL)
    {
        return;
    }


    page->firstItem =
            firstItem;


    page->itemCount =
            count;


    page->selectedItem =
            firstItem;


    page->selectedIndex =
            0U;

}



/******************************************************************************
 * Main Menu Tree
 ******************************************************************************/

static void MenuItems_BuildMainTree(void)
{

    MenuItems_LinkItems(
            &g_itemLiveMonitor,
            &g_itemStartStream);



    MenuItems_LinkItems(
            &g_itemStartStream,
            &g_itemStreamSettings);



    MenuItems_LinkItems(
            &g_itemStreamSettings,
            &g_itemAlarmSettings);



    MenuItems_LinkItems(
            &g_itemAlarmSettings,
            &g_itemCalibration);



    MenuItems_LinkItems(
            &g_itemCalibration,
            &g_itemServiceMode);



    MenuItems_LinkItems(
            &g_itemServiceMode,
            &g_itemSystemInfo);



    MenuItems_AssignPageItems(
            &g_mainMenuPage,
            &g_itemLiveMonitor,
            7U);

}



/******************************************************************************
 * Stream Settings Tree
 ******************************************************************************/

static void MenuItems_BuildStreamTree(void)
{

    MenuItems_LinkItems(
            &g_itemBaudRate,
            &g_itemSampleRate);



    MenuItems_AssignPageItems(
            &g_streamSettingsPage,
            &g_itemBaudRate,
            2U);

}



/******************************************************************************
 *
 * End Of Part 5/6
 *
 ******************************************************************************/

/******************************************************************************
 * Alarm Settings Tree
 ******************************************************************************/

static void MenuItems_BuildAlarmTree(void)
{

    MenuItems_LinkItems(
            &g_itemAlarmEnable,
            &g_itemLowVoltageLimit);



    MenuItems_LinkItems(
            &g_itemLowVoltageLimit,
            &g_itemHighVoltageLimit);



    MenuItems_AssignPageItems(
            &g_alarmSettingsPage,
            &g_itemAlarmEnable,
            3U);

}



/******************************************************************************
 * Calibration Tree
 ******************************************************************************/

static void MenuItems_BuildCalibrationTree(void)
{

    MenuItems_LinkItems(
            &g_itemInputCalibration,
            &g_itemOutputCalibration);



    MenuItems_AssignPageItems(
            &g_calibrationPage,
            &g_itemInputCalibration,
            2U);

}



/******************************************************************************
 * Service Mode Tree
 ******************************************************************************/

static void MenuItems_BuildServiceTree(void)
{

    MenuItems_LinkItems(
            &g_itemButtonTest,
            &g_itemBuzzerTest);



    MenuItems_LinkItems(
            &g_itemBuzzerTest,
            &g_itemLcdTest);



    MenuItems_LinkItems(
            &g_itemLcdTest,
            &g_itemServiceAdvanced);



    MenuItems_AssignPageItems(
            &g_serviceModePage,
            &g_itemButtonTest,
            4U);

}



/******************************************************************************
 * Advanced Service Tree
 ******************************************************************************/

static void MenuItems_BuildServiceAdvancedTree(void)
{

    MenuItems_LinkItems(
            &g_itemFactoryCalibration,
            &g_itemRestoreDefault);



    MenuItems_AssignPageItems(
            &g_serviceAdvancedPage,
            &g_itemFactoryCalibration,
            2U);

}



/******************************************************************************
 * System Information Tree
 ******************************************************************************/

static void MenuItems_BuildSystemInfoTree(void)
{

    MenuItems_LinkItems(
            &g_itemFirmwareVersion,
            &g_itemBuildInformation);



    MenuItems_AssignPageItems(
            &g_systemInfoPage,
            &g_itemFirmwareVersion,
            2U);

}



/******************************************************************************
 * Public Initialization
 ******************************************************************************/

MenuResult_t MenuItems_Init(void)
{

    MenuItems_InitPages();


    MenuItems_InitItems();



    /*
     * Build complete menu hierarchy.
     */

    MenuItems_BuildMainTree();


    MenuItems_BuildStreamTree();


    MenuItems_BuildAlarmTree();


    MenuItems_BuildCalibrationTree();


    MenuItems_BuildServiceTree();


    MenuItems_BuildServiceAdvancedTree();


    MenuItems_BuildSystemInfoTree();



    return MENU_RESULT_OK;

}



/******************************************************************************
 * Public Page Access
 ******************************************************************************/

MenuPage_t *MenuItems_GetPage(
        MenuPageId_t id)
{

    switch(id)
    {

        case MENU_PAGE_MAIN:

            return &g_mainMenuPage;


        case MENU_PAGE_LIVE_MONITOR:

            return &g_liveMonitorPage;


        case MENU_PAGE_STREAM_SETTINGS:

            return &g_streamSettingsPage;


        case MENU_PAGE_ALARM_SETTINGS:

            return &g_alarmSettingsPage;


        case MENU_PAGE_CALIBRATION:

            return &g_calibrationPage;


        case MENU_PAGE_SERVICE_MODE:

            return &g_serviceModePage;


        case MENU_PAGE_SERVICE_ADVANCED:

            return &g_serviceAdvancedPage;


        case MENU_PAGE_SYSTEM_INFO:

            return &g_systemInfoPage;


        default:

            return NULL;

    }

}



/******************************************************************************
 * Public Item Access
 *
 * Restored complete lookup table.
 *
 * This function provides access to every
 * static MenuItem_t object.
 ******************************************************************************/

MenuItem_t *MenuItems_GetItem(
        MenuItemId_t id)
{

    switch(id)
    {

        case MENU_ITEM_LIVE_MONITOR:

            return &g_itemLiveMonitor;


        case MENU_ITEM_START_STREAM:

            return &g_itemStartStream;


        case MENU_ITEM_STREAM_SETTINGS:

            return &g_itemStreamSettings;


        case MENU_ITEM_ALARM_SETTINGS:

            return &g_itemAlarmSettings;


        case MENU_ITEM_CALIBRATION:

            return &g_itemCalibration;


        case MENU_ITEM_SERVICE_MODE:

            return &g_itemServiceMode;


        case MENU_ITEM_SYSTEM_INFO:

            return &g_itemSystemInfo;



        case MENU_ITEM_BAUD_RATE:

            return &g_itemBaudRate;


        case MENU_ITEM_SAMPLE_RATE:

            return &g_itemSampleRate;



        case MENU_ITEM_ALARM_ENABLE:

            return &g_itemAlarmEnable;


        case MENU_ITEM_LOW_VOLTAGE_LIMIT:

            return &g_itemLowVoltageLimit;


        case MENU_ITEM_HIGH_VOLTAGE_LIMIT:

            return &g_itemHighVoltageLimit;



        case MENU_ITEM_INPUT_CALIBRATION:

            return &g_itemInputCalibration;


        case MENU_ITEM_OUTPUT_CALIBRATION:

            return &g_itemOutputCalibration;



        case MENU_ITEM_BUTTON_TEST:

            return &g_itemButtonTest;


        case MENU_ITEM_BUZZER_TEST:

            return &g_itemBuzzerTest;


        case MENU_ITEM_LCD_TEST:

            return &g_itemLcdTest;


        case MENU_ITEM_SERVICE_ADVANCED:

            return &g_itemServiceAdvanced;



        case MENU_ITEM_FACTORY_CALIBRATION:

            return &g_itemFactoryCalibration;


        case MENU_ITEM_RESTORE_DEFAULT:

            return &g_itemRestoreDefault;



        case MENU_ITEM_FIRMWARE_VERSION:

            return &g_itemFirmwareVersion;


        case MENU_ITEM_BUILD_INFORMATION:

            return &g_itemBuildInformation;



        default:

            return NULL;

    }

}



/******************************************************************************
 * Root Page Access
 ******************************************************************************/

MenuPage_t *MenuItems_GetRootPage(void)
{

    return &g_mainMenuPage;

}



/******************************************************************************
 * Version Information
 ******************************************************************************/

const char *MenuItems_GetVersion(void)
{

    return MENU_ITEMS_VERSION_STRING;

}



/******************************************************************************
 * Revision History
 ******************************************************************************/

/*

Version 2.0.4

Date:
2026-07-22


Changes:


    - Restored missing initialization functions.


    - Restored complete MenuItems_GetItem()
      access table.


    - Fixed linker and runtime lookup issues.


    - Preserved:

          menu_types.h v2.0.0 compatibility

          menu_engine.c v2.1.0 compatibility

          static allocation architecture

          callback driven design


*/



/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
