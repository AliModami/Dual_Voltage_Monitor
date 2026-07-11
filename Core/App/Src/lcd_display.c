/**
 ******************************************************************************
 * @file           lcd_display.c
 * @brief          لایه مدیریت نمایش LCD برای پروژه Dual Voltage Monitor
 *
 * @details
 *   این فایل پیاده‌سازی لایه میانی Display است.
 *
 *   وظیفه این لایه:
 *
 *   - مدیریت صفحات نمایش
 *   - مدیریت Refresh LCD
 *   - آماده‌سازی اطلاعات برای نمایش
 *   - ایجاد یک رابط ساده برای Menu و Application
 *
 *
 *   این فایل مستقیماً با سخت‌افزار LCD کار نمی‌کند.
 *
 *   ارتباط سخت‌افزاری توسط:
 *
 *             lcd_i2c.c
 *
 *   انجام می‌شود.
 *
 *
 *   معماری:
 *
 *   ─────────────────────────────────────────
 *
 *       main.c
 *          |
 *          |
 *       menu.c
 *       monitor.c
 *       stream.c
 *
 *          |
 *          v
 *
 *       lcd_display.c
 *
 *          |
 *          v
 *
 *       lcd_i2c.c
 *
 *          |
 *          v
 *
 *       PCF8574 + HD44780 LCD
 *
 *   ─────────────────────────────────────────
 *
 *
 * @design_reason
 *
 *   اگر مستقیماً در تمام فایل‌های پروژه بنویسیم:
 *
 *       LCD_SetCursor()
 *       LCD_Print()
 *
 *   بعداً تغییر LCD یا طراحی منو سخت خواهد شد.
 *
 *
 *   بنابراین یک لایه میانی ایجاد می‌کنیم که فقط مسئول
 *   "نمایش اطلاعات" باشد.
 *
 *
 * @hardware
 *
 *   MCU:
 *       STM32F103C8T6
 *
 *   Display:
 *       LCD Character 20x4
 *
 *   Interface:
 *       I2C + PCF8574
 *
 *
 * @version    1.0.0
 * @date       2026
 * @author     Dual Voltage Monitor Project
 *
 ******************************************************************************
 */


/* ──────────────────────────────────────────────────────────────────────────
 * Dependencies
 *
 * کتابخانه‌های مورد نیاز این فایل
 * ──────────────────────────────────────────────────────────────────────────
 */


#include "lcd_display.h"
#include "lcd_i2c.h"

#include <stdio.h>
#include <string.h>




/* ──────────────────────────────────────────────────────────────────────────
 * Private Constants
 *
 * ثابت‌های داخلی فایل
 *
 * این مقادیر فقط در همین فایل استفاده می‌شوند.
 * بنابراین با static تعریف شده‌اند.
 *
 * ──────────────────────────────────────────────────────────────────────────
 */


/*
 * آدرس پیش‌فرض LCD
 *
 * در صورت نیاز می‌توان بعداً از تنظیمات Config خوانده شود.
 */
#define LCD_DISPLAY_I2C_ADDRESS     LCD_I2C_ADDR_DEFAULT




/*
 * کاراکتر فاصله برای پاک کردن خط LCD
 */
#define LCD_DISPLAY_SPACE           ' '





/* ──────────────────────────────────────────────────────────────────────────
 * Private Variables
 *
 * متغیرهای داخلی این Module
 *
 * فقط lcd_display.c اجازه دسترسی مستقیم دارد.
 *
 * ──────────────────────────────────────────────────────────────────────────
 */


/*
 * Handle اصلی لایه Display
 *
 * تمام وضعیت LCD Display در این ساختار نگهداری می‌شود.
 *
 * مشابه روش HAL:
 *
 *     UART_HandleTypeDef
 *     ADC_HandleTypeDef
 *
 * اینجا هم:
 *
 *     LCD_DisplayHandle_t
 *
 */
static LCD_DisplayHandle_t lcd_display;



/* ──────────────────────────────────────────────────────────────────────────
 * Private Functions Prototype
 *
 * توابع داخلی که فقط در همین فایل استفاده می‌شوند.
 *
 * ──────────────────────────────────────────────────────────────────────────
 */


static void LCD_Display_ClearLine(uint8_t row);





