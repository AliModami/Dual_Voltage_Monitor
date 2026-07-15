/******************************************************************************
 *
 * File Name :
 *
 *      menu_types.h
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
 * Common data type definitions shared by the complete Menu Framework.
 *
 * This file is the central data contract of the menu architecture.
 *
 * It contains only:
 *
 *      • Enumerations
 *      • Structures
 *      • Callback type definitions
 *      • Public constants
 *
 * No implementation code exists in this file.
 *
 *------------------------------------------------------------------------------
 *
 * Architecture
 * =============================================================================
 *
 *                  +----------------------+
 *                  |    menu_types.h      |
 *                  +----------------------+
 *
 *                     Data Contract Layer
 *
 *              +-----------+-----------+
 *              |           |           |
 *              |           |           |
 *              v           v           v
 *
 *         menu_items   menu_engine   menu_renderer
 *
 * Every menu module depends on this file.
 * This file depends only on the C standard library.
 *
 *------------------------------------------------------------------------------
 *
 * Design Rules
 * =============================================================================
 *
 * 1. No implementation code.
 *
 * 2. No global variables.
 *
 * 3. No hardware dependency.
 *
 * 4. No HAL dependency.
 *
 * 5. No LCD dependency.
 *
 * 6. Keep this file completely platform independent.
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

#ifndef MENU_TYPES_H
#define MENU_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************
 *
 * Forward Declarations
 *
 * Forward declarations eliminate circular dependencies between
 * MenuPage, MenuItem and MenuEngine structures.
 *
 ******************************************************************************/

typedef struct MenuItem   MenuItem_t;
typedef struct MenuPage   MenuPage_t;
typedef struct MenuEngine MenuEngine_t;

/******************************************************************************
 *
 * Design Philosophy
 * =============================================================================
 *
 * The menu framework is built around only three runtime objects.
 *
 *                  +----------------+
 *                  |   MenuPage     |
 *                  +----------------+
 *                           |
 *                           | owns
 *                           v
 *                  +----------------+
 *                  |   MenuItem     |
 *                  +----------------+
 *                           ^
 *                           |
 *                  +----------------+
 *                  |  MenuEngine    |
 *                  +----------------+
 *
 * MenuPage
 * --------
 * Owns a collection of MenuItem objects.
 *
 * MenuItem
 * --------
 * Represents one selectable entry.
 *
 * MenuEngine
 * ----------
 * Controls navigation state only.
 *
 * Responsibilities are intentionally separated.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Menu Item Types
 *
 * These values determine how MenuEngine behaves when the user
 * presses ENTER on a menu item.
 *
 ******************************************************************************/

typedef enum
{
    /*
     * Passive item.
     * Display only.
     */
    MENU_ITEM_NORMAL = 0,

    /*
     * Opens another menu page.
     */
    MENU_ITEM_SUBMENU,

    /*
     * Enters edit mode.
     */
    MENU_ITEM_EDIT,

    /*
     * Displays or edits a configurable value.
     */
    MENU_ITEM_VALUE,

    /*
     * Executes an application callback.
     */
    MENU_ITEM_ACTION,

    /*
     * Read-only information item.
     */
    MENU_ITEM_INFO

} MenuItemType_t;

/******************************************************************************
 *
 * Menu Engine State
 *
 * The Menu Engine operates as a simple finite state machine (FSM).
 *
 * Only one state can be active at any given time.
 *
 ******************************************************************************/

typedef enum
{
    /**********************************************************************
     * Engine is inactive.
     **********************************************************************/
    MENU_STATE_IDLE = 0,

    /**********************************************************************
     * Normal menu navigation.
     *
     * UP/DOWN move the selection.
     * ENTER activates the selected item.
     * BACK returns to the parent page.
     **********************************************************************/
    MENU_STATE_NAVIGATION,

    /**********************************************************************
     * Value editing mode.
     *
     * The selected menu item is currently being modified.
     **********************************************************************/
    MENU_STATE_EDIT,

    /**********************************************************************
     * Action execution state.
     *
     * Used while an application callback is running.
     **********************************************************************/
    MENU_STATE_ACTION,

    /**********************************************************************
     * Temporary message state.
     *
     * Examples:
     *      - Saved
     *      - Completed
     *      - Error
     *      - Warning
     *
     * During this state the renderer normally displays a temporary
     * message instead of the menu itself.
     **********************************************************************/
    MENU_STATE_MESSAGE

} MenuState_t;


