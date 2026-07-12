/******************************************************************************
 * @file    menu_items.c
 * @brief   Static Menu Database
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * MCU :
 *      STM32F103C8T6
 *
 * Framework :
 *      STM32 HAL
 *
 *-----------------------------------------------------------------------------
 *
 * Author :
 *      Ali Modami & OpenAI
 *
 *-----------------------------------------------------------------------------
 *
 * Description
 * ============================================================================
 *
 * این فایل بانک اطلاعاتی (Database) سیستم Menu را ایجاد می‌کند.
 *
 * این فایل:
 *
 *      ✓ Menu Engine نیست
 *      ✓ Menu Renderer نیست
 *      ✓ LCD Driver نیست
 *      ✓ Button Handler نیست
 *
 * مسئولیت این فایل فقط:
 *
 *      • ایجاد Page ها
 *      • ایجاد MenuItem ها
 *      • لینک کردن Item ها
 *      • اتصال Callback ها
 *
 *-----------------------------------------------------------------------------
 *
 * Architecture
 *
 *              menu_types.h
 *                     │
 *                     ▼
 *              menu_items.h
 *                     │
 *                     ▼
 *              menu_items.c
 *              (Database Layer)
 *                     │
 *                     ▼
 *              menu_engine.c
 *             (Navigation Layer)
 *                     │
 *                     ▼
 *             menu_renderer.c
 *              (Display Layer)
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "menu_items.h"

#include <stddef.h>

/******************************************************************************
 * External Action Functions
 ******************************************************************************/

/*
 * Action Layer
 *
 * Callback ها در فایل menu_action.c
 * پیاده‌سازی می‌شوند.
 */

#include "menu_actions.h"


//extern void Menu_Action_LiveMonitor(MenuItem_t *item);
//
//extern void Menu_Action_StartStream(MenuItem_t *item);
//
//extern void Menu_Action_SystemInfo(MenuItem_t *item);
//
//extern void Menu_Action_Calibration(MenuItem_t *item);
//
//extern void Menu_Action_ServiceMode(MenuItem_t *item);

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

static void Menu_CreatePages(void);

static void Menu_CreateItems(void);

static void Menu_LinkItems(void);

static void Menu_AttachCallbacks(void);

/******************************************************************************
 * Global Page Objects
 ******************************************************************************/

MenuPage_t g_mainMenuPage;

MenuPage_t g_streamSettingsPage;

MenuPage_t g_alarmSettingsPage;

MenuPage_t g_calibrationPage;

MenuPage_t g_serviceModePage;

MenuPage_t g_systemInfoPage;

/******************************************************************************
 * Global Menu Items
 ******************************************************************************/

/*========================= Main Menu =========================*/

MenuItem_t g_liveMonitorItem;

MenuItem_t g_startStreamItem;

MenuItem_t g_streamSettingsItem;

MenuItem_t g_alarmSettingsItem;

MenuItem_t g_calibrationItem;

MenuItem_t g_serviceModeItem;

MenuItem_t g_systemInfoItem;

/*===================== Stream Settings =======================*/

MenuItem_t g_baudRateItem;

MenuItem_t g_sampleRateItem;

/*====================== Alarm Settings =======================*/

MenuItem_t g_alarmEnableItem;

MenuItem_t g_lowVoltageItem;

MenuItem_t g_highVoltageItem;

MenuItem_t g_alarmModeItem;

/*======================= Calibration =========================*/

MenuItem_t g_inputCalibrationItem;

MenuItem_t g_outputCalibrationItem;

MenuItem_t g_factoryCalibrationItem;

/*======================= Service Mode ========================*/

MenuItem_t g_buttonTestItem;

MenuItem_t g_buzzerTestItem;

MenuItem_t g_lcdTestItem;

MenuItem_t g_restoreDefaultItem;

/******************************************************************************
 *
 * Menu_CreatePages()
 *
 ******************************************************************************/

/*
 * این تابع فقط Page Object ها را مقداردهی اولیه می‌کند.
 *
 * در این مرحله:
 *
 *      ✓ شناسه صفحه
 *      ✓ عنوان صفحه
 *      ✓ Parent Page
 *
 * تنظیم می‌شوند.
 *
 * هنوز:
 *
 *      • MenuItem ها
 *      • Linked List
 *      • Callback ها
 *
 * ایجاد نشده‌اند.
 */

