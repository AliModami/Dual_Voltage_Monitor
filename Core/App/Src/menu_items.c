/******************************************************************************
 *
 * @file    menu_items.c
 *
 * @brief   Page Based Menu Database
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor - Page Based Menu
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file contains the complete menu tree definition.
 *
 *      Design rules:
 *
 *      - Menu data is separated from menu control logic.
 *      - No LCD dependency.
 *      - No button dependency.
 *      - No scrolling concept.
 *      - Each page contains maximum three visible items.
 *      - Large menus are divided into multiple pages.
 *
 ******************************************************************************/


#include "menu_items.h"
#include "menu_actions.h"
#include "menu_edit.h"



/*
 * ============================================================================
 * Private Action Prototypes
 * ============================================================================
 */


/*
 * General actions
 */
static void Action_LiveMonitor(void);

static void Action_StartStream(void);



/*
 * Stream settings actions
 */
static void Action_BaudRate(void);

static void Action_SampleRate(void);



/*
 * Alarm settings actions
 */
static void Action_AlarmEnable(void);

static void Action_LowVoltageLimit(void);

static void Action_HighVoltageLimit(void);

static void Action_AlarmMode(void);



/*
 * Calibration actions
 */
static void Action_InputVoltageOffset(void);

static void Action_OutputVoltageOffset(void);



/*
 * Service mode actions
 */
static void Action_BuzzerTest(void);

static void Action_LCDTest(void);

static void Action_AutoCalibrate(void);

static void Action_FactoryDefault(void);





/*
 * ============================================================================
 * MAIN MENU PAGE 0
 * ============================================================================
 *
 * Line 0 : Main Menu
 * Line 1 : > Live Monitor
 * Line 2 :   Start Stream
 * Line 3 :   Stream Settings
 *
 * ============================================================================
 */


static const MenuItem_t main_page0_items[] =
{

    {
        .text       = "Live Monitor",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_LiveMonitor
    },


    {
        .text       = "Start Stream",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_StartStream
    },


    {
        .text       = "Stream Settings",
        .type       = MENU_ITEM_SUBMENU,
        .child_page = MENU_PAGE_STREAM_SETTINGS,
        .action     = 0
    }

};





/*
 * ============================================================================
 * MAIN MENU PAGE 1
 * ============================================================================
 *
 * Line 0 : Main Menu
 * Line 1 :   Alarm Settings
 * Line 2 :   Calibration
 * Line 3 :   Service Mode
 *
 * ============================================================================
 */


static const MenuItem_t main_page1_items[] =
{

    {
        .text       = "Alarm Settings",
        .type       = MENU_ITEM_SUBMENU,
        .child_page = MENU_PAGE_ALARM_SETTINGS_0,
        .action     = 0
    },


    {
        .text       = "Calibration",
        .type       = MENU_ITEM_SUBMENU,
        .child_page = MENU_PAGE_CALIBRATION,
        .action     = 0
    },


    {
        .text       = "Service Mode",
        .type       = MENU_ITEM_SUBMENU,
        .child_page = MENU_PAGE_SERVICE_0,
        .action     = 0
    }

};





/*
 * ============================================================================
 * MAIN MENU PAGE 2
 * ============================================================================
 */


static const MenuItem_t main_page2_items[] =
{

    {
        .text       = "System Info",
        .type       = MENU_ITEM_SUBMENU,
        .child_page = MENU_PAGE_SYSTEM_INFO,
        .action     = 0
    }

};

/*
 * ============================================================================
 * STREAM SETTINGS PAGE
 * ============================================================================
 *
 * Line 0 : Stream Settings
 * Line 1 :   Baud Rate
 * Line 2 :   Sample Rate
 *
 * ============================================================================
 */


static const MenuItem_t stream_settings_items[] =
{

    {
        .text       = "Baud Rate",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_BaudRate
    },


    {
        .text       = "Sample Rate",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_SampleRate
    }

};





/*
 * ============================================================================
 * ALARM SETTINGS PAGE 0
 * ============================================================================
 *
 * Line 0 : Alarm Settings
 * Line 1 :   Alarm Enable
 * Line 2 :   Low Voltage Limit
 * Line 3 :   High Voltage Limit
 *
 * ============================================================================
 */


static const MenuItem_t alarm_settings_page0_items[] =
{

    {
        .text       = "Alarm Enable",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_AlarmEnable
    },


    {
        .text       = "Low Voltage Limit",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_LowVoltageLimit
    },


    {
        .text       = "High Voltage Limit",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_HighVoltageLimit
    }

};