/* ══════════════════════════════════════════════════════════════════════════
 * Initialization
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  راه‌اندازی کامل لایه Display
 *
 * @details
 *
 *   این تابع نقطه شروع استفاده از LCD Display است.
 *
 *
 *   مراحل انجام شده:
 *
 *   1) ذخیره هندل I2C
 *
 *   2) راه‌اندازی Driver پایین‌تر lcd_i2c
 *
 *   3) روشن کردن Backlight
 *
 *   4) پاک کردن LCD
 *
 *   5) آماده‌سازی وضعیت اولیه صفحه
 *
 *
 *   نکته معماری:
 *
 *   این تابع نمی‌داند I2C1 چیست.
 *
 *   فقط یک Handle دریافت می‌کند.
 *
 *
 * @param hi2c
 *
 *      اشاره‌گر به I2C مورد استفاده LCD
 *
 *
 *      مثال:
 *
 *          LCD_Display_Init(&hi2c1);
 *
 *
 */
void LCD_Display_Init(I2C_HandleTypeDef *hi2c)
{
    /*
     * ذخیره I2C Handle
     *
     * بعداً برای توسعه چند LCD یا تغییر I2C
     * بسیار مهم خواهد بود.
     */
    lcd_display.hi2c = hi2c;



    /*
     * راه‌اندازی درایور سخت‌افزاری LCD
     *
     * اینجا فقط از API درایور استفاده می‌کنیم.
     *
     * جزئیات I2C و PCF8574 داخل lcd_i2c.c مخفی است.
     */
    LCD_Init(
            hi2c,
            LCD_DISPLAY_I2C_ADDRESS,
            LCD_DISPLAY_COLS,
            LCD_DISPLAY_ROWS);



    /*
     * روشن کردن نور پس‌زمینه
     */
    LCD_BacklightOn();



    /*
     * پاک کردن صفحه
     */
    LCD_Clear();



    /*
     * مقداردهی اولیه وضعیت Display
     */
    lcd_display.current_page = LCD_PAGE_NONE;



    /*
     * در شروع درخواست Refresh داریم
     */
    lcd_display.refresh_required = 1;



    /*
     * عنوان اولیه خالی است
     */
    memset(
            lcd_display.title,
            0,
            sizeof(lcd_display.title));
}

/* ══════════════════════════════════════════════════════════════════════════
 * Private Utility Functions
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  پاک کردن یک ردیف مشخص از LCD
 *
 * @details
 *
 *   این تابع یک تابع داخلی است و مستقیماً توسط Application
 *   استفاده نمی‌شود.
 *
 *
 *   دلیل ایجاد این تابع:
 *
 *   دستور LCD_Clear() کل صفحه را پاک می‌کند.
 *
 *   اما در صفحات مانیتورینگ، معمولاً فقط یک مقدار تغییر می‌کند.
 *
 *
 *   مثال:
 *
 *       Vin: 220.5V
 *
 *       تبدیل می‌شود به:
 *
 *       Vin: 221.0V
 *
 *
 *   اگر کل صفحه را پاک کنیم:
 *
 *       - Flicker ایجاد می‌شود
 *       - زمان بیشتری مصرف می‌شود
 *
 *
 *   بنابراین فقط همان خط را پاک می‌کنیم.
 *
 *
 * @param row
 *
 *      شماره ردیف LCD
 *
 */
static void LCD_Display_ClearLine(uint8_t row)
{
    uint8_t i;



    /*
     * رفتن به ابتدای خط مورد نظر
     */
    LCD_SetCursor(0, row);



    /*
     * پر کردن تمام ستون‌ها با Space
     *
     * LCD کاراکتر حذف واقعی ندارد.
     *
     * بنابراین برای پاک کردن یک قسمت:
     *
     *     کاراکتر فاصله چاپ می‌کنیم.
     */
    for(i = 0; i < LCD_DISPLAY_COLS; i++)
    {
        LCD_PrintChar(LCD_DISPLAY_SPACE);
    }



    /*
     * برگشت به ابتدای همان خط
     *
     * تا متن بعدی از جای درست نوشته شود.
     */
    LCD_SetCursor(0, row);
}





/* ══════════════════════════════════════════════════════════════════════════
 * Public Display Functions
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  پاک کردن کامل LCD
 *
 * @details
 *
 *   تمام محتوای صفحه پاک می‌شود.
 *
 *   این تابع از LCD_Clear در Driver استفاده می‌کند.
 *
 *
 *   استفاده:
 *
 *       LCD_Display_Clear();
 *
 */
