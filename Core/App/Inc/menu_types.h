/******************************************************************************
 *
 * @file    menu_types.h
 *
 * @brief   Common Type Definitions For Dual Voltage Monitor Menu Framework
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
 *      This file contains every common type shared by the complete
 *      menu framework.
 *
 *      This header is intentionally hardware independent.
 *
 *      Modules using this file:
 *
 *          - menu_engine
 *          - menu_renderer
 *          - menu_items
 *          - menu_controller
 *          - application callbacks
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/******************************************************************************
 * Forward Declarations
 ******************************************************************************/

typedef struct MenuItem          MenuItem_t;
typedef struct MenuPage          MenuPage_t;
typedef struct MenuEngine        MenuEngine_t;
typedef struct MenuEditContext   MenuEditContext_t;

/******************************************************************************
 * Result Codes
 ******************************************************************************/

typedef enum
{
    MENU_RESULT_OK = 0,

    MENU_RESULT_ERROR,

    MENU_RESULT_INVALID_PARAMETER,

    MENU_RESULT_NULL_POINTER,

    MENU_RESULT_NOT_FOUND,

    MENU_RESULT_BUSY,

    MENU_RESULT_NOT_ALLOWED

} MenuResult_t;

/******************************************************************************
 * Menu Engine State
 ******************************************************************************/

typedef enum
{
    MENU_STATE_IDLE = 0,

    MENU_STATE_NAVIGATION,

    MENU_STATE_EDIT,

    MENU_STATE_ACTION,

    MENU_STATE_MESSAGE,

    MENU_STATE_LOCKED,

    MENU_STATE_ERROR

} MenuState_t;

/******************************************************************************
 * Menu Events
 ******************************************************************************/

typedef enum
{
    MENU_EVENT_NONE = 0,

    MENU_EVENT_UP,

    MENU_EVENT_DOWN,

    MENU_EVENT_ENTER,

    MENU_EVENT_BACK,

    MENU_EVENT_PAGE_ENTER,

    MENU_EVENT_PAGE_EXIT,

    MENU_EVENT_SELECTION_CHANGED,

    MENU_EVENT_VALUE_CHANGED

} MenuEvent_t;

/******************************************************************************
 * Menu Item Type
 ******************************************************************************/

typedef enum
{
    MENU_ITEM_NORMAL = 0,

    MENU_ITEM_ACTION,

    MENU_ITEM_SUBMENU,

    MENU_ITEM_INFO,

    MENU_ITEM_VALUE,

    MENU_ITEM_EDIT,

    MENU_ITEM_MONITOR,

    MENU_ITEM_CUSTOM

} MenuItemType_t;

/******************************************************************************
 * Menu Page Type
 ******************************************************************************/

typedef enum
{
    MENU_PAGE_STANDARD = 0,

    MENU_PAGE_SETTINGS,

    MENU_PAGE_MONITOR,

    MENU_PAGE_INFORMATION,

    MENU_PAGE_SERVICE,

    MENU_PAGE_CUSTOM

} MenuPageType_t;

/******************************************************************************
 * Menu Navigation Command
 ******************************************************************************/

typedef enum
{
    MENU_NAV_NONE = 0,

    MENU_NAV_UP,

    MENU_NAV_DOWN,

    MENU_NAV_LEFT,

    MENU_NAV_RIGHT,

    MENU_NAV_ENTER,

    MENU_NAV_BACK

} MenuNavigation_t;


/******************************************************************************
 * Menu Visibility
 ******************************************************************************/

typedef enum
{
    MENU_VISIBLE = 0,

    MENU_HIDDEN,

    MENU_DISABLED

} MenuVisibility_t;


/******************************************************************************
 * Menu Edit Type
 ******************************************************************************/