/******************************************************************************
 *
 * Menu Events
 *
 * The Menu Engine never communicates directly with the button driver.
 *
 * Hardware-specific button events are translated into these logical
 * events by the application layer.
 *
 *      Physical Button            Menu Event
 *      ----------------           ----------------
 *      BUTTON_UP          ----->  MENU_EVENT_UP
 *      BUTTON_DOWN        ----->  MENU_EVENT_DOWN
 *      BUTTON_ENTER       ----->  MENU_EVENT_ENTER
 *      BUTTON_BACK        ----->  MENU_EVENT_BACK
 *
 ******************************************************************************/

typedef enum
{
    /**********************************************************************
     * No pending event.
     **********************************************************************/
    MENU_EVENT_NONE = 0,

    /**********************************************************************
     * Move selection upward.
     **********************************************************************/
    MENU_EVENT_UP,

    /**********************************************************************
     * Move selection downward.
     **********************************************************************/
    MENU_EVENT_DOWN,

    /**********************************************************************
     * Execute the selected item.
     **********************************************************************/
    MENU_EVENT_ENTER,

    /**********************************************************************
     * Return to the parent page.
     **********************************************************************/
    MENU_EVENT_BACK

} MenuEvent_t;


/******************************************************************************
 *
 * Menu Page Identifier
 *
 * Every page has a unique identifier.
 *
 * These IDs are intended for:
 *
 *      • Navigation
 *      • Debugging
 *      • Logging
 *      • Application decisions
 *
 ******************************************************************************/

typedef enum
{
    /**********************************************************************
     * Root menu page.
     **********************************************************************/
    MENU_PAGE_MAIN = 0,

    /**********************************************************************
     * UART streaming configuration.
     **********************************************************************/
    MENU_PAGE_STREAM,

    /**********************************************************************
     * Alarm configuration page.
     **********************************************************************/
    MENU_PAGE_ALARM,

    /**********************************************************************
     * ADC calibration page.
     **********************************************************************/
    MENU_PAGE_CALIBRATION,

    /**********************************************************************
     * Hardware diagnostics and service page.
     **********************************************************************/
    MENU_PAGE_SERVICE,

    /**********************************************************************
     * Firmware and hardware information page.
     **********************************************************************/
    MENU_PAGE_SYSTEM_INFO

} MenuPageId_t;
/******************************************************************************
 *
 * Menu Item Identifier
 *
 * Every MenuItem has a unique identifier that never changes,
 * regardless of its position inside the menu hierarchy.
 *
 * The application layer should always identify menu items by their ID
 * instead of their displayed title or screen position.
 *
 ******************************************************************************/