void LCD_Display_Clear(void)
{
    LCD_Clear();



    /*
     * بعد از پاک شدن صفحه،
     * نیاز به Refresh مجدد داریم.
     */
    lcd_display.refresh_required = 1;
}





/**
 * @brief  نمایش عنوان صفحه
 *
 * @details
 *
 *   عنوان معمولاً در ردیف اول LCD نمایش داده می‌شود.
 *
 *
 *   مثال:
 *
 *       +--------------------+
 *       | Voltage Monitor    |
 *       |                    |
 *       | Vin: 220.5V        |
 *       | Vout:219.8V        |
 *       +--------------------+
 *
 *
 *   همچنین عنوان در Handle ذخیره می‌شود
 *   تا در صورت نیاز قابل بازیابی باشد.
 *
 *
 * @param title
 *
 *      متن عنوان
 *
 */
void LCD_Display_ShowTitle(const char *title)
{
    /*
     * جلوگیری از خطای Pointer NULL
     *
     * اگر متن وجود نداشته باشد،
     * کاری انجام نمی‌دهیم.
     */
    if(title == NULL)
    {
        return;
    }



    /*
     * ذخیره عنوان در ساختار داخلی
     *
     * strncpy باعث جلوگیری از Overflow می‌شود.
     */
    strncpy(
            lcd_display.title,
            title,
            LCD_DISPLAY_TITLE_SIZE - 1);



    /*
     * اطمینان از پایان رشته
     *
     * در C باید آخر رشته حتماً '\0' باشد.
     */
    lcd_display.title[LCD_DISPLAY_TITLE_SIZE - 1] = '\0';



    /*
     * پاک کردن خط اول
     */
    LCD_Display_ClearLine(0);



    /*
     * رفتن به ابتدای خط اول
     */
    LCD_SetCursor(0,0);



    /*
     * چاپ عنوان
     */
    LCD_Print(
            lcd_display.title);



    /*
     * اعلام نیاز Refresh
     */
    lcd_display.refresh_required = 1;
}






/**
 * @brief  نمایش متن در یک خط مشخص
 *
 * @details
 *
 *   قبل از نمایش متن، خط پاک می‌شود.
 *
 *   این کار باعث می‌شود اگر متن جدید کوتاه‌تر بود،
 *   کاراکترهای قبلی باقی نمانند.
 *
 *
 *   مثال:
 *
 *   قبل:
 *
 *       Voltage = 220.50
 *
 *
 *   بعد:
 *
 *       Voltage = 9
 *
 *
 *   بدون پاک کردن:
 *
 *       Voltage = 950
 *
 *   خواهد شد که اشتباه است.
 *
 *
 * @param row
 *
 *      شماره خط
 *
 * @param text
 *
 *      متن مورد نمایش
 *
 */
void LCD_Display_PrintLine(
        uint8_t row,
        const char *text)
{
    /*
     * بررسی محدوده خط
     */
    if(row >= LCD_DISPLAY_ROWS)
    {
        return;
    }



    /*
     * بررسی Pointer
     */
    if(text == NULL)
    {
        return;
    }



    /*
     * پاک کردن خط
     */
    LCD_Display_ClearLine(row);



    /*
     * رفتن به ابتدای خط
     */
    LCD_SetCursor(0,row);



    /*
     * نمایش متن
     */
    LCD_Print(text);



    /*
     * درخواست Refresh
     */
    lcd_display.refresh_required = 1;
}

/* ══════════════════════════════════════════════════════════════════════════
 * Floating Point Display
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  نمایش یک مقدار اعشاری همراه با Label
 *
 * @details
 *
 *   این تابع برای نمایش مقادیر اندازه‌گیری شده طراحی شده است.
 *
 *   مثال کاربرد در پروژه:
 *
 *       Vin: 220.5V
 *       Vout:219.8V
 *
 *
 *   به جای اینکه هر Module خودش:
 *
 *       sprintf()
 *       LCD_SetCursor()
 *       LCD_Print()
 *
 *   را انجام دهد، فقط این تابع را فراخوانی می‌کند.
 *
 *
 *   این کار باعث می‌شود:
 *
 *       - کدهای تکراری کمتر شوند
 *       - معماری پروژه تمیزتر شود
 *       - تغییر LCD ساده‌تر شود
 *
 *
 * @param row
 *        شماره ردیف LCD
 *
 * @param label
 *        متن قبل از مقدار
 *
 * @param value
 *        مقدار اعشاری برای نمایش
 *
 * @param decimals
 *        تعداد ارقام اعشار
 *
 */
