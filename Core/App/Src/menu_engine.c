/******************************************************************************
 *
 * @file    menu_engine.c
 *
 * @brief   Menu Navigation Engine Implementation
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
 * ============================================================================
 *
 * این فایل هسته منطقی سیستم Menu را پیاده‌سازی می‌کند.
 *
 * مسئولیت این ماژول:
 *
 *      - مدیریت صفحه جاری
 *      - مدیریت Item انتخاب شده
 *      - پردازش Event های منو
 *      - Navigation بین Page ها
 *      - اجرای Callback های مربوط به Action ها
 *
 *
 * این فایل هیچ شناختی از:
 *
 *      LCD
 *      GPIO
 *      Button Driver
 *      UART
 *      ADC
 *
 * ندارد.
 *
 *
 * معماری:
 *
 *
 *                  Button Application
 *                         |
 *                         |
 *                    Menu Event
 *                         |
 *                         v
 *
 *                 +---------------+
 *                 | menu_engine.c |
 *                 +---------------+
 *
 *                         |
 *                         |
 *              +----------+----------+
 *              |                     |
 *              v                     v
 *
 *        menu_items.c          menu_renderer.c
 *
 *        Database              Display
 *
 *
 *------------------------------------------------------------------------------
 *
 * Design Rules
 * ============================================================================
 *
 * 1- Menu Engine مالک MenuItem و MenuPage نیست.
 *
 * 2- تمام Item ها در menu_items.c به صورت Static ساخته می‌شوند.
 *
 * 3- Engine فقط Pointer آن‌ها را مدیریت می‌کند.
 *
 * 4- Renderer فقط وضعیت Engine را می‌خواند.
 *
 * 5- Engine هیچ دسترسی مستقیم به سخت‌افزار ندارد.
 *
 *
 *------------------------------------------------------------------------------
 *
 * Author :
 *
 *      Ali Modami & OpenAI
 *
 *------------------------------------------------------------------------------
 *
 * Version :
 *
 *      2.0.0
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "menu_engine.h"

#include "menu_items.h"

#include <stddef.h>
#include <stdbool.h>



/******************************************************************************
 * Private Variables
 ******************************************************************************/

/*
 * تنها نمونه Runtime از Menu Engine
 *
 * این ساختار وضعیت فعلی Navigation را نگهداری می‌کند.
 *
 * توجه:
 *
 * این متغیر مالک Database نیست.
 */
static MenuEngine_t g_menuEngine;



/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/*
 * حرکت به Item بعدی
 */
static void MenuEngine_SelectNext(void);



/*
 * حرکت به Item قبلی
 */
static void MenuEngine_SelectPrevious(void);



/*
 * اجرای Item انتخاب شده
 */
static void MenuEngine_EnterItem(void);



/*
 * بازگشت به Page والد
 */
static void MenuEngine_Back(void);



/******************************************************************************
 *
 * Public Functions
 *
 ******************************************************************************/


/******************************************************************************
 *
 * MenuEngine_Init()
 *
 ******************************************************************************/

/**
 * @brief
 *      Initialize Menu Engine.
 *
 * @details
 *
 *      این تابع باید بعد از:
 *
 *          Menu_ItemsInit()
 *
 *      فراخوانی شود.
 *
 *
 *      ترتیب صحیح:
 *
 *          Menu_ItemsInit();
 *
 *          MenuEngine_Init();
 *
 *
 ******************************************************************************/

void MenuEngine_Init(void)
{

    /*
     * پاک کردن وضعیت قبلی Engine
     */
    g_menuEngine.currentPage =
            NULL;


    g_menuEngine.currentItem =
            NULL;


    g_menuEngine.state =
            MENU_STATE_IDLE;


    g_menuEngine.pageChanged =
            false;


    g_menuEngine.selectionChanged =
            false;


    g_menuEngine.editMode =
            false;



    /*
     * دریافت Root Page از Database
     */
    g_menuEngine.currentPage =
            Menu_GetMainPage();



    if(g_menuEngine.currentPage != NULL)
    {

        /*
         * انتخاب اولین Item صفحه اصلی
         */
        g_menuEngine.currentItem =
                g_menuEngine.currentPage->firstItem;



        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentItem;



        /*
         * درخواست Refresh اولیه LCD
         */
        g_menuEngine.pageChanged =
                true;

    }

}