typedef enum
{

    /**********************************************************************
     *
     * Main Menu
     *
     **********************************************************************/

    /*
     * Opens the real-time voltage monitor screen.
     */
    MENU_ITEM_ID_LIVE_MONITOR = 0,

    /*
     * Starts UART streaming.
     */
    MENU_ITEM_ID_START_STREAM,

    /*
     * Opens the Stream Settings submenu.
     */
    MENU_ITEM_ID_STREAM_SETTINGS,

    /*
     * Opens the Alarm Settings submenu.
     */
    MENU_ITEM_ID_ALARM_SETTINGS,

    /*
     * Opens the Calibration submenu.
     */
    MENU_ITEM_ID_CALIBRATION,

    /*
     * Opens the Service Mode submenu.
     */
    MENU_ITEM_ID_SERVICE_MODE,

    /*
     * Opens the System Information page.
     */
    MENU_ITEM_ID_SYSTEM_INFO,



    /**********************************************************************
     *
     * Stream Settings
     *
     **********************************************************************/

    /*
     * UART communication baud rate.
     */
    MENU_ITEM_ID_BAUD_RATE,

    /*
     * ADC sampling interval.
     */
    MENU_ITEM_ID_SAMPLE_RATE,



    /**********************************************************************
     *
     * Alarm Settings
     *
     **********************************************************************/

    /*
     * Enable / Disable alarm system.
     */
    MENU_ITEM_ID_ALARM_ENABLE,

    /*
     * Low voltage threshold.
     */
    MENU_ITEM_ID_LOW_VOLTAGE,

    /*
     * High voltage threshold.
     */
    MENU_ITEM_ID_HIGH_VOLTAGE,

    /*
     * Alarm operating mode.
     */
    MENU_ITEM_ID_ALARM_MODE,



    /**********************************************************************
     *
     * Calibration
     *
     **********************************************************************/

    /*
     * Input voltage calibration.
     */
    MENU_ITEM_ID_INPUT_CALIBRATION,

    /*
     * Output voltage calibration.
     */
    MENU_ITEM_ID_OUTPUT_CALIBRATION,

    /*
     * Restore factory calibration values.
     */
    MENU_ITEM_ID_FACTORY_CALIBRATION,



    /**********************************************************************
     *
     * Service Mode
     *
     **********************************************************************/

    /*
     * Physical button diagnostic.
     */
    MENU_ITEM_ID_BUTTON_TEST,

    /*
     * Buzzer diagnostic.
     */
    MENU_ITEM_ID_BUZZER_TEST,

    /*
     * LCD diagnostic.
     */
    MENU_ITEM_ID_LCD_TEST,

    /*
     * Restore factory default settings.
     */
    MENU_ITEM_ID_RESTORE_DEFAULT,



    /**********************************************************************
     *
     * End Marker
     *
     * This is NOT a real menu item.
     *
     * It is used only for counting the total number of menu IDs.
     *
     **********************************************************************/

    MENU_ITEM_ID_COUNT

} MenuItemId_t;


/******************************************************************************
 *
 * Menu Callback Type
 *
 * Every executable menu action uses the same callback prototype.
 *
 * This abstraction allows MenuEngine to execute actions without
 * knowing any application-specific implementation details.
 *
 ******************************************************************************/

/*
 * Generic callback prototype.
 */
typedef void (*MenuCallback_t)(MenuItem_t *item);
/******************************************************************************
 *
 * Menu Item Object
 *
 * A MenuItem is the smallest executable or displayable element inside
 * the menu database.
 *
 * Every MenuItem belongs to exactly one MenuPage.
 *
 * Depending on its type, a MenuItem may:
 *
 *      • Execute an application callback
 *      • Open a child menu page
 *      • Display a configurable value
 *      • Enter value edit mode
 *      • Display read-only information
 *
 * The Menu Engine never allocates MenuItem objects dynamically.
 * All MenuItem instances are statically created inside menu_items.c.
 *
 ******************************************************************************/

struct MenuItem
{

    /**********************************************************************
     *
     * Item Identity
     *
     **********************************************************************/

    /*
     * Unique item identifier.
     */
    MenuItemId_t id;

    /*
     * Display title shown by the renderer.
     */
    const char *title;

    /*
     * Item behavior type.
     */
    MenuItemType_t type;



    /**********************************************************************
     *
     * Navigation Information
     *
     **********************************************************************/

    /*
     * Owner page.
     *
     * Every MenuItem belongs to exactly one page.
     */
    MenuPage_t *ownerPage;

    /*
     * Child page.
     *
     * Used only when the item type is MENU_ITEM_SUBMENU.
     * Otherwise this pointer is NULL.
     */
    MenuPage_t *childPage;

    /*
     * Next item in the linked list.
     */
    MenuItem_t *next;

    /*
     * Previous item in the linked list.
     */
    MenuItem_t *previous;



    /**********************************************************************
     *
     * Action Callback
     *
     **********************************************************************/

    /*
     * Callback executed when ENTER is pressed.
     *
     * For non-action items this pointer may be NULL.
     */
    MenuCallback_t enterCallback;

};