void LCD_Display_PrintFloat(
        uint8_t row,
        const char *label,
        float value,
        uint8_t decimals)
{
    char buffer[32];


    /*
     * بررسی محدوده ردیف
     */
    if(row >= LCD_DISPLAY_ROWS)
    {
        return;
    }



    /*
     * جلوگیری از استفاده از Pointer نامعتبر
     */
    if(label == NULL)
    {
        return;
    }



    /*
     * ساخت رشته نهایی برای LCD
     *
     * مثال:
     *
     * label = "Vin:"
     * value = 220.5
     *
     * نتیجه:
     *
     * "Vin:220.5"
     *
     */
    snprintf(
            buffer,
            sizeof(buffer),
            "%s%.1f",
            label,
            value);



    /*
     * نمایش در خط مورد نظر
     */
    LCD_Display_PrintLine(
            row,
            buffer);
}






/* ══════════════════════════════════════════════════════════════════════════
 * Page Management
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  تعیین صفحه فعلی LCD
 *
 * @details
 *
 *   این تابع برای هماهنگی با سیستم Menu استفاده خواهد شد.
 *
 *   مثال:
 *
 *       کاربر وارد Monitor Menu می‌شود:
 *
 *       LCD_Display_SetPage(
 *                  LCD_PAGE_MONITOR);
 *
 *
 *   بعداً Menu Manager می‌تواند بداند
 *   در حال حاضر کدام صفحه فعال است.
 *
 *
 * @param page
 *        صفحه جدید
 *
 */
void LCD_Display_SetPage(
        LCD_DisplayPage_t page)
{
    lcd_display.current_page = page;



    /*
     * تغییر صفحه یعنی نیاز به بازسازی نمایش داریم.
     */
    lcd_display.refresh_required = 1;
}






/**
 * @brief  دریافت صفحه فعلی LCD
 *
 * @return
 *        صفحه فعال فعلی
 *
 */
LCD_DisplayPage_t LCD_Display_GetPage(void)
{
    return lcd_display.current_page;
}






/* ══════════════════════════════════════════════════════════════════════════
 * Refresh Management
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  درخواست Refresh نمایش
 *
 * @details
 *
 *   در سیستم‌های Embedded بهتر است LCD دائماً Update نشود.
 *
 *   نوشتن دائم روی LCD:
 *
 *       - CPU را مشغول می‌کند
 *       - Flicker ایجاد می‌کند
 *       - سرعت Menu را کاهش می‌دهد
 *
 *
 *   بنابراین فقط Flag تغییر می‌کند.
 *
 *   بخش اصلی برنامه بعداً تصمیم می‌گیرد چه زمانی Refresh انجام شود.
 *
 */
void LCD_Display_RequestRefresh(void)
{
    lcd_display.refresh_required = 1;
}






/**
 * @brief  بررسی وضعیت Refresh
 *
 * @return
 *
 *       1  → نیاز به Refresh وجود دارد
 *
 *       0  → صفحه فعلی معتبر است
 *
 */
uint8_t LCD_Display_NeedRefresh(void)
{
    return lcd_display.refresh_required;
}






/**
 * @brief  پاک کردن Flag Refresh
 *
 * @details
 *
 *   بعد از اینکه صفحه دوباره نوشته شد،
 *   این تابع فراخوانی می‌شود.
 *
 */
void LCD_Display_ClearRefreshFlag(void)
{
    lcd_display.refresh_required = 0;
}






/* ══════════════════════════════════════════════════════════════════════════
 * Backlight Control
 * ══════════════════════════════════════════════════════════════════════════
 */


/**
 * @brief  روشن کردن نور پس‌زمینه LCD
 *
 */
void LCD_Display_BacklightOn(void)
{
    LCD_BacklightOn();
}






/**
 * @brief  خاموش کردن نور پس‌زمینه LCD
 *
 */
void LCD_Display_BacklightOff(void)
{
    LCD_BacklightOff();
}





/* ══════════════════════════════════════════════════════════════════════════
 * End of File
 * ══════════════════════════════════════════════════════════════════════════
 */