/******************************************************************************
 *
 * MenuEngine_ProcessEvent()
 *
 ******************************************************************************/

/**
 * @brief
 *      Process external menu event.
 *
 * @param event
 *      Menu event generated by Application layer.
 *
 *
 * @details
 *
 *      این تابع تنها ورودی خارجی Engine است.
 *
 *      Engine فقط Event های منطقی دریافت می‌کند:
 *
 *          MENU_EVENT_UP
 *          MENU_EVENT_DOWN
 *          MENU_EVENT_ENTER
 *          MENU_EVENT_BACK
 *
 ******************************************************************************/

void MenuEngine_ProcessEvent(MenuEvent_t event)
{

    switch(event)
    {


        case MENU_EVENT_UP:


            MenuEngine_SelectPrevious();


            break;



        case MENU_EVENT_DOWN:


            MenuEngine_SelectNext();


            break;



        case MENU_EVENT_ENTER:


            MenuEngine_EnterItem();


            break;



        case MENU_EVENT_BACK:


            MenuEngine_Back();


            break;



        default:


            break;

    }

}


/******************************************************************************
*
* MenuEngine_SelectNext()
*
******************************************************************************/

/**
* @brief
*      Move selection cursor to next item.
*
* @details
*
*      این تابع فقط Navigation را انجام می‌دهد.
*
*      هیچ تغییری در:
*
*          LCD
*          Action
*          Setting Value
*
* ایجاد نمی‌کند.
*
*
*      رفتار Navigation:
*
*          Item آخر
*              |
*              v
*          توقف
*
*
*      Circular Navigation وجود ندارد.
*
******************************************************************************/

static void MenuEngine_SelectNext(void)
{

   MenuItem_t *currentItem;



   /*
    * بررسی وجود Page فعال
    */
   if(g_menuEngine.currentPage == NULL)
   {
       return;
   }



   /*
    * دریافت Item انتخاب شده فعلی
    */
   currentItem =
           g_menuEngine.currentPage->selectedItem;



   if(currentItem == NULL)
   {
       return;
   }



   /*
    * اگر Item بعدی وجود دارد،
    * Cursor حرکت می‌کند.
    */
   if(currentItem->next != NULL)
   {

       g_menuEngine.currentPage->selectedItem =
               currentItem->next;



       g_menuEngine.currentItem =
               currentItem->next;



       /*
        * فقط مکان‌نما تغییر کرده است.
        */
       g_menuEngine.selectionChanged =
               true;

   }

}



/******************************************************************************
*
* MenuEngine_SelectPrevious()
*
******************************************************************************/

/**
* @brief
*      Move selection cursor to previous item.
*
******************************************************************************/

static void MenuEngine_SelectPrevious(void)
{

   MenuItem_t *currentItem;



   if(g_menuEngine.currentPage == NULL)
   {
       return;
   }



   currentItem =
           g_menuEngine.currentPage->selectedItem;



   if(currentItem == NULL)
   {
       return;
   }



   /*
    * اگر Item قبلی وجود داشته باشد،
    * حرکت انجام می‌شود.
    */
   if(currentItem->previous != NULL)
   {

       g_menuEngine.currentPage->selectedItem =
               currentItem->previous;



       g_menuEngine.currentItem =
               currentItem->previous;



       g_menuEngine.selectionChanged =
               true;

   }

}



/******************************************************************************
*
* MenuEngine_EnterItem()
*
******************************************************************************/

/**
* @brief
*      Execute selected menu item.
*
* @details
*
*      رفتار بر اساس نوع Item تعیین می‌شود.
*
*
*      MENU_ITEM_SUBMENU
*
*          ورود به صفحه زیرمجموعه
*
*
*      MENU_ITEM_ACTION
*
*          اجرای Callback
*
*
*      MENU_ITEM_VALUE
*
*          ورود به حالت Edit
*
******************************************************************************/