/******************************************************************************
 *
 * Menu Page Object
 *
 * A MenuPage owns a collection of MenuItem objects.
 *
 * The Menu Engine always operates on one active page.
 *
 * Each page contains a doubly linked list of MenuItem objects.
 *
 *                 +-----------------------------+
 *                 |         MenuPage            |
 *                 +-----------------------------+
 *                 | id                          |
 *                 | title                       |
 *                 | firstItem ----------------------+
 *                 | lastItem ------------------+    |
 *                 | selectedItem -----------+  |    |
 *                 +-------------------------|--|----+
 *                                           |  |
 *                                           v  v
 *
 *      Item1 <----> Item2 <----> Item3 <----> Item4
 *
 * The renderer uses the selectedItem pointer to determine which
 * menu entry should be highlighted.
 *
 ******************************************************************************/

struct MenuPage
{

    /**********************************************************************
     *
     * Page Identity
     *
     **********************************************************************/

    /*
     * Unique page identifier.
     */
    MenuPageId_t id;

    /*
     * Page title displayed by the renderer.
     */
    const char *title;



    /**********************************************************************
     *
     * Linked List
     *
     **********************************************************************/

    /*
     * First item belonging to this page.
     */
    MenuItem_t *firstItem;

    /*
     * Last item belonging to this page.
     */
    MenuItem_t *lastItem;

    /*
     * Currently selected item.
     *
     * This pointer represents the current cursor position while the
     * page is active.
     */
    MenuItem_t *selectedItem;



    /**********************************************************************
     *
     * Navigation
     *
     **********************************************************************/

    /*
     * Parent page.
     *
     * The root page always has a NULL parent.
     */
    MenuPage_t *parentPage;



    /**********************************************************************
     *
     * Statistics
     *
     **********************************************************************/

    /*
     * Total number of items contained in this page.
     */
    uint8_t itemCount;

};

/******************************************************************************
 *
 * Menu Engine Object
 *
 * The MenuEngine is the runtime core of the Menu Framework.
 *
 * It stores only the current navigation state.
 *
 * IMPORTANT
 * ============================================================================
 *
 * The MenuEngine does NOT own any MenuPage or MenuItem objects.
 *
 * All pages and menu items are created statically inside menu_items.c.
 *
 * The engine only stores pointers to those objects and updates them during
 * navigation.
 *
 * Responsibilities
 * ============================================================================
 *
 *      • Keep track of the active page
 *      • Keep track of the selected item
 *      • Manage the navigation state machine
 *      • Request renderer updates
 *      • Control edit mode
 *
 * Non-Responsibilities
 * ============================================================================
 *
 *      • Drawing on the LCD
 *      • Creating menu pages
 *      • Executing hardware drivers
 *      • Managing application settings
 *
 ******************************************************************************/

struct MenuEngine
{

    /**********************************************************************
     *
     * Current Navigation Context
     *
     **********************************************************************/

    /*
     * Currently active menu page.
     *
     * Every navigation operation is performed relative to this page.
     */
    MenuPage_t *currentPage;

    /*
     * Currently selected menu item.
     *
     * This pointer should always be synchronized with
     * currentPage->selectedItem.
     */
    MenuItem_t *currentItem;



    /**********************************************************************
     *
     * State Machine
     *
     **********************************************************************/

    /*
     * Current engine state.
     *
     * Examples:
     *
     *      MENU_STATE_NAVIGATION
     *      MENU_STATE_EDIT
     *      MENU_STATE_ACTION
     */
    MenuState_t state;



    /**********************************************************************
     *
     * Renderer Status Flags
     *
     **********************************************************************/

    /*
     * Requests a complete page redraw.
     *
     * This flag becomes true when:
     *
     *      • Entering a new page
     *      • Returning to a parent page
     *      • Resetting the menu
     */
    bool pageChanged;

    /*
     * Requests only a cursor refresh.
     *
     * This flag is set when the selected menu item changes without
     * changing the current page.
     */
    bool selectionChanged;

    /*
     * Indicates whether the engine is currently editing a value.
     */
    bool editMode;

};

/******************************************************************************
 *
 * Public Configuration Constants
 *
 * These constants define generic characteristics of the menu framework.
 *
 * They are intentionally kept independent from the LCD driver so that
 * the menu system can be reused with different display hardware.
 *
 ******************************************************************************/