/*
 * ============================================================================
 * ALARM SETTINGS PAGE 1
 * ============================================================================
 */


static const MenuItem_t alarm_settings_page1_items[] =
{

    {
        .text       = "Alarm Mode",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_AlarmMode
    }

};





/*
 * ============================================================================
 * CALIBRATION PAGE
 * ============================================================================
 */


static const MenuItem_t calibration_items[] =
{

    {
        .text       = "Vinput Offset",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_InputVoltageOffset
    },


    {
        .text       = "Voutput Offset",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_OutputVoltageOffset
    }

};





/*
 * ============================================================================
 * SERVICE MODE PAGE 0
 * ============================================================================
 */


static const MenuItem_t service_page0_items[] =
{

    {
        .text       = "Buzzer Test",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_BuzzerTest
    },


    {
        .text       = "LCD Test",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_LCDTest
    },


    {
        .text       = "Auto Calibrate",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_AutoCalibrate
    }

};





/*
 * ============================================================================
 * SERVICE MODE PAGE 1
 * ============================================================================
 */


static const MenuItem_t service_page1_items[] =
{

    {
        .text       = "Factory Default",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = Action_FactoryDefault
    }

};





/*
 * ============================================================================
 * SYSTEM INFORMATION PAGE
 * ============================================================================
 */


static const MenuItem_t system_info_items[] =
{

    {
        .text       = "DVM Logger V1.0",
        .type       = MENU_ITEM_ACTION,
        .child_page = MENU_INVALID_PAGE,
        .action     = 0
    }

};

/*
 * ============================================================================
 * PAGE TABLE
 * ============================================================================
 *
 * Every logical page is described here.
 *
 * Navigation rules:
 *
 *      next_page / previous_page:
 *          Used for page based navigation.
 *
 *      parent_page:
 *          Used by BACK operation.
 *
 *      item_count:
 *          Number of valid items on page.
 *
 * ============================================================================
 */


static const MenuPage_t menu_pages[MENU_PAGE_COUNT] =
{


/*
 * ----------------------------------------------------------------------------
 * MAIN MENU PAGE 0
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_MAIN_0] =
{
    .title         = "Main Menu -- Page1/3",

    .parent_page   = MENU_INVALID_PAGE,

    .items         = main_page0_items,

    .item_count    = 3U,

    .next_page     = MENU_PAGE_MAIN_1,

    .previous_page = MENU_INVALID_PAGE
},





/*
 * ----------------------------------------------------------------------------
 * MAIN MENU PAGE 1
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_MAIN_1] =
{
    .title         = "Main Menu -- Page2/3",

    .parent_page   = MENU_INVALID_PAGE,

    .items         = main_page1_items,

    .item_count    = 3U,

    .next_page     = MENU_PAGE_MAIN_2,

    .previous_page = MENU_PAGE_MAIN_0
},





/*
 * ----------------------------------------------------------------------------
 * MAIN MENU PAGE 2
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_MAIN_2] =
{
    .title         = "Main Menu -- Page3/3",

    .parent_page   = MENU_INVALID_PAGE,

    .items         = main_page2_items,

    .item_count    = 1U,

    .next_page     = MENU_INVALID_PAGE,

    .previous_page = MENU_PAGE_MAIN_1
},





/*
 * ----------------------------------------------------------------------------
 * STREAM SETTINGS
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_STREAM_SETTINGS] =
{
    .title         = "Stream Settings",

    .parent_page   = MENU_PAGE_MAIN_0,

    .items         = stream_settings_items,

    .item_count    = 2U,

    .next_page     = MENU_INVALID_PAGE,

    .previous_page = MENU_INVALID_PAGE
},





/*
 * ----------------------------------------------------------------------------
 * ALARM SETTINGS PAGE 0
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_ALARM_SETTINGS_0] =
{
    .title         = "Alarm Settings",

    .parent_page   = MENU_PAGE_MAIN_1,

    .items         = alarm_settings_page0_items,

    .item_count    = 3U,

    .next_page     = MENU_PAGE_ALARM_SETTINGS_1,

    .previous_page = MENU_INVALID_PAGE
},





/*
 * ----------------------------------------------------------------------------
 * ALARM SETTINGS PAGE 1
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_ALARM_SETTINGS_1] =
{
    .title         = "Alarm Settings",

    .parent_page   = MENU_PAGE_ALARM_SETTINGS_0,

    .items         = alarm_settings_page1_items,

    .item_count    = 1U,

    .next_page     = MENU_INVALID_PAGE,

    .previous_page = MENU_PAGE_ALARM_SETTINGS_0
},





/*
 * ----------------------------------------------------------------------------
 * CALIBRATION
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_CALIBRATION] =
{
    .title         = "Calibration",

    .parent_page   = MENU_PAGE_MAIN_1,

    .items         = calibration_items,

    .item_count    = 2U,

    .next_page     = MENU_INVALID_PAGE,

    .previous_page = MENU_INVALID_PAGE
},





/*
 * ----------------------------------------------------------------------------
 * SERVICE MODE PAGE 0
 * ----------------------------------------------------------------------------
 *
 * Items:
 *
 *      Buzzer Test
 *      LCD Test
 *
 * Button Test removed.
 *
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_SERVICE_0] =
{
    .title         = "Service Mode",

    .parent_page   = MENU_PAGE_MAIN_1,

    .items         = service_page0_items,

    .item_count    = 3U,

    .next_page     = MENU_PAGE_SERVICE_1,

    .previous_page = MENU_INVALID_PAGE
},





/*
 * ----------------------------------------------------------------------------
 * SERVICE MODE PAGE 1
 * ----------------------------------------------------------------------------
 *
 * Items:
 *
 *      Auto Calibrate
 *      Factory Default
 *
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_SERVICE_1] =
{
    .title         = "Service Mode",

    .parent_page   = MENU_PAGE_SERVICE_0,

    .items         = service_page1_items,

    .item_count    = 1U,

    .next_page     = MENU_INVALID_PAGE,

    .previous_page = MENU_PAGE_SERVICE_0
},

/*
 * ----------------------------------------------------------------------------
 * SYSTEM INFORMATION PAGE
 * ----------------------------------------------------------------------------
 */


[MENU_PAGE_SYSTEM_INFO] =
{
    .title         = "System Info",

    .parent_page   = MENU_PAGE_MAIN_2,

    .items         = system_info_items,

    .item_count    = 1U,

    .next_page     = MENU_INVALID_PAGE,

    .previous_page = MENU_INVALID_PAGE
}


};