static void MenuEngine_EnterItem(void)
{

   MenuItem_t *item;



   /*
    * دریافت Page فعال
    */
   if(g_menuEngine.currentPage == NULL)
   {
       return;
   }



   /*
    * دریافت Item انتخاب شده
    */
   item =
       g_menuEngine.currentPage->selectedItem;



   if(item == NULL)
   {
       return;
   }



   switch(item->type)
   {


       /**************************************************************
        *
        * Sub Menu
        *
        **************************************************************/

       case MENU_ITEM_SUBMENU:


           /*
            * بررسی وجود Page مقصد
            */
           if(item->childPage != NULL)
           {

               /*
                * تغییر Page فعال
                */
               g_menuEngine.currentPage =
                       item->childPage;



               /*
                * انتخاب اولین Item صفحه جدید
                */
               g_menuEngine.currentPage->selectedItem =
                       g_menuEngine.currentPage->firstItem;



               g_menuEngine.currentItem =
                       g_menuEngine.currentPage->selectedItem;



               /*
                * درخواست Refresh کامل
                */
               g_menuEngine.pageChanged =
                       true;

           }


           break;



       /**************************************************************
        *
        * Action Item
        *
        **************************************************************/

       case MENU_ITEM_ACTION:


           /*
            * اجرای Callback مربوط به Item
            */
           if(item->enterCallback != NULL)
           {

               item->enterCallback(item);

           }


           break;



       /**************************************************************
        *
        * Value Item
        *
        **************************************************************/

       case MENU_ITEM_VALUE:


           /*
            * ورود به حالت ویرایش
            *
            * مدیریت مقدار واقعی
            * در Settings Engine انجام خواهد شد.
            */
           g_menuEngine.state =
                   MENU_STATE_EDIT;



           g_menuEngine.editMode =
                   true;


           break;



       default:


           break;

   }

}



/******************************************************************************
*
* MenuEngine_Back()
*
******************************************************************************/

/**
* @brief
*      Return to parent page.
*
* @details
*
*      این تابع فقط Navigation را انجام می‌دهد.
*
*
*      اگر Page جاری Root باشد:
*
*          هیچ اتفاقی نمی‌افتد.
*
*
*      اگر Parent Page وجود داشته باشد:
*
*          Page قبلی فعال می‌شود.
*
******************************************************************************/

static void MenuEngine_Back(void)
{

   MenuPage_t *parentPage;



   /*
    * بررسی Page جاری
    */
   if(g_menuEngine.currentPage == NULL)
   {
       return;
   }



   /*
    * دریافت Parent
    */
   parentPage =
           g_menuEngine.currentPage->parentPage;



   /*
    * اگر Parent وجود نداشته باشد،
    * یعنی در Root Menu هستیم.
    */
   if(parentPage == NULL)
   {
       return;
   }



   /*
    * بازگشت به Page والد
    */
   g_menuEngine.currentPage =
           parentPage;



   /*
    * انتخاب Item مربوط به این SubMenu
    *
    * توجه:
    *
    * در طراحی فعلی، Page والد قبلاً
    * selectedItem خود را حفظ می‌کند.
    */
   g_menuEngine.currentItem =
           g_menuEngine.currentPage->selectedItem;



   /*
    * درخواست Refresh کامل صفحه
    */
   g_menuEngine.pageChanged =
           true;



   /*
    * خروج از حالت Edit
    */
   g_menuEngine.editMode =
           false;



   g_menuEngine.state =
           MENU_STATE_NAVIGATION;

}



/******************************************************************************
*
* MenuEngine_GetCurrentPage()
*
******************************************************************************/

/**
* @brief
*      Get active menu page.
*
* @return
*      Pointer to current MenuPage.
*
*
* @note
*      Renderer از این تابع برای دریافت اطلاعات
*      صفحه جاری استفاده می‌کند.
*
******************************************************************************/

MenuPage_t *MenuEngine_GetCurrentPage(void)
{

   return g_menuEngine.currentPage;

}



/******************************************************************************
*
* MenuEngine_GetSelectedItem()
*
******************************************************************************/

/**
* @brief
*      Get currently selected menu item.
*
* @return
*      Pointer to selected MenuItem.
*
******************************************************************************/

MenuItem_t *MenuEngine_GetSelectedItem(void)
{

   return g_menuEngine.currentItem;

}



/******************************************************************************
*
* MenuEngine_IsPageChanged()
*
******************************************************************************/

/**
* @brief
*      Check full page refresh request.
*
* @return
*
*      true:
*          Page changed.
*
*      false:
*          No page change.
*
******************************************************************************/

bool MenuEngine_IsPageChanged(void)
{

   return g_menuEngine.pageChanged;

}



