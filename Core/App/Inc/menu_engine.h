/******************************************************************************
 *
 * @file    menu_engine.h
 *
 * @brief   Runtime Menu Navigation Engine
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
 *
 *      Runtime navigation engine of the menu framework.
 *
 *      This module owns ONLY the runtime state of the menu system.
 *
 *      It never owns menu pages or menu items.
 *
 *------------------------------------------------------------------------------
 *
 * Responsibilities
 *
 *      • Current page management
 *      • Current item management
 *      • Navigation state machine
 *      • Edit mode control
 *      • Callback execution
 *      • Renderer refresh notification
 *
 *------------------------------------------------------------------------------
 *
 * Dependencies
 *
 *      menu_types.h
 *
 ******************************************************************************/

#ifndef MENU_ENGINE_H
#define MENU_ENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdbool.h>

#include "menu_types.h"


/******************************************************************************
 * Initialization
 ******************************************************************************/

/**
 * @brief Initialize Menu Engine runtime.
 */
void MenuEngine_Init(void);


/**
 * @brief Reset runtime state.
 */
void MenuEngine_Reset(void);


/******************************************************************************
 * Event Processing
 ******************************************************************************/

/**
 * @brief Process one navigation event.
 *
 * @param event
 *      Navigation event.
 */
void MenuEngine_ProcessEvent(MenuEvent_t event);


/******************************************************************************
 * Runtime Access
 ******************************************************************************/

/**
 * @brief Return engine instance.
 */
MenuEngine_t *MenuEngine_GetInstance(void);


/**
 * @brief Return current page.
 */
MenuPage_t *MenuEngine_GetCurrentPage(void);


/**
 * @brief Return selected item.
 */
MenuItem_t *MenuEngine_GetSelectedItem(void);
/******************************************************************************
 * Navigation
 ******************************************************************************/

/**
 * @brief Move selection to previous item.
 */
void MenuEngine_MoveUp(void);


/**
 * @brief Move selection to next item.
 */
void MenuEngine_MoveDown(void);


/**
 * @brief Execute currently selected item.
 */
void MenuEngine_ExecuteSelectedItem(void);


/**
 * @brief Return to parent page.
 */
void MenuEngine_Back(void);


/******************************************************************************
 * Engine State
 ******************************************************************************/

/**
 * @brief Get current engine state.
 */
MenuState_t MenuEngine_GetState(void);


/**
 * @brief Set engine state.
 *
 * @param state
 *      New runtime state.
 */
void MenuEngine_SetState(MenuState_t state);


/******************************************************************************
 * Edit Mode
 ******************************************************************************/

/**
 * @brief Check whether edit mode is active.
 *
 * @retval true
 * @retval false
 */
bool MenuEngine_IsEditMode(void);


/**
 * @brief Enable or disable edit mode.
 *
 * @param enable
 */
void MenuEngine_SetEditMode(bool enable);

/******************************************************************************
 * Refresh Management
 ******************************************************************************/

/**
 * @brief Request complete page redraw.
 *
 * @details
 * Sets the page refresh flag. The renderer should perform a full
 * redraw during its next update cycle.
 */
void MenuEngine_RequestPageRefresh(void);


/**
 * @brief Request cursor-only refresh.
 *
 * @details
 * Sets the selection refresh flag. The renderer should update only
 * the cursor without rebuilding the whole page.
 */
void MenuEngine_RequestSelectionRefresh(void);


/**
 * @brief Check whether a page refresh is pending.
 *
 * @retval true
 * @retval false
 */
bool MenuEngine_IsPageChanged(void);


/**
 * @brief Clear page refresh request.
 */
void MenuEngine_ClearPageChanged(void);


/**
 * @brief Check whether selection refresh is pending.
 *
 * @retval true
 * @retval false
 */
bool MenuEngine_IsSelectionChanged(void);


/**
 * @brief Clear selection refresh request.
 */
void MenuEngine_ClearSelectionChanged(void);


/******************************************************************************
 * Information
 ******************************************************************************/

/**
 * @brief Return number of menu items in current page.
 */
uint8_t MenuEngine_GetItemCount(void);


/**
 * @brief Check whether current page is root page.
 */
bool MenuEngine_IsRootPage(void);


/**
 * @brief Return current page identifier.
 */