static void Menu_CreatePages(void)
{

    /**********************************************************************
     * Main Menu
     **********************************************************************/

    g_mainMenuPage.id             = MENU_PAGE_MAIN;
    g_mainMenuPage.title          = "Main Menu";
    g_mainMenuPage.firstItem      = NULL;
    g_mainMenuPage.lastItem       = NULL;
    g_mainMenuPage.selectedItem   = NULL;
    g_mainMenuPage.parentPage     = NULL;
    g_mainMenuPage.itemCount      = 0;


    /**********************************************************************
     * Stream Settings
     **********************************************************************/

    g_streamSettingsPage.id            = MENU_PAGE_STREAM;
    g_streamSettingsPage.title         = "Stream Settings";
    g_streamSettingsPage.firstItem     = NULL;
    g_streamSettingsPage.lastItem      = NULL;
    g_streamSettingsPage.selectedItem  = NULL;
    g_streamSettingsPage.parentPage    = &g_mainMenuPage;
    g_streamSettingsPage.itemCount     = 0;


    /**********************************************************************
     * Alarm Settings
     **********************************************************************/

    g_alarmSettingsPage.id            = MENU_PAGE_ALARM;
    g_alarmSettingsPage.title         = "Alarm Settings";
    g_alarmSettingsPage.firstItem     = NULL;
    g_alarmSettingsPage.lastItem      = NULL;
    g_alarmSettingsPage.selectedItem  = NULL;
    g_alarmSettingsPage.parentPage    = &g_mainMenuPage;
    g_alarmSettingsPage.itemCount     = 0;


    /**********************************************************************
     * Calibration
     **********************************************************************/

    g_calibrationPage.id            = MENU_PAGE_CALIBRATION;
    g_calibrationPage.title         = "Calibration";
    g_calibrationPage.firstItem     = NULL;
    g_calibrationPage.lastItem      = NULL;
    g_calibrationPage.selectedItem  = NULL;
    g_calibrationPage.parentPage    = &g_mainMenuPage;
    g_calibrationPage.itemCount     = 0;


    /**********************************************************************
     * Service Mode
     **********************************************************************/

    g_serviceModePage.id            = MENU_PAGE_SERVICE;
    g_serviceModePage.title         = "Service Mode";
    g_serviceModePage.firstItem     = NULL;
    g_serviceModePage.lastItem      = NULL;
    g_serviceModePage.selectedItem  = NULL;
    g_serviceModePage.parentPage    = &g_mainMenuPage;
    g_serviceModePage.itemCount     = 0;


    /**********************************************************************
     * System Information
     **********************************************************************/

    g_systemInfoPage.id            = MENU_PAGE_SYSTEM_INFO;
    g_systemInfoPage.title         = "System Information";
    g_systemInfoPage.firstItem     = NULL;
    g_systemInfoPage.lastItem      = NULL;
    g_systemInfoPage.selectedItem  = NULL;
    g_systemInfoPage.parentPage    = &g_mainMenuPage;
    g_systemInfoPage.itemCount     = 0;
}

/******************************************************************************
 *
 * Menu_CreateItems()
 *
 ******************************************************************************/

/*
 * این تابع تمام MenuItem های سیستم را ایجاد می‌کند.
 *
 * در این مرحله فقط:
 *
 *      • ID
 *      • Title
 *      • Type
 *      • Owner Page
 *      • Child Page
 *
 * مقداردهی می‌شوند.
 *
 * لینک شدن Linked List در تابع جداگانه:
 *
 *      Menu_LinkItems()
 *
 * انجام خواهد شد.
 *
 ******************************************************************************/

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
     * نمایش لحظه‌ای ولتاژ ورودی و خروجی
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
     * شروع ارسال اطلاعات UART
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
     */
//------------------------------------------------------------------------
//    g_systemInfoItem.id =
//            MENU_ITEM_ID_SYSTEM_INFO;
//
//    g_systemInfoItem.title =
//            "System Information";
//
//    g_systemInfoItem.type =
//            MENU_ITEM_ACTION;

    /*
     * System Information
     *
     * ورود به صفحه اطلاعات سیستم
     */

    g_systemInfoItem.id =
            MENU_ITEM_ID_SYSTEM_INFO;

    g_systemInfoItem.title =
            "System Information";

    g_systemInfoItem.type =
            MENU_ITEM_SUBMENU;



//--------------------------------------------------------------


    g_systemInfoItem.ownerPage =
            &g_mainMenuPage;


//--------------------------------------------------------------------------
//    g_systemInfoItem.childPage =
//            NULL;

    g_systemInfoItem.childPage =
            &g_systemInfoPage;

