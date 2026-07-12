/******************************************************************************
 * @file    menu_engine.h
 * @brief   Menu Navigation and Control Engine Interface
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
 * این فایل Interface موتور اصلی منو را تعریف می‌کند.
 *
 *
 * فلسفه معماری:
 * ---------------------------------------------------------------------------
 *
 * Menu Engine فقط مسئول:
 *
 *      - مدیریت Navigation
 *      - نگهداری وضعیت جاری
 *      - پردازش Event های منو
 *      - انتخاب Page و Item فعال
 *
 * می‌باشد.
 *
 *
 * این ماژول هیچ شناختی از:
 *
 *      LCD
 *      Button GPIO
 *      UART
 *      ADC
 *      Application Logic
 *
 * ندارد.
 *
 *
 * ارتباط با سایر لایه‌ها:
 *
 *
 *          Button Layer
 *                |
 *                |
 *                v
 *          Menu Event
 *                |
 *                |
 *                v
 *          Menu Engine
 *                |
 *                |
 *                v
 *          Menu Renderer
 *
 *
 ******************************************************************************/

#ifndef MENU_ENGINE_H
#define MENU_ENGINE_H


/******************************************************************************
 *
 * Includes
 *
 ******************************************************************************/

/*
 * فقط Type های پایه منو از این فایل استفاده می‌شوند.
 *
 * menu_types.h مالک:
 *
 *      MenuItem_t
 *      MenuPage_t
 *      MenuState_t
 *      MenuEvent_t
 *
 * می‌باشد.
 *
 */

#include "menu_types.h"



/******************************************************************************
 *
 * Initialization API
 *
 ******************************************************************************/

/**
 * @brief Initialize Menu Engine
 *
 * وظایف:
 *
 *      - مقداردهی اولیه Engine
 *      - انتخاب Page اولیه
 *      - انتخاب Item اولیه
 *      - قرار دادن Engine در حالت Navigation
 *
 */
void MenuEngine_Init(void);



/******************************************************************************
 *
 * Event Processing API
 *
 ******************************************************************************/

/**
 * @brief Process Menu Event
 *
 * دریافت Event از لایه Button Application.
 *
 * مثال:
 *
 *      BUTTON_UP
 *
 * تبدیل شده به:
 *
 *      MENU_EVENT_UP
 *
 *
 * Engine فقط Event را پردازش می‌کند
 * و هیچ اطلاعی از منبع Event ندارد.
 *
 */
void MenuEngine_ProcessEvent(MenuEvent_t event);



/******************************************************************************
 *
 * Current Object Access API
 *
 ******************************************************************************/

/**
 * @brief Get Current Menu Page
 *
 * این تابع برای Renderer استفاده خواهد شد.
 *
 * خروجی:
 *
 *      Page فعال فعلی
 *
 */
MenuPage_t* MenuEngine_GetCurrentPage(void);



/**
 * @brief Get Selected Menu Item
 *
 * این تابع Item فعال فعلی را برمی‌گرداند.
 *
 * Renderer از این اطلاعات برای نمایش
 * علامت انتخاب استفاده می‌کند.
 *
 */
MenuItem_t* MenuEngine_GetSelectedItem(void);



/******************************************************************************
 *
 * State Information API
 *
 ******************************************************************************/

/**
 * @brief Get Current Engine State
 *
 * برای ماژول‌های دیگر مانند Renderer
 * یا Application Layer استفاده می‌شود.
 *
 */
MenuState_t MenuEngine_GetState(void);



/**
 * @brief Get Current Page ID
 *
 * شناسه Page فعال را برمی‌گرداند.
 *
 */
MenuPageId_t MenuEngine_GetCurrentPageId(void);



/**
 * @brief Get Selected Item ID
 *
 * شناسه Item انتخاب شده را برمی‌گرداند.
 *
 */
MenuItemId_t MenuEngine_GetSelectedItemId(void);



/******************************************************************************
 *
 * Control API
 *
 ******************************************************************************/

/**
 * @brief Reset Menu Engine
 *
 * بازگشت کامل Navigation به وضعیت اولیه:
 *
 *      Main Page
 *      First Item
 *      Navigation State
 *
 */
void MenuEngine_Reset(void);



#endif /* MENU_ENGINE_H */