uint16_t MenuEngine_GetCurrentPageId(void);


/**
 * @brief Return current item identifier.
 */
uint16_t MenuEngine_GetCurrentItemId(void);
/******************************************************************************
 * Engine Synchronization
 ******************************************************************************/

/**
 * @brief Synchronize runtime pointers.
 *
 * @details
 * Ensures that:
 *
 *      currentPage
 *      currentItem
 *      selectedItem
 *
 * remain consistent.
 *
 * Normally used internally, but exposed for debugging.
 */
void MenuEngine_DebugRefresh(void);


/**
 * @brief Periodic runtime update.
 *
 * @details
 * Current implementation is event driven.
 *
 * Reserved for future support of:
 *
 *      • Timeouts
 *      • Auto return
 *      • Message timers
 *      • Long key processing
 */
void MenuEngine_Update(void);


/******************************************************************************
 * Runtime Query
 ******************************************************************************/

/**
 * @brief Check whether engine has an active page.
 *
 * @retval true
 * @retval false
 */
bool MenuEngine_HasCurrentPage(void);


/**
 * @brief Check whether engine has a selected item.
 *
 * @retval true
 * @retval false
 */
bool MenuEngine_HasSelectedItem(void);


/**
 * @brief Check whether navigation is currently allowed.
 *
 * @retval true
 * @retval false
 */
bool MenuEngine_IsNavigationEnabled(void);


/******************************************************************************
 * Compatibility Interface
 *
 * These APIs are intentionally preserved because the existing
 * menu_engine.c implementation and the renderer use them.
 ******************************************************************************/

/**
 * @brief Request complete renderer refresh.
 */
void MenuEngine_RequestFullRefresh(void);


/**
 * @brief Request cursor-only refresh.
 */
void MenuEngine_RequestCursorRefresh(void);
//******************************************************************************
 //* Design Rules
 //*
// ******************************************************************************

/*
 * Rule 1
 *
 * The Menu Engine owns only the runtime state.
 *
 * Menu pages and menu items are permanently allocated
 * by the database layer.




 * Rule 2
 *
 * The Menu Engine never accesses hardware directly.
 *
 * All user interaction arrives through MenuEvent_t.




 * Rule 3
 *
 * Rendering is completely independent of navigation.
 *
 * The engine communicates with the renderer only through
 * refresh flags.




 * Rule 4
 *
 * Application-specific behavior shall be implemented only
 * through callback functions stored inside MenuItem objects.




 * Rule 5
 *
 * Dynamic memory allocation is prohibited.




 * Rule 6
 *
 * The engine never modifies the menu database structure.
 *
 * It only changes runtime pointers.



******************************************************************************
 * Thread Safety
 *
 ******************************************************************************


 * Current implementation assumes a single execution context.
 *
 * Typical execution order:
 *
 *      Button_Update()
 *              |
 *              v
 *      MenuEngine_ProcessEvent()
 *              |
 *              v
 *      MenuRenderer_Update()
 *              |
 *              v
 *      Application_Update()
 *
 * If an RTOS is introduced later, external synchronization
 * shall be implemented by the application layer.



******************************************************************************
 * Future Extension Notes
 *
 ******************************************************************************


 * Reserved extension points:
 *
 *      - Timeout processing
 *      - Message manager
 *      - Long key detection
 *      - Encoder support
 *      - Touch interface
 *      - Multi-language support
 *      - Dynamic page generation
 *      - Animation support
 *      - RTOS message queue integration

******************************************************************************
 * C++ Compatibility
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MENU_ENGINE_H */


/*
 * *****************************************************************************
 *
 * Revision History
 *
 ******************************************************************************


 * Version : 2.0.0
 *
 * Architectural Updates
 *
 *      • Runtime interface unified.
 *
 *      • Public API synchronized with
 *        menu_engine.c runtime.
 *
 *      • Refresh request API standardized.
 *
 *      • Edit mode interface unified.
 *
 *      • Navigation interface simplified.
 *
 *      • Renderer dependency removed.
 *
 *      • Hardware independence preserved.
 *
 *      • Ready for integration with:
 *
 *              menu_types.h
 *              menu_items.h
 *              menu_items.c
 *              menu_renderer.h
 *              menu_renderer.c
 *



*/