//--------------------------------------------------------------------------

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
    //-----------------------------------------------------------------------------------------------------------------------------------


    /**********************************************************************
     *
     * Alarm Settings Page Items
     *
     **********************************************************************/


    /*
     * Alarm Enable
     *
     * فعال یا غیرفعال کردن سیستم هشدار
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
     * Low Voltage Limit
     *
     * حد پایین ولتاژ
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
     * High Voltage Limit
     *
     * حد بالای ولتاژ
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
     * Once / Repeat
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
     * کالیبراسیون کانال اندازه‌گیری ورودی
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
     * کالیبراسیون کانال اندازه‌گیری خروجی
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
     * بازگردانی کالیبراسیون کارخانه
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
     * بازگردانی تنظیمات کارخانه
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

/*
 * این تابع Linked List مربوط به Item ها را ایجاد می‌کند.
 *
 * نکته معماری:
 *
 * Menu Engine فقط از Pointer های:
 *
 *      firstItem
 *      next
 *      previous
 *
 * استفاده خواهد کرد.
 *
 * Database Layer مسئول ایجاد این ارتباط است.
 *
 ******************************************************************************/

static void Menu_LinkItems(void)
{


    /**********************************************************************
     *
     * Main Menu Linked List
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


    g_mainMenuPage.itemCount = 7;



    /**********************************************************************
     *
     * Stream Settings Linked List
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


    g_streamSettingsPage.itemCount = 2;


    /**********************************************************************
     *
     * Alarm Settings Linked List
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


    g_alarmSettingsPage.itemCount = 4;



    /**********************************************************************
     *
     * Calibration Linked List
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


    g_calibrationPage.itemCount = 3;



    /**********************************************************************
     *
     * Service Mode Linked List
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


    g_serviceModePage.itemCount = 4;



    /**********************************************************************
     *
     * Set Default Selected Item
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


}


/******************************************************************************
 *
 * Menu_AttachCallbacks()
 *
 ******************************************************************************/

/*
 * در این تابع فقط Callback های اجرایی به Action Item ها متصل می‌شوند.
 *
 * Database Layer فقط اتصال را انجام می‌دهد.
 *
 * اجرای Action توسط Menu Engine انجام خواهد شد.
 *
 ******************************************************************************/

static void Menu_AttachCallbacks(void)
{


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



    /*
     * Calibration
     */

    g_inputCalibrationItem.enterCallback =
            Menu_Action_Calibration;


    g_outputCalibrationItem.enterCallback =
            Menu_Action_Calibration;


    g_factoryCalibrationItem.enterCallback =
            Menu_Action_Calibration;



    /*
     * Service Mode
     */

    g_buttonTestItem.enterCallback =
            Menu_Action_ServiceMode;


    g_buzzerTestItem.enterCallback =
            Menu_Action_ServiceMode;


    g_lcdTestItem.enterCallback =
            Menu_Action_ServiceMode;


    g_restoreDefaultItem.enterCallback =
            Menu_Action_ServiceMode;

}

/******************************************************************************
 *
 * Menu_ItemsInit()
 *
 ******************************************************************************/

/*
 * این تابع نقطه شروع ایجاد Database منو است.
 *
 * ترتیب اجرا مهم است:
 *
 *      1) ایجاد Page ها
 *
 *      2) ایجاد Item ها
 *
 *      3) لینک کردن Item ها
 *
 *      4) اتصال Callback ها
 *
 *
 * بعد از اجرای این تابع:
 *
 *      Menu Engine
 *
 * می‌تواند بدون دانستن جزئیات Database
 * با سیستم Menu کار کند.
 *
 ******************************************************************************/

void Menu_ItemsInit(void)
{

    /*
     * Create Pages
     */

    Menu_CreatePages();



    /*
     * Create Items
     */

    Menu_CreateItems();



    /*
     * Link Items
     */

    Menu_LinkItems();



    /*
     * Attach Actions
     */

    Menu_AttachCallbacks();

}



/******************************************************************************
 *
 * Menu_GetMainPage()
 *
 ******************************************************************************/

/*
 * Return:
 *
 *      Pointer to Root Menu Page
 *
 *
 * این تابع تنها راه استاندارد برای دریافت
 * صفحه اصلی منو توسط Menu Engine است.
 *
 ******************************************************************************/

MenuPage_t *Menu_GetMainPage(void)
{

    return &g_mainMenuPage;

}



/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