/******************************************************************************
*
* MenuEngine_ClearPageChanged()
*
******************************************************************************/

/**
* @brief
*      Clear page refresh flag.
*
******************************************************************************/

void MenuEngine_ClearPageChanged(void)
{

   g_menuEngine.pageChanged =
           false;

}



/******************************************************************************
*
* MenuEngine_IsSelectionChanged()
*
******************************************************************************/

/**
* @brief
*      Check cursor movement flag.
*
* @return
*
*      true:
*          Only selection changed.
*
******************************************************************************/

bool MenuEngine_IsSelectionChanged(void)
{

   return g_menuEngine.selectionChanged;

}



/******************************************************************************
*
* MenuEngine_ClearSelectionChanged()
*
******************************************************************************/

/**
* @brief
*      Clear cursor movement flag.
*
******************************************************************************/

void MenuEngine_ClearSelectionChanged(void)
{

   g_menuEngine.selectionChanged =
           false;

}


/******************************************************************************
 *
 * MenuEngine_GetState()
 *
 ******************************************************************************/

/**
 * @brief
 *      Get current state of Menu Engine.
 *
 * @return
 *      Current MenuState_t value.
 *
 ******************************************************************************/

MenuState_t MenuEngine_GetState(void)
{

    return g_menuEngine.state;

}



/******************************************************************************
 *
 * MenuEngine_SetState()
 *
 ******************************************************************************/

/**
 * @brief
 *      Change Menu Engine state.
 *
 * @param state
 *      New engine state.
 *
 *
 * @details
 *
 *      این تابع برای تغییر State از بیرون Engine
 *      استفاده می‌شود.
 *
 ******************************************************************************/

void MenuEngine_SetState(MenuState_t state)
{

    g_menuEngine.state =
            state;

}



/******************************************************************************
 *
 * MenuEngine_IsEditMode()
 *
 ******************************************************************************/

/**
 * @brief
 *      Check edit mode status.
 *
 * @return
 *
 *      true:
 *          Engine is editing a value.
 *
 *      false:
 *          Normal navigation mode.
 *
 ******************************************************************************/

bool MenuEngine_IsEditMode(void)
{

    return g_menuEngine.editMode;

}



/******************************************************************************
 *
 * MenuEngine_EnableEditMode()
 *
 ******************************************************************************/

/**
 * @brief
 *      Enable value edit mode.
 *
 ******************************************************************************/

void MenuEngine_EnableEditMode(void)
{

    g_menuEngine.editMode =
            true;


    g_menuEngine.state =
            MENU_STATE_EDIT;

}



/******************************************************************************
 *
 * MenuEngine_DisableEditMode()
 *
 ******************************************************************************/

/**
 * @brief
 *      Disable value edit mode.
 *
 ******************************************************************************/

void MenuEngine_DisableEditMode(void)
{

    g_menuEngine.editMode =
            false;


    g_menuEngine.state =
            MENU_STATE_NAVIGATION;

}



/******************************************************************************
 *
 * MenuEngine_SetMessageState()
 *
 ******************************************************************************/

/**
 * @brief
 *      Enter temporary message state.
 *
 * @details
 *
 *      این حالت برای نمایش پیام‌های کوتاه استفاده می‌شود:
 *
 *          Saved
 *          Error
 *          Completed
 *
 ******************************************************************************/

void MenuEngine_SetMessageState(void)
{

    g_menuEngine.state =
            MENU_STATE_MESSAGE;

}



/******************************************************************************
 *
 * MenuEngine_Reset()
 *
 ******************************************************************************/

/**
 * @brief
 *      Reset engine navigation state.
 *
 * @details
 *
 *      بعد از Reset:
 *
 *          - Root Menu فعال می‌شود.
 *          - اولین Item انتخاب می‌شود.
 *
 ******************************************************************************/

void MenuEngine_Reset(void)
{

    /*
     * دریافت Root Page
     */
    g_menuEngine.currentPage =
            Menu_GetMainPage();



    if(g_menuEngine.currentPage != NULL)
    {

        /*
         * انتخاب اولین Item
         */
        g_menuEngine.currentItem =
                g_menuEngine.currentPage->firstItem;



        g_menuEngine.currentPage->selectedItem =
                g_menuEngine.currentItem;

    }



    /*
     * بازگشت به حالت Navigation
     */
    g_menuEngine.state =
            MENU_STATE_NAVIGATION;



    g_menuEngine.editMode =
            false;



    /*
     * درخواست Refresh کامل
     */
    g_menuEngine.pageChanged =
            true;



    g_menuEngine.selectionChanged =
            false;

}