typedef enum
{
    MENU_EDIT_TYPE_NONE = 0,

    MENU_EDIT_UINT8,

    MENU_EDIT_INT8,

    MENU_EDIT_UINT16,

    MENU_EDIT_INT16,

    MENU_EDIT_UINT32,

    MENU_EDIT_INT32,

    MENU_EDIT_FLOAT,

    MENU_EDIT_BOOL

} MenuEditType_t;


/******************************************************************************
 * Scroll Mode
 ******************************************************************************/

typedef enum
{
    MENU_SCROLL_CLAMP = 0,

    MENU_SCROLL_WRAP,

    MENU_SCROLL_DISABLED

} MenuScrollMode_t;


/******************************************************************************
 * Text Alignment
 ******************************************************************************/

typedef enum
{
    MENU_ALIGN_LEFT = 0,

    MENU_ALIGN_CENTER,

    MENU_ALIGN_RIGHT

} MenuAlignment_t;


/******************************************************************************
 * Callback Types
 ******************************************************************************/

typedef void (*MenuCallback_t)(void);

typedef void (*MenuItemCallback_t)(MenuItem_t *item);

typedef void (*MenuPageCallback_t)(MenuPage_t *page);

typedef int32_t (*MenuValueGetter_t)(void);

typedef void (*MenuValueSetter_t)(int32_t value);

typedef bool (*MenuValidator_t)(int32_t value);


/******************************************************************************
 * Menu Selection State
 ******************************************************************************/

typedef struct
{
    uint16_t selectedIndex;

    uint16_t firstVisibleIndex;

    uint8_t visibleRows;

    bool selectionChanged;

    bool pageChanged;

} MenuSelectionState_t;


/******************************************************************************
 * Menu Statistics
 ******************************************************************************/

typedef struct
{
    uint32_t inputCount;

    uint32_t pageChangeCount;

    uint32_t editCount;

    uint32_t errorCount;

} MenuStatistics_t;


/******************************************************************************
 * Menu Configuration
 ******************************************************************************/

typedef struct
{
    uint8_t visibleRows;

    bool enableWrapAround;

    bool enableCursor;

    bool enableStatistics;

} MenuConfig_t;

/******************************************************************************
 * Menu Edit Context
 ******************************************************************************/

struct MenuEditContext
{
    MenuEditType_t type;

    void *data;

    int32_t minimum;

    int32_t maximum;

    int32_t step;

    int32_t originalValue;

    bool modified;

    MenuValidator_t validator;

    MenuCallback_t onCommit;

    MenuCallback_t onCancel;
};


/******************************************************************************
 * Menu Item
 ******************************************************************************/

struct MenuItem
{
    /**********************************************************************
     * Identification
     **********************************************************************/

    uint16_t id;

    const char *title;

    const char *description;

    MenuItemType_t type;

    MenuVisibility_t visibility;


    /**********************************************************************
     * Tree Navigation
     **********************************************************************/

    MenuItem_t *parent;

    MenuItem_t *child;

    MenuItem_t *next;

    MenuItem_t *previous;

    MenuPage_t *childPage;


    /**********************************************************************
     * Value Management
     **********************************************************************/

    void *data;

    MenuEditType_t dataType;

    MenuValueGetter_t getter;

    MenuValueSetter_t setter;

    MenuValidator_t validator;


    /**********************************************************************
     * Item Callbacks
     **********************************************************************/

    MenuItemCallback_t onEnter;

    MenuItemCallback_t onSelect;

    MenuItemCallback_t onExit;


    /*
     * Compatibility callback used by the existing
     * menu engine implementation.
     */
    MenuItemCallback_t enterCallback;


    /**********************************************************************
     * User Extension
     **********************************************************************/

    void *userData;
};

/******************************************************************************
 * Menu Page
 ******************************************************************************/

struct MenuPage
{
    /**********************************************************************
     * Identification
     **********************************************************************/

    uint16_t id;

    const char *title;

    MenuPageType_t type;


    /**********************************************************************
     * Item Management
     **********************************************************************/

    MenuItem_t *firstItem;

    MenuItem_t *lastItem;

    MenuItem_t *selectedItem;

