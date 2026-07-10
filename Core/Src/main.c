/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  *
  * Dual Voltage Monitor
  *
  * This file contains:
  *
  * - STM32 initialization
  * - Peripheral initialization
  * - Application startup sequence
  * - Main application loop
  *
  * Current test purpose:
  *
  * Verify complete button path:
  *
  * GPIO
  *   |
  *   v
  * buttons.c
  *   |
  *   v
  * button_app.c
  *   |
  *   v
  * main.c
  *
  ******************************************************************************
  */
/* USER CODE END Header */


/* Includes ------------------------------------------------------------------*/

#include "main.h"


/* USER CODE BEGIN Includes */

#include <stdio.h>

#include "buzzer.h"
#include "buttons.h"
#include "button_app.h"






/* USER CODE END Includes */



/* Private variables ---------------------------------------------------------*/

ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;



/* USER CODE BEGIN PV */

/*
 * Private application variables will be added here
 * when required by the application.
 *
 * Currently no debug variable is needed.
 */


/* USER CODE END PV */



/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_ADC1_Init(void);

static void MX_I2C1_Init(void);





/**
 * @brief Main application entry point.
 */
int main(void)
{

    /*
     * Initialize HAL library.
     *
     * This configures SysTick
     * and prepares STM32 HAL layer.
     */
    HAL_Init();



    /*
     * Configure system clock.
     *
     * Blue Pill:
     * HSE 8MHz crystal
     * PLL x9
     * SYSCLK 72MHz
     */
    SystemClock_Config();



    /*
     * Initialize hardware peripherals.
     */
    MX_GPIO_Init();

    MX_ADC1_Init();

    MX_I2C1_Init();



    /* USER CODE BEGIN 2 */





    /*
     * Initialize buzzer module.
     */
    Buzzer_Init();



    /*
     * Initialize physical button driver.
     *
     * This layer only reads GPIO
     * and generates button events.
     */
    Buttons_Init();



    /*
     * Initialize application button layer.
     *
     * This converts hardware events
     * into application commands.
     */
    ButtonApp_Init();



    /*
     * Startup delay.
     *
     * Gives LCD module time to stabilize.
     */
    HAL_Delay(500);


//
//    LCD_Clear();
//
//    LCD_SetCursor(0,0);
//
//    LCD_Print("Button Test");
//
//    LCD_SetCursor(1,0);
//
//    LCD_Print("Ready");



    /* USER CODE END 2 */


    /* Infinite loop */

    /* USER CODE BEGIN WHILE */

    while (1)
    {





        Buzzer_Task();

        Buttons_Task();

        ButtonApp_Task();





    }
    /* USER CODE END WHILE */
}

/**
  ******************************************************************************
  * @brief System Clock Configuration
  *
  * @details
  * Configure STM32F103 clock tree.
  *
  * External crystal:
  *
  *      HSE = 8MHz
  *
  * PLL:
  *
  *      8MHz x 9 = 72MHz
  *
  ******************************************************************************
  */
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};



    /*
     * Configure oscillator.
     */
    RCC_OscInitStruct.OscillatorType =
            RCC_OSCILLATORTYPE_HSE;



    RCC_OscInitStruct.HSEState =
            RCC_HSE_ON;



    RCC_OscInitStruct.HSEPredivValue =
            RCC_HSE_PREDIV_DIV1;



    RCC_OscInitStruct.HSIState =
            RCC_HSI_ON;



    RCC_OscInitStruct.PLL.PLLState =
            RCC_PLL_ON;



    RCC_OscInitStruct.PLL.PLLSource =
            RCC_PLLSOURCE_HSE;



    RCC_OscInitStruct.PLL.PLLMUL =
            RCC_PLL_MUL9;



    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }




    /*
     * Configure CPU and bus clocks.
     */
    RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK |
            RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_PCLK1 |
            RCC_CLOCKTYPE_PCLK2;



    RCC_ClkInitStruct.SYSCLKSource =
            RCC_SYSCLKSOURCE_PLLCLK;



    RCC_ClkInitStruct.AHBCLKDivider =
            RCC_SYSCLK_DIV1;



    RCC_ClkInitStruct.APB1CLKDivider =
            RCC_HCLK_DIV2;



    RCC_ClkInitStruct.APB2CLKDivider =
            RCC_HCLK_DIV1;



    if(HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }




    /*
     * ADC clock configuration.
     *
     * ADC maximum clock on STM32F103:
     *
     * 72MHz / 2 = 36MHz
     */
    PeriphClkInit.PeriphClockSelection =
            RCC_PERIPHCLK_ADC;



    PeriphClkInit.AdcClockSelection =
            RCC_ADCPCLK2_DIV2;



    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

}