/******************************************************************************
 *
 * MenuEngine_GetItemCount()
 *
 ******************************************************************************/

/**
 * @brief
 *      Get number of items in current page.
 *
 * @return
 *      Number of Menu Items.
 *
 ******************************************************************************/

uint8_t MenuEngine_GetItemCount(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return 0;
    }



    return g_menuEngine.currentPage->itemCount;

}



/******************************************************************************
 *
 * MenuEngine_IsRootPage()
 *
 ******************************************************************************/

/**
 * @brief
 *      Check if current page is root menu.
 *
 * @return
 *      true if current page has no parent.
 *
 ******************************************************************************/

bool MenuEngine_IsRootPage(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return true;
    }



    return
        (g_menuEngine.currentPage->parentPage == NULL);

}


/******************************************************************************
 *
 * MenuEngine_GetCurrentItemId()
 *
 ******************************************************************************/

/**
 * @brief
 *      Get ID of currently selected item.
 *
 * @return
 *      MenuItemId_t of selected item.
 *
 * @details
 *
 *      این تابع برای Application Layer و Debug
 *      استفاده می‌شود.
 *
 ******************************************************************************/

MenuItemId_t MenuEngine_GetCurrentItemId(void)
{

    if(g_menuEngine.currentItem == NULL)
    {
        return MENU_ITEM_ID_COUNT;
    }



    return g_menuEngine.currentItem->id;

}



/******************************************************************************
 *
 * MenuEngine_GetCurrentPageId()
 *
 ******************************************************************************/

/**
 * @brief
 *      Get ID of current page.
 *
 * @return
 *      MenuPageId_t of active page.
 *
 ******************************************************************************/

MenuPageId_t MenuEngine_GetCurrentPageId(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return MENU_PAGE_MAIN;
    }



    return g_menuEngine.currentPage->id;

}



/******************************************************************************
 *
 * MenuEngine_RequestRefresh()
 *
 ******************************************************************************/

/**
 * @brief
 *      Request complete menu refresh.
 *
 * @details
 *
 *      Renderer در اجرای بعدی باید
 *      کل صفحه را دوباره رسم کند.
 *
 ******************************************************************************/

void MenuEngine_RequestRefresh(void)
{

    g_menuEngine.pageChanged =
            true;

}



/******************************************************************************
 *
 * MenuEngine_RequestSelectionRefresh()
 *
 ******************************************************************************/

/**
 * @brief
 *      Request cursor refresh only.
 *
 * @details
 *
 *      برای زمانی استفاده می‌شود که فقط
 *      Item انتخاب شده تغییر کرده است.
 *
 ******************************************************************************/

void MenuEngine_RequestSelectionRefresh(void)
{

    g_menuEngine.selectionChanged =
            true;

}



/******************************************************************************
 *
 * MenuEngine_UpdateCurrentItem()
 *
 ******************************************************************************/

/**
 * @brief
 *      Synchronize current item pointer.
 *
 * @details
 *
 *      در معماری فعلی Page مالک selectedItem است.
 *
 *      این تابع باعث می‌شود Pointer داخلی Engine
 *      با Page هماهنگ بماند.
 *
 ******************************************************************************/

void MenuEngine_UpdateCurrentItem(void)
{

    if(g_menuEngine.currentPage == NULL)
    {
        return;
    }



    g_menuEngine.currentItem =
            g_menuEngine.currentPage->selectedItem;

}



/******************************************************************************
 *
 * MenuEngine_GetHandle()
 *
 ******************************************************************************/

/**
 * @brief
 *      Return Menu Engine object.
 *
 * @return
 *      Pointer to engine instance.
 *
 * @note
 *
 *      این تابع برای ماژول‌هایی که نیاز به مشاهده
 *      وضعیت Engine دارند استفاده می‌شود.
 *
 ******************************************************************************/

MenuEngine_t *MenuEngine_GetHandle(void)
{

    return &g_menuEngine;

}



/******************************************************************************
 *
 * End Of File
 *
 ******************************************************************************/