    uint16_t itemCount;

    uint16_t selectedIndex;

    uint8_t visibleRows;


    /**********************************************************************
     * Navigation
     **********************************************************************/

    MenuPage_t *parentPage;

    MenuPage_t *previousPage;


    /**********************************************************************
     * Page Callbacks
     **********************************************************************/

    MenuPageCallback_t onEnter;

    MenuPageCallback_t onExit;


    /**********************************************************************
     * Renderer
     **********************************************************************/

    bool redrawRequired;

    void *userData;
};


/******************************************************************************
 * Menu Engine Runtime
 ******************************************************************************/

struct MenuEngine
{
    /**********************************************************************
     * Runtime State
     **********************************************************************/

    MenuState_t state;

    MenuPage_t *currentPage;

    MenuItem_t *currentItem;


    /**********************************************************************
     * Edit Context
     **********************************************************************/

    MenuEditContext_t editContext;


    /**********************************************************************
     * Navigation Configuration
     **********************************************************************/

    MenuScrollMode_t scrollMode;


    /**********************************************************************
     * Renderer Synchronization
     **********************************************************************/

    MenuSelectionState_t selection;


    /**********************************************************************
     * Last Event
     **********************************************************************/

    MenuEvent_t lastEvent;


    /**********************************************************************
     * Runtime Configuration
     **********************************************************************/

    MenuConfig_t config;


    /**********************************************************************
     * Statistics
     **********************************************************************/

    MenuStatistics_t statistics;


    /**********************************************************************
     * Compatibility Fields
     *
     * Required by menu_engine.c
     **********************************************************************/

    bool editMode;

    bool pageChanged;

    bool selectionChanged;


    /**********************************************************************
     * User Extension
     **********************************************************************/

    void *userData;
};
/******************************************************************************
 * Default Configuration
 ******************************************************************************/

#define MENU_DEFAULT_VISIBLE_ROWS         (3U)

#define MENU_DEFAULT_WRAP_AROUND          (false)

#define MENU_DEFAULT_CURSOR_ENABLE        (true)

#define MENU_DEFAULT_STATISTICS_ENABLE    (false)


/******************************************************************************
 * Utility Macros
 ******************************************************************************/

/*
 * Pointer validation
 */
#define MENU_IS_VALID_POINTER(ptr)        ((ptr) != NULL)


/*
 * Boolean to integer conversion
 */
#define MENU_BOOL_TO_UINT(value)          ((value) ? 1U : 0U)


/*
 * Number of elements in a static array
 */
#define MENU_ARRAY_SIZE(array) \
    (sizeof(array) / sizeof((array)[0]))


/******************************************************************************
 * Version Information
 ******************************************************************************/

#define MENU_TYPES_VERSION_MAJOR          (2U)

#define MENU_TYPES_VERSION_MINOR          (0U)

#define MENU_TYPES_VERSION_PATCH          (0U)

#define MENU_TYPES_VERSION_STRING         "2.0.0"


/******************************************************************************
 * Module Design Notes
 *
 * The menu framework is divided into four independent layers:
 *
 *      +------------------------------+
 *      |      Application Layer       |
 *      +--------------+---------------+
 *                     |
 *                     v
 *      +------------------------------+
 *      |       Menu Callbacks         |
 *      +--------------+---------------+
 *                     |
 *                     v
 *      +------------------------------+
 *      |        Menu Engine           |
 *      +--------------+---------------+
 *                     |
 *                     v
 *      +------------------------------+
 *      |       Menu Renderer          |
 *      +------------------------------+
 *
 * Design Principles
 *
 *  1. Hardware independent.
 *  2. No dynamic memory allocation.
 *  3. Static menu database.
 *  4. Runtime state separated from database.
 *  5. Renderer separated from navigation.
 *  6. Application accessed only through callbacks.
 *
 ******************************************************************************/
/******************************************************************************
 * C++ Compatibility
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MENU_TYPES_H */



