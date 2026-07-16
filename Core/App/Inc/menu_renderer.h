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
 *      This file defines the public interface of the menu renderer layer.
 *
 *      The renderer is responsible for converting menu engine state into
 *      visible LCD content.
 *
 *
 *      The renderer is responsible for:
 *
 *          - Drawing menu pages
 *          - Displaying item titles
 *          - Displaying selection cursor
 *          - Managing page refresh
 *          - Managing cursor refresh
 *
 *
 *      The renderer does NOT:
 *
 *          - Process buttons
 *          - Navigate menus
 *          - Modify menu values
 *          - Access application settings
 *          - Control LCD hardware directly
 *
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
 * Design Rules:
 *
 *      1.
 *      Renderer observes Engine state only.
 *
 *
 *      2.
 *      Renderer never changes navigation state.
 *
 *
 *      3.
 *      Renderer uses LCD abstraction only.
 *
 *
 *      4.
 *      Refresh decisions are controlled by Engine flags.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Dependencies:
 *
 *      menu_types.h
 *
 *      menu_engine.h
 *
 *      lcd_display.h
 *
 *------------------------------------------------------------------------------
 *
 * Author:
 *
 *      Ali Modami
 *
 *------------------------------------------------------------------------------
 *
 * Version:
 *
 *      1.0.0
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
 *      Maximum number of menu rows displayed.
 *
 * @details
 *
 *      The project uses 20x4 LCD.
 *
 *      First row:
 *
 *          Page title
 *
 *      Remaining rows:
 *
 *          Menu items
 *
 */
#define MENU_RENDERER_VISIBLE_ROWS       (3U)



/**
 * @brief
 *      Cursor character.
 *
 */
#define MENU_RENDERER_CURSOR_CHARACTER   '>'




/**
 * @brief
 *      Empty menu line character.
 *
 */
#define MENU_RENDERER_SPACE_CHARACTER    ' '




/**
 * @brief
 *      Maximum menu item text length.
 *
 */
#define MENU_RENDERER_TEXT_SIZE          (21U)




/******************************************************************************
 * Renderer State
 ******************************************************************************/

/**
 * @brief
 *      Renderer initialization state.
 *
 */
typedef enum
{

    /**
     * Renderer is not initialized.
     */
    MENU_RENDERER_NOT_INITIALIZED = 0U,


    /**
     * Renderer is ready.
     */
    MENU_RENDERER_READY,


    /**
     * Renderer error state.
     */
    MENU_RENDERER_ERROR


} MenuRendererState_t;





/******************************************************************************
 * Renderer Handle
 ******************************************************************************/

/**
 * @brief
 *      Menu renderer runtime object.
 *
 * @details
 *
 *      Stores renderer internal state.
 *
 *      The menu engine owns navigation state.
 *
 *      The renderer owns only presentation state.
 *
 */
typedef struct
{

    /**
     * Current renderer state.
     */
    MenuRendererState_t state;



    /**
     * Pointer to LCD display abstraction.
     *
     * Renderer writes output through this interface.
     */
    LCD_DisplayHandle_t *display;



    /**
     * Current rendered page.
     *
     * Used to detect page changes.
     */
    MenuPage_t *renderedPage;



    /**
     * Last rendered selection index.
     *
     * Used for cursor-only refresh.
     */
    uint16_t renderedSelection;



    /**
     * Complete redraw request flag.
     */
    bool forceRedraw;



} MenuRenderer_t;




/******************************************************************************
 * Initialization
 ******************************************************************************/

/**
 * @brief
 *      Initializes menu renderer.
 *
 * @details
 *
 *      Initialization sequence:
 *
 *          1.
 *          Reset renderer state.
 *
 *          2.
 *          Connect LCD display layer.
 *
 *          3.
 *          Prepare first redraw.
 *
 *
 * @param display
 *
 *      Pointer to LCD display abstraction.
 *
 */
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
 */
MenuRenderer_t *MenuRenderer_GetInstance(void);



/******************************************************************************
 * Rendering Service
 ******************************************************************************/

/**
 * @brief
 *      Executes renderer service.
 *
 * @details
 *
 *      This function should be called periodically.
 *
 *      It checks:
 *
 *          - Page refresh request
 *          - Selection refresh request
 *
 *      and updates LCD when required.
 *
 */
void MenuRenderer_Update(void);



/**
 * @brief
 *      Performs complete rendering.
 *
 * @details
 *
 *      Redraws:
 *
 *          - Page title
 *          - Visible menu items
 *          - Selection cursor
 *
 */
