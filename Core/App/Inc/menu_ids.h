/******************************************************************************
 * @file    menu_ids.h
 * @brief   Global Menu Identifier Definitions
 *-----------------------------------------------------------------------------
 * Project :
 *      Dual Voltage Monitor
 *
 * Target :
 *      STM32F103C8T6 (Blue Pill)
 *
 * Framework :
 *      STM32 HAL
 *
 * IDE :
 *      STM32CubeIDE
 *
 *-----------------------------------------------------------------------------
 * WHY THIS FILE EXISTS
 *-----------------------------------------------------------------------------
 *
 *  In small embedded projects developers usually access menu pages
 *  directly using pointers.
 *
 *  That approach works for small software but becomes difficult to
 *  maintain as the project grows.
 *
 *  This project uses a different architecture.
 *
 *  Every Menu Page and every Menu Item owns a UNIQUE ID.
 *
 *  The rest of the firmware communicates using these IDs instead of
 *  directly accessing pointers.
 *
 *  Example:
 *
 *      Menu_Open(MENU_ID_SYSTEM_INFO);
 *
 *      Menu_Select(MENU_ID_SAMPLE_RATE);
 *
 *      Menu_Find(MENU_ID_ALARM_ENABLE);
 *
 *-----------------------------------------------------------------------------
 *
 * Advantages
 *
 *      • Better readability
 *
 *      • Easier debugging
 *
 *      • Less module dependency
 *
 *      • Future UART control
 *
 *      • Future Bluetooth control
 *
 *      • Future Touch LCD support
 *
 *      • Event logging
 *
 *      • Automation
 *
 *-----------------------------------------------------------------------------
 *
 * System Architecture
 *
 *
 *                   Application
 *                        │
 *                        ▼
 *                 Menu Engine
 *                        │
 *                        ▼
 *                 menu_ids.h
 *                        │
 *                        ▼
 *          Find Menu Object By ID
 *                        │
 *                        ▼
 *                  Menu Renderer
 *
 *-----------------------------------------------------------------------------
 *
 * Design Philosophy
 *
 *      IDs never change.
 *
 *      Menu titles may change.
 *
 *      LCD pages may change.
 *
 *      Navigation may change.
 *
 *      But IDs remain fixed forever.
 *
 *-----------------------------------------------------------------------------
 *
 * NOTE
 *
 *      Never reuse an old ID for another purpose.
 *
 *      If an item is removed,
 *      keep its ID reserved.
 *
 ******************************************************************************/

#ifndef MENU_IDS_H
#define MENU_IDS_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/******************************************************************************
 *
 * MENU IDENTIFIERS
 *
 * Every menu object in the system owns
 * one unique identifier.
 *
 ******************************************************************************/

typedef enum
{
    /**********************************************************************
     * Root Menu
     **********************************************************************/

    MENU_ID_MAIN = 0,

    /**********************************************************************
     * Main Menu
     **********************************************************************/

    MENU_ID_LIVE_MONITOR,

    MENU_ID_START_STREAM,

    MENU_ID_STREAM_SETTINGS,

    MENU_ID_ALARM_SETTINGS,

    MENU_ID_CALIBRATION,

    MENU_ID_SERVICE_MODE,

    MENU_ID_SYSTEM_INFO,

    /**********************************************************************
     * Stream Settings
     **********************************************************************/

    MENU_ID_BAUD_RATE,

    MENU_ID_SAMPLE_RATE,

    /**********************************************************************
     * Alarm Settings
     **********************************************************************/

    MENU_ID_ALARM_ENABLE,

    MENU_ID_LOW_VOLTAGE_LIMIT,

    MENU_ID_HIGH_VOLTAGE_LIMIT,

    MENU_ID_ALARM_MODE,

    /**********************************************************************
     * Calibration
     **********************************************************************/

    MENU_ID_INPUT_CALIBRATION,

    MENU_ID_OUTPUT_CALIBRATION,

    MENU_ID_FACTORY_CALIBRATION,

    /**********************************************************************
     * Service Mode
     **********************************************************************/

    MENU_ID_BUTTON_TEST,

    MENU_ID_BUZZER_TEST,

    MENU_ID_LCD_TEST,

    MENU_ID_RESTORE_DEFAULT,

    /**********************************************************************
     * Internal Pages
     **********************************************************************/

    MENU_ID_CONFIRM_DIALOG,

    MENU_ID_MESSAGE_BOX,

    MENU_ID_NUMERIC_EDITOR,

    MENU_ID_BOOLEAN_EDITOR,

    MENU_ID_ENUM_EDITOR,

    MENU_ID_CALIBRATION_WIZARD,

    /**********************************************************************
     * Reserved IDs
     *
     * These IDs are intentionally left
     * available for future development.
     **********************************************************************/

    MENU_ID_RESERVED_01,

    MENU_ID_RESERVED_02,

    MENU_ID_RESERVED_03,

    MENU_ID_RESERVED_04,

    MENU_ID_RESERVED_05,

    /**********************************************************************
     * Always Keep Last
     **********************************************************************/

    MENU_ID_COUNT

} MenuId_t;


/******************************************************************************
 *
 * DESIGN NOTES
 *
 ******************************************************************************/

/*
    Why not use strings?

        "Sample Rate"

    Because strings consume Flash
    and are slower to compare.

------------------------------------------------------------

    Why not use pointers?

    Because other modules should not
    know internal addresses.

------------------------------------------------------------

    Why Enum?

    Because:

        • Fast

        • Safe

        • Readable

        • Easy to Debug

        • Supported by IDE

*/


/******************************************************************************
 *
 * WHEN SHOULD THIS FILE CHANGE?
 *
 ******************************************************************************/

/*
    Modify this file ONLY when:

        • A new menu page is added.

        • A new menu item is created.

        • A new internal page is required.

    Otherwise this file should remain stable.
*/


/******************************************************************************
 *
 * FUTURE DEVELOPMENT
 *
 ******************************************************************************/

/*
    Planned future IDs:

        MENU_ID_EVENT_LOG

        MENU_ID_ADC_MONITOR

        MENU_ID_UART_SETTINGS

        MENU_ID_FACTORY_TEST

        MENU_ID_PASSWORD

        MENU_ID_DEVICE_SERIAL

        MENU_ID_LANGUAGE

        MENU_ID_DISPLAY_SETTINGS

        MENU_ID_DATE_TIME

        MENU_ID_FIRMWARE_UPDATE

*/


/******************************************************************************
 *
 * LEARNING NOTES
 *
 ******************************************************************************/

/*
    This file is intentionally very simple.

    It contains NO functions.

    It contains NO variables.

    It contains NO application logic.

    Its only responsibility is defining a stable set
    of identifiers used by the complete Menu Engine.

    Think of these IDs as the "names" of every page.

    The Menu Engine can always locate a page by its ID
    without knowing where that page is stored in memory.

    This greatly reduces coupling between modules
    and makes future maintenance much easier.
*/


#ifdef __cplusplus
}
#endif

#endif /* MENU_IDS_H */

/******************************************************************************
 *
 * END OF FILE
 *
 ******************************************************************************/