/**
  ******************************************************************************
  * @brief ADC1 Initialization Function
  *
  * @details
  *
  * ADC is initialized here.
  *
  * Current project stage:
  *
  * ADC hardware is prepared,
  * measurement application will be added later.
  *
  ******************************************************************************
  */
static void MX_ADC1_Init(void)
{

    ADC_ChannelConfTypeDef sConfig = {0};



    /*
     * Select ADC peripheral.
     */
    hadc1.Instance = ADC1;



    /*
     * Single conversion mode.
     */
    hadc1.Init.ScanConvMode =
            ADC_SCAN_DISABLE;



    hadc1.Init.ContinuousConvMode =
            DISABLE;



    hadc1.Init.DiscontinuousConvMode =
            DISABLE;



    hadc1.Init.ExternalTrigConv =
            ADC_SOFTWARE_START;



    hadc1.Init.DataAlign =
            ADC_DATAALIGN_RIGHT;



    hadc1.Init.NbrOfConversion =
            1;



    if(HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }




    /*
     * Configure ADC channel 0.
     *
     * PA0 pin.
     */
    sConfig.Channel =
            ADC_CHANNEL_0;



    sConfig.Rank =
            ADC_REGULAR_RANK_1;



    sConfig.SamplingTime =
            ADC_SAMPLETIME_1CYCLE_5;



    if(HAL_ADC_ConfigChannel(&hadc1,&sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}





/**
  ******************************************************************************
  * @brief I2C1 Initialization Function
  *
  * @details
  *
  * Used for LCD I2C backpack.
  *
  * Speed:
  *
  *      100kHz Standard Mode
  *
  ******************************************************************************
  */
static void MX_I2C1_Init(void)
{

    hi2c1.Instance = I2C1;



    hi2c1.Init.ClockSpeed =
            100000;



    hi2c1.Init.DutyCycle =
            I2C_DUTYCYCLE_2;



    hi2c1.Init.OwnAddress1 =
            0;



    hi2c1.Init.AddressingMode =
            I2C_ADDRESSINGMODE_7BIT;



    hi2c1.Init.DualAddressMode =
            I2C_DUALADDRESS_DISABLE;



    hi2c1.Init.OwnAddress2 =
            0;



    hi2c1.Init.GeneralCallMode =
            I2C_GENERALCALL_DISABLE;



    hi2c1.Init.NoStretchMode =
            I2C_NOSTRETCH_DISABLE;



    if(HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }

}





/**
  ******************************************************************************
  * @brief GPIO Initialization Function
  *
  * @details
  *
  * Configure:
  *
  * PB pins:
  *
  *      UP
  *      DOWN
  *      ENTER
  *      BACK
  *
  *
  * BUZZER:
  *
  *      Output pin
  *
  ******************************************************************************
  */
static void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};



    /*
     * Enable GPIO clocks.
     */
    __HAL_RCC_GPIOD_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();




    /*
     * Initial buzzer state:
     *
     * OFF
     */
    HAL_GPIO_WritePin(
            BUZZER_GPIO_Port,
            BUZZER_Pin,
            GPIO_PIN_RESET);




    /*
     * Configure buttons.
     *
     * Buttons are active LOW.
     *
     * Therefore internal pull-up is used.
     */
    GPIO_InitStruct.Pin =
            BTN_ENTER_Pin |
            BTN_DOWN_Pin  |
            BTN_UP_Pin    |
            BTN_BACK_Pin;



    GPIO_InitStruct.Mode =
            GPIO_MODE_INPUT;



    GPIO_InitStruct.Pull =
            GPIO_PULLUP;



    HAL_GPIO_Init(
            GPIOB,
            &GPIO_InitStruct);




    /*
     * Configure buzzer output.
     */
    GPIO_InitStruct.Pin =
            BUZZER_Pin;



    GPIO_InitStruct.Mode =
            GPIO_MODE_OUTPUT_PP;



    GPIO_InitStruct.Pull =
            GPIO_NOPULL;



    GPIO_InitStruct.Speed =
            GPIO_SPEED_FREQ_LOW;



    HAL_GPIO_Init(
            BUZZER_GPIO_Port,
            &GPIO_InitStruct);

}





/**
  ******************************************************************************
  * @brief Error Handler
  *
  * @details
  *
  * If any HAL initialization fails,
  * MCU stops here.
  *
  ******************************************************************************
  */
void Error_Handler(void)
{

    __disable_irq();



    while(1)
    {

        /*
         * Stay here.
         *
         * This indicates a fatal initialization error.
         */

    }

}



#ifdef USE_FULL_ASSERT


void assert_failed(uint8_t *file, uint32_t line)
{

    /*
     * User may add debugging output here.
     */

}


#endif