void MenuRenderer_Render(void);



/**
 * @brief
 *      Renders current menu page.
 *
 */
void MenuRenderer_RenderPage(void);



/**
 * @brief
 *      Updates only selection cursor.
 *
 * @details
 *
 *      Used when selection changes without page change.
 *
 */
void MenuRenderer_RenderSelection(void);

/******************************************************************************
 * Refresh Management
 ******************************************************************************/

/**
 * @brief
 *      Requests complete renderer redraw.
 *
 * @details
 *
 *      Forces renderer to redraw:
 *
 *          - Page title
 *          - Menu items
 *          - Cursor
 *
 */
void MenuRenderer_RequestFullRedraw(void);




/**
 * @brief
 *      Clears renderer redraw request.
 *
 */
void MenuRenderer_ClearRedrawRequest(void);




/**
 * @brief
 *      Checks if renderer requires complete redraw.
 *
 * @return
 *
 *      true:
 *          Redraw required.
 *
 */
bool MenuRenderer_NeedRedraw(void);




/******************************************************************************
 * Information Access
 ******************************************************************************/

/**
 * @brief
 *      Returns renderer state.
 *
 * @return
 *
 *      Current renderer state.
 *
 */
MenuRendererState_t MenuRenderer_GetState(void);




/**
 * @brief
 *      Checks renderer initialization state.
 *
 * @return
 *
 *      true:
 *          Renderer initialized.
 *
 */
bool MenuRenderer_IsInitialized(void);




/******************************************************************************
 * Display Helpers
 ******************************************************************************/

/**
 * @brief
 *      Converts menu item into display text.
 *
 * @details
 *
 *      This function only prepares visible text.
 *
 *      It does not write to LCD.
 *
 *
 * @param item
 *
 *      Pointer to menu item.
 *
 *
 * @param buffer
 *
 *      Output text buffer.
 *
 *
 * @param size
 *
 *      Buffer size.
 *
 */
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
 *      The title is generated from MenuPage_t.
 *
 *
 * @param page
 *
 *      Current menu page.
 *
 *
 * @param buffer
 *
 *      Output title buffer.
 *
 *
 * @param size
 *
 *      Buffer size.
 *
 */
void MenuRenderer_FormatPageTitle(
        MenuPage_t *page,
        char *buffer,
        uint16_t size);




/******************************************************************************
 * Validation
 ******************************************************************************/

/**
 * @brief
 *      Checks renderer object validity.
 *
 * @return
 *
 *      true:
 *          Renderer object is valid.
 *
 */
bool MenuRenderer_IsValid(void);




/******************************************************************************
 * Internal Design Notes
 ******************************************************************************/

/*

Menu Renderer Ownership Model:



        +--------------------------------+
        |        Menu Engine             |
        |                                |
        | Navigation                     |
        | Selection                      |
        | Events                         |
        +---------------+----------------+
                        |
                        |
                        v


        +--------------------------------+
        |        Menu Renderer           |
        |                                |
        | Page Drawing                   |
        | Cursor Drawing                 |
        | LCD Formatting                 |
        +---------------+----------------+
                        |
                        |
                        v


        +--------------------------------+
        |        LCD Display             |
        |                                |
        | Character Output               |
        | Line Management                |
        +--------------------------------+



Renderer Rules:



1.

Renderer reads Engine state.

Renderer never modifies Engine state.



2.

Renderer never accesses:

    - Buttons
    - ADC
    - UART
    - Flash
    - Application settings



3.

Renderer uses LCD_Display API only.



4.

Renderer supports partial refresh:

    Page refresh:

        - Title
        - Items


    Selection refresh:

        - Cursor only



5.

No dynamic memory allocation is allowed.



*/





/******************************************************************************
 * Version Information
 ******************************************************************************/

/**
 * @brief
 *      Menu Renderer header version.
 *
 */
#define MENU_RENDERER_VERSION_MAJOR      (1U)

#define MENU_RENDERER_VERSION_MINOR      (0U)

#define MENU_RENDERER_VERSION_PATCH      (0U)



#define MENU_RENDERER_VERSION_STRING     "1.0.0"





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

Version 1.0.0

Date:
2026-07-16


Changes:


    - Initial creation of menu renderer interface.


    - Added separation between:

          Menu Engine

          LCD Display Layer



    - Added page rendering API.


    - Added selection rendering API.


    - Added refresh synchronization support.


    - Preserved static allocation architecture.



******************************************************************************/

/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
