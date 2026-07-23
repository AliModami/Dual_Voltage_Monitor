/******************************************************************************
 *
 * @file    menu_renderer.h
 *
 * @brief   Menu Renderer Interface For Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * Project:
 *
 *      Dual Voltage Monitor
 *
 *------------------------------------------------------------------------------
 *
 * MCU:
 *
 *      STM32F103C8T6
 *
 *------------------------------------------------------------------------------
 *
 * Framework:
 *
 *      STM32 HAL
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *      This file defines the public interface of the Menu Renderer layer.
 *
 *      The renderer converts Menu Engine state into LCD presentation.
 *
 *
 *      Renderer responsibilities:
 *
 *          - Draw menu pages.
 *          - Display page titles.
 *          - Display visible menu items.
 *          - Display selection cursor.
 *          - Manage refresh requests.
 *
 *
 *      Renderer does NOT:
 *
 *          - Process buttons.
 *          - Navigate menus.
 *          - Modify menu data.
 *          - Access application settings.
 *          - Control LCD hardware directly.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Architecture:
 *
 *
 *              menu_items.c
 *                    |
 *                    v
 *
 *              menu_engine.c
 *                    |
 *                    v
 *
 *             menu_renderer.c
 *                    |
 *                    v
 *
 *             lcd_display.c
 *                    |
 *                    v
 *
 *              lcd_i2c.c
 *
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      1.0.2
 *
 ******************************************************************************/

#ifndef MENU_RENDERER_H
#define MENU_RENDERER_H


/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>


#include "menu_types.h"

#include "menu_engine.h"

#include "lcd_display.h"