/*
 * Number of menu rows that can be displayed simultaneously.
 *
 * For a 20x4 LCD:
 *
 *      Line 1 : Page Title
 *      Line 2 : Menu Item
 *      Line 3 : Menu Item
 *      Line 4 : Menu Item
 *
 * Therefore only three menu items are visible at one time.
 */
#define MENU_VISIBLE_ROWS              (3U)


/*
 * Maximum length of a menu item title.
 *
 * This value is used by the renderer for alignment and validation.
 *
 * Note:
 * The title itself is stored as a constant string pointer.
 * No memory is allocated based on this value.
 */
#define MENU_MAX_TITLE_LENGTH          (20U)


/******************************************************************************
 *
 * Design Notes
 *
 * The following relationships are maintained throughout the framework:
 *
 *      MenuPage
 *          ├── firstItem
 *          ├── lastItem
 *          └── selectedItem
 *
 *      MenuItem
 *          ├── ownerPage
 *          ├── childPage
 *          ├── next
 *          ├── previous
 *          └── enterCallback
 *
 *      MenuEngine
 *          ├── currentPage
 *          ├── currentItem
 *          ├── state
 *          ├── pageChanged
 *          ├── selectionChanged
 *          └── editMode
 *
 * These relationships allow each module to focus on a single
 * responsibility while keeping coupling between modules low.
 *
 ******************************************************************************/
/******************************************************************************
 *
 * Layer Responsibilities
 *
 * The menu framework is divided into independent layers.
 *
 * Each layer owns one clearly defined responsibility.
 *
 ******************************************************************************/

/*
 * ---------------------------------------------------------------------------
 * menu_types.h
 * ---------------------------------------------------------------------------
 *
 * Purpose
 * -------
 * Defines all public data types shared by the framework.
 *
 * Responsibilities
 * ----------------
 *      • Enumerations
 *      • Structures
 *      • Callback definitions
 *      • Public constants
 *
 * Does NOT contain
 * ----------------
 *      • Global variables
 *      • Executable code
 *      • Hardware access
 *
 */


/*
 * ---------------------------------------------------------------------------
 * menu_items.c
 * ---------------------------------------------------------------------------
 *
 * Purpose
 * -------
 * Implements the static menu database.
 *
 * Responsibilities
 * ----------------
 *      • Create menu pages
 *      • Create menu items
 *      • Build linked lists
 *      • Connect callback functions
 *
 */


/*
 * ---------------------------------------------------------------------------
 * menu_engine.c
 * ---------------------------------------------------------------------------
 *
 * Purpose
 * -------
 * Implements all navigation logic.
 *
 * Responsibilities
 * ----------------
 *      • Process menu events
 *      • Move the cursor
 *      • Enter and leave pages
 *      • Execute callbacks
 *      • Maintain runtime state
 *
 */


/*
 * ---------------------------------------------------------------------------
 * menu_renderer.c
 * ---------------------------------------------------------------------------
 *
 * Purpose
 * -------
 * Converts the current menu state into a visual representation.
 *
 * Responsibilities
 * ----------------
 *      • Draw page titles
 *      • Draw menu items
 *      • Draw selection cursor
 *      • Minimize LCD refresh operations
 *
 */
/*
 * ---------------------------------------------------------------------------
 * menu_actions.c
 * ---------------------------------------------------------------------------
 *
 * Purpose
 * -------
 * Implements application-specific behavior.
 *
 * Responsibilities
 * ----------------
 *      • Execute menu actions
 *      • Start monitoring
 *      • Start data streaming
 *      • Open information screens
 *      • Launch calibration procedures
 *
 * This layer is the only part of the framework that should interact
 * directly with the application logic.
 *
 */


/******************************************************************************
 *
 * Dependency Rules
 *
 * The following dependency hierarchy must always be preserved.
 *
 ******************************************************************************/