/*
 * ============================================================================
 * Public API
 * ============================================================================
 *
 * Provides read-only access to menu page database.
 *
 * Used by:
 *
 *      menu_controller.c
 *
 * ============================================================================
 */


const MenuPage_t *MenuItems_GetPage(
        MenuPageId_t page_id)
{

    /*
     * Validate page index.
     */
    if(page_id >= MENU_PAGE_COUNT)
    {
        return 0;
    }


    return &menu_pages[page_id];

}






/*
 * ============================================================================
 * Menu Action Wrappers
 * ============================================================================
 *
 * These functions isolate menu database
 * from application action implementation.
 *
 * ============================================================================
 */



static void Action_LiveMonitor(void)
{
    MenuAction_LiveMonitor();
}





static void Action_StartStream(void)
{
    MenuAction_StartStream();
}





static void Action_BaudRate(void)
{
    MenuAction_BaudRate();
}





static void Action_SampleRate(void)
{
    MenuAction_SampleRate();
}





static void Action_AlarmEnable(void)
{
    MenuAction_AlarmEnable();
}





static void Action_LowVoltageLimit(void)
{
    MenuAction_LowVoltageLimit();
}





static void Action_HighVoltageLimit(void)
{
    MenuAction_HighVoltageLimit();
}





static void Action_AlarmMode(void)
{
    MenuAction_AlarmMode();
}





static void Action_InputVoltageOffset(void)
{
    MenuAction_InputVoltageOffset();
}





static void Action_OutputVoltageOffset(void)
{
    MenuAction_OutputVoltageOffset();
}





static void Action_BuzzerTest(void)
{
    MenuAction_BuzzerTest();
}





static void Action_LCDTest(void)
{
    MenuAction_LCDTest();
}





static void Action_AutoCalibrate(void)
{
    MenuAction_AutoCalibrate();
}





static void Action_FactoryDefault(void)
{
    MenuAction_FactoryDefault();
}





/******************************************************************************
 *
 *                              END OF FILE
 *
 *      menu_items.c
 *
 *      Version:
 *
 *          Clean Final v1.1.0
 *
 *
 *      Changes:
 *
 *          - Removed Button Test item from Service Mode.
 *          - Renamed Factory Calibration to Auto Calibrate.
 *          - Renamed Restore Default to Factory Default.
 *          - Removed unused Button Test action wrapper.
 *          - Preserved Page Based Menu architecture.
 *          - Preserved controller and renderer compatibility.
 *
 ******************************************************************************/