#ifdef __cplusplus
extern "C"
{
#endif



/******************************************************************************
 * Renderer Configuration
 ******************************************************************************/

/**
 * @brief
 *      Number of visible menu rows.
 *
 * @details
 *
 *      For 20x4 LCD:
 *
 *          Row 0 : Page title
 *
 *          Row 1-3 : Menu items
 *
 */
#define MENU_RENDERER_VISIBLE_ROWS       (3U)



/**
 * @brief
 *      Selection cursor character.
 */
#define MENU_RENDERER_CURSOR_CHARACTER   '>'




/**
 * @brief
 *      Space character.
 */
#define MENU_RENDERER_SPACE_CHARACTER    ' '




/**
 * @brief
 *      Maximum text buffer size.
 */
#define MENU_RENDERER_TEXT_SIZE          (21U)



/******************************************************************************
 * Renderer State
 ******************************************************************************/

/**
 * @brief
 *      Renderer runtime state.
 */
typedef enum
{

    /**
     * Renderer not initialized.
     */
    MENU_RENDERER_NOT_INITIALIZED = 0U,


    /**
     * Renderer ready.
     */
    MENU_RENDERER_READY,


    /**
     * Renderer error.
     */
    MENU_RENDERER_ERROR


} MenuRendererState_t;




/******************************************************************************
 * Renderer Handle
 ******************************************************************************/

/**
 * @brief
 *      Renderer runtime object.
 *
 * @details
 *
 *      Stores only presentation state.
 *
 *      Navigation ownership belongs to Menu Engine.
 *
 */
typedef struct
{

    /**
     * Current renderer state.
     */
    MenuRendererState_t state;



    /**
     * LCD display abstraction handle.
     */
    LCD_DisplayHandle_t *display;



    /**
     * Last rendered page pointer.
     */
    MenuPage_t *renderedPage;



    /**
     * Last rendered selection index.
     */
    uint16_t renderedSelection;



    /**
     * Complete redraw request flag.
     */
    bool forceRedraw;



} MenuRenderer_t;




/******************************************************************************
 *
 * End Of Part 1/4
 *
 ******************************************************************************/

/******************************************************************************
 * Initialization API
 ******************************************************************************/

/**
 * @brief
 *      Initializes menu renderer.
 *
 * @details
 *
 *      Initialization sequence:
 *
 *          1. Reset renderer internal state.
 *
 *          2. Attach LCD display abstraction.
 *
 *          3. Prepare first redraw request.
 *
 *
 * @param display
 *
 *      Pointer to LCD display handle.
 *
 * @example
 *
 *      MenuRenderer_Init(
 *              LCD_Display_GetHandle());
 *
 ******************************************************************************/

void MenuRenderer_Init(
        LCD_DisplayHandle_t *display);




/**
 * @brief
 *      Returns renderer instance.
 *
 * @return
 *
 *      Pointer to renderer object.
 *
 ******************************************************************************/

MenuRenderer_t *MenuRenderer_GetInstance(void);




/******************************************************************************
 * Rendering API
 ******************************************************************************/

/**
 * @brief
 *      Executes renderer service.
 *
 * @details
 *
 *      This function should be called periodically.
 *
 *      Example:
 *
 *          while(1)
 *          {
 *
 *              MenuEngine_Service();
 *
 *              MenuRenderer_Update();
 *
 *          }
 *
 ******************************************************************************/

void MenuRenderer_Update(void);




/**
 * @brief
 *      Performs complete rendering.
 *
 * @details
 *
 *      Renders:
 *
 *          - Page title
 *          - Menu items
 *          - Selection cursor
 *
 ******************************************************************************/

void MenuRenderer_Render(void);




/**
 * @brief
 *      Renders current menu page.
 *
 * @details
 *
 *      Draws:
 *
 *          - Page title
 *          - Visible item rows
 *
 ******************************************************************************/

void MenuRenderer_RenderPage(void);




/**
 * @brief
 *      Updates selection cursor display.
 *
 * @details
 *
 *      Updates only visible cursor state.
 *
 *      Menu navigation remains owned by Menu Engine.
 *
 ******************************************************************************/

void MenuRenderer_RenderSelection(void);




/******************************************************************************
 * Refresh Management API
 ******************************************************************************/

/**
 * @brief
 *      Requests complete redraw.
 *
 * @details
 *
 *      Forces renderer to update:
 *
 *          - Title
 *          - Items
 *          - Cursor
 *
 ******************************************************************************/

void MenuRenderer_RequestFullRedraw(void);




/**
 * @brief
 *      Clears redraw request flag.
 *
 ******************************************************************************/

void MenuRenderer_ClearRedrawRequest(void);




/**
 * @brief
 *      Checks redraw request status.
 *
 * @return
 *
 *      true:
 *          Full redraw requested.
 *
 ******************************************************************************/

bool MenuRenderer_NeedRedraw(void);




/******************************************************************************
 * Information API
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer current state.
 *
 * @return
 *
 *      Renderer state.
 *
 ******************************************************************************/

MenuRendererState_t MenuRenderer_GetState(void);




/**
 * @brief
 *      Checks renderer initialization status.
 *
 * @return
 *
 *      true:
 *          Renderer ready.
 *
 ******************************************************************************/

bool MenuRenderer_IsInitialized(void);




/**
 * @brief
 *      Validates renderer object.
 *
 * @return
 *
 *      true:
 *          Renderer object is valid.
 *
 ******************************************************************************/

bool MenuRenderer_IsValid(void);




/******************************************************************************
 *
 * End Of Part 2/4
 *
 ******************************************************************************/

/******************************************************************************
 * Display Formatting API
 ******************************************************************************/

/**
 * @brief
 *      Converts menu item information into display text.
 *
 * @details
 *
 *      This function prepares text only.
 *
 *      It does not access LCD hardware.
 *
 *
 * @param item
 *      Pointer to menu item.
 *
 *
 * @param buffer
 *      Output character buffer.
 *
 *
 * @param size
 *      Buffer size.
 *
 ******************************************************************************/

void MenuRenderer_FormatItemText(
        MenuItem_t *item,
        char *buffer,
        uint16_t size);




/**
 * @brief
 *      Formats current page title.
 *
 * @details
 *
 *      Converts MenuPage title into display text.
 *
 *
 * @param page
 *      Pointer to current menu page.
 *
 *
 * @param buffer
 *      Output character buffer.
 *
 *
 * @param size
 *      Buffer size.
 *
 ******************************************************************************/

void MenuRenderer_FormatPageTitle(
        MenuPage_t *page,
        char *buffer,
        uint16_t size);




/******************************************************************************
 * Renderer Control API
 ******************************************************************************/

/**
 * @brief
 *      Resets renderer presentation state.
 *
 * @details
 *
 *      This function resets only renderer internal state.
 *
 *      Menu Engine state is not modified.
 *
 ******************************************************************************/

void MenuRenderer_Reset(void);




/**
 * @brief
 *      Deinitializes renderer module.
 *
 * @details
 *
 *      Clears renderer state.
 *
 *      Dynamic memory is not used.
 *
 ******************************************************************************/

void MenuRenderer_DeInit(void);




/******************************************************************************
 * Internal Design Rules
 ******************************************************************************/

/*

Renderer Ownership Model:



        +--------------------------------+
        |          Menu Engine           |
        |                                |
        |  Navigation                    |
        |  Selection                     |
        |  Events                        |
        +---------------+----------------+
                        |
                        |
                        v


        +--------------------------------+
        |        Menu Renderer           |
        |                                |
        |  Page Rendering                |
        |  Cursor Rendering              |
        |  Text Formatting               |
        +---------------+----------------+
                        |
                        |
                        v


        +--------------------------------+
        |        LCD Display             |
        |                                |
        |  Line Output                   |
        |  Display Management            |
        +--------------------------------+



Renderer Rules:


1.

Renderer reads Menu Engine state only.


2.

Renderer never modifies:

    - Menu navigation
    - Menu selection
    - Menu data


3.

Renderer never accesses:

    - Buttons
    - ADC
    - UART
    - Flash
    - Application settings


4.

Renderer uses LCD Display abstraction only.


5.

No dynamic memory allocation is allowed.


6.

Rendering architecture:

    Page Based Rendering

    No scrolling support.



*/




/******************************************************************************
 *
 * End Of Part 3/4
 *
 ******************************************************************************/

/******************************************************************************
 * Version Information
 ******************************************************************************/

/**
 * @brief
 *      Menu Renderer header version.
 *
 ******************************************************************************/

#define MENU_RENDERER_VERSION_MAJOR      (1U)

#define MENU_RENDERER_VERSION_MINOR      (0U)

#define MENU_RENDERER_VERSION_PATCH      (2U)


#define MENU_RENDERER_VERSION_STRING     "1.0.2"




/******************************************************************************
 * C++ Compatibility
 ******************************************************************************/

#ifdef __cplusplus
}
#endif



#endif /* MENU_RENDERER_H */




/******************************************************************************
 *
 * Revision History
 *
 ******************************************************************************

Version 1.0.2

Date:
2026-07-22


Changes:


    - Header synchronized with:

          menu_renderer.c v1.0.2



    - Added missing API declarations:

          MenuRenderer_Reset()

          MenuRenderer_DeInit()



    - Removed unsupported API dependency.


    - Preserved Page Based rendering architecture.


    - Removed all scrolling references.


    - Verified compatibility with:

          menu_types.h       v2.0.0

          menu_engine.h      v2.1.0

          lcd_display.h     v1.0.0



    - Preserved static allocation design.


    - Renderer remains presentation-only layer.



******************************************************************************/




/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