/*
 *
 *                     menu_types.h
 *                           │
 *                           ▼
 *                     menu_items.h
 *                           │
 *                           ▼
 *                     menu_items.c
 *                           │
 *                           ▼
 *                     menu_engine.c
 *                           │
 *                           ▼
 *                    menu_renderer.c
 *
 *
 *                     menu_actions.c
 *                           ▲
 *                           │
 *                    (Callbacks Only)
 *
 *
 * Rules
 * ============================================================================
 *
 * 1.
 * menu_types.h shall remain completely independent from the rest of the
 * framework.
 *
 * 2.
 * menu_items.c owns the static menu database.
 *
 * 3.
 * menu_engine.c must never create or destroy menu objects.
 *
 * 4.
 * menu_renderer.c shall never modify the menu database.
 *
 * 5.
 * menu_actions.c shall never perform navigation directly.
 *
 * 6.
 * Navigation decisions always belong to Menu Engine.
 *
 */
/******************************************************************************
 *
 * Coding Guidelines
 *
 * The following rules apply to every source file that belongs to the
 * Menu Framework.
 *
 ******************************************************************************/

/*
 * Rule 1
 * -------
 * Every module must have a single, clearly defined responsibility.
 */


/*
 * Rule 2
 * -------
 * Dynamic memory allocation is not permitted.
 *
 * All menu pages and menu items shall be statically allocated.
 */


/*
 * Rule 3
 * -------
 * Hardware-dependent code must never appear in this file.
 *
 * This header shall remain completely portable.
 */


/*
 * Rule 4
 * -------
 * Every public type shall be documented using Doxygen-style comments.
 */


/*
 * Rule 5
 * -------
 * Navigation logic belongs exclusively to Menu Engine.
 *
 * Database modules must never perform navigation.
 */


/*
 * Rule 6
 * -------
 * Renderer modules must never modify runtime data.
 *
 * Rendering is a read-only operation.
 */


/*
 * Rule 7
 * -------
 * Application-specific behavior shall be implemented only through
 * callback functions.
 */


/*
 * Rule 8
 * -------
 * Public structures should remain stable to preserve compatibility
 * with other framework modules.
 */


/******************************************************************************
 *
 * File Summary
 *
 * This file defines every public data type required by the Menu Framework.
 *
 * It intentionally contains:
 *
 *      • No executable code
 *      • No global objects
 *      • No hardware dependency
 *      • No application logic
 *
 * This design allows the entire framework to remain modular,
 * reusable, and easy to maintain.
 *
 ******************************************************************************/

/******************************************************************************
 *
 * Maintenance Notes
 *
 * Future modifications to this file should follow the rules below.
 *
 ******************************************************************************/

/*
 * 1.
 * New menu pages shall be added only by extending MenuPageId_t.
 */


/*
 * 2.
 * New menu items shall be added only by extending MenuItemId_t.
 *
 * Existing identifiers must never be reordered because application
 * code may depend on their values.
 */


/*
 * 3.
 * New runtime states shall be added to MenuState_t only when a
 * distinct state machine behavior is required.
 */


/*
 * 4.
 * New menu events shall represent logical user actions rather than
 * physical hardware events.
 */


/*
 * 5.
 * Callback prototypes should remain generic.
 *
 * This keeps Menu Engine independent from application modules.
 */


/******************************************************************************
 *
 * Compatibility
 *
 * This header is intended to be compatible with:
 *
 *      • C99
 *      • C11
 *      • C++
 *
 * The C++ compatibility block ensures that function declarations
 * retain C linkage when included from C++ source files.
 *
 ******************************************************************************/

/*
 * Nothing below this point should contain implementation code.
 *
 * This file is intentionally limited to declarations and public
 * definitions only.
 */

/******************************************************************************
 *
 * Public API Overview
 *
 * This header is the foundation of the Menu Framework.
 *
 * All other menu modules depend on the public data types defined here.
 *
 * Module Responsibilities
 * ============================================================================
 *
 *  menu_types.h
 *      Public data contract shared by the entire framework.
 *
 *  menu_items.c
 *      Creates the static menu database.
 *
 *  menu_engine.c
 *      Implements navigation and runtime state management.
 *
 *  menu_renderer.c
 *      Renders the current menu state on the display.
 *
 *  menu_actions.c
 *      Implements application-specific menu behavior.
 *
 * This separation ensures:
 *
 *      • Low module coupling
 *      • High maintainability
 *      • Easy testing
 *      • Hardware independence
 *      • Clear ownership of responsibilities
 *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MENU_TYPES_H */

/******************************************************************************
 *
 * End of File
 *
 ******************************************************************************/
