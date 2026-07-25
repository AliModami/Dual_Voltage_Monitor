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
  *   |
  *   v
  * main.c
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "lcd_i2c.h"
#include "lcd_display.h"
#include "buzzer.h"
#include "buttons.h"
#include "menu_items.h"
#include "menu_controller.h"
#include "menu_renderer.h"
#include "menu_edit.h"
#include "screen_manager.h"
#include "menu_actions.h"
#include "adc_app.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */




  /*
   * Startup delay.
   * Gives LCD module time to stabilize.
   */

  HAL_Delay(2000);


  // ( Handler I2C1، Address, Columns، Rows )َ
  //LCD_Init(&hi2c1, 0x27, 20, 4);

  LCD_Display_Init(&hi2c1);

  MenuController_Init();

  MenuEdit_Init();

  MenuRenderer_Init();

  ScreenManager_Init();

  Buzzer_Init();

  Buttons_Init();

  ScreenManager_Render();

//  MenuRenderer_Update();
//
//  LCD_Display_RenderMenu();

  //ADC_App_Init();



//------------ LCD Test Line Begin --------------------------
//  LCD_Clear();
//  HAL_Delay(5);
//  LCD_SetCursor(0, 0);
//  LCD_Print("Hello STM32!");
//  LCD_SetCursor(0, 1);       // Line2
//  LCD_Print("Line 2!");
//  LCD_SetCursor(0, 2);       // Line2
//  LCD_Print("Line 3!");
//  LCD_SetCursor(0, 3);       // Line2
//  LCD_Print("Line 4!");
//------------ LCD Test Line END ----------------------------




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    while (1)
    {


        ADC_App_Task();
        //ScreenManager_Task();
        Buzzer_Task();
        Buttons_Task();





//----------------------------------------- Button Test  Begin--------------------------------------


        Button_Event_t event;


        if(Buttons_GetEvent(&event))
        {

            switch(event.button)
            {


//            case BUTTON_ID_UP:
//
//
//                if(MenuEdit_IsActive())
//                {
//                    MenuEdit_Increment();
//                }
//                else
//                {
//                    MenuController_MoveUp();
//                }
//
//
//                break;


            case BUTTON_ID_UP:

                if(MenuEdit_IsActive())
                {
                    MenuEdit_Increment();
                }
                else
                {
                    MenuController_MoveUp();

                    printf("UP Page=%d Index=%d\r\n",
                           MenuController_GetCurrentPageId(),
                           MenuController_GetSelectedIndex());
                }

                break;














//            case BUTTON_ID_DOWN:
//
//
//                if(MenuEdit_IsActive())
//                {
//                    MenuEdit_Decrement();
//                }
//                else
//                {
//                    MenuController_MoveDown();
//                }
//
//
//                break;




            case BUTTON_ID_DOWN:

                if(MenuEdit_IsActive())
                {
                    MenuEdit_Decrement();
                }
                else
                {
                    MenuController_MoveDown();

                    printf("DOWN Page=%d Index=%d\r\n",
                           MenuController_GetCurrentPageId(),
                           MenuController_GetSelectedIndex());
                }

                break;












//            case BUTTON_ID_ENTER:
//
//                if(MenuEdit_IsActive())
//                {
//                    MenuEdit_Confirm();
//                }
//                else
//                {
//                    MenuController_Enter();
//                }
//
//                break;



            case BUTTON_ID_ENTER:

                if(MenuEdit_IsActive())
                {
                    MenuEdit_Confirm();
                }
                else
                {
                    const MenuItem_t *item;

                    item = MenuController_GetSelectedItem();

                    if(item != 0)
                    {
                        printf("ENTER: Page=%d Index=%d Type=%d Child=%d Text=%s\r\n",
                               MenuController_GetCurrentPageId(),
                               MenuController_GetSelectedIndex(),
                               item->type,
                               item->child_page,
                               item->text);
                    }

                    MenuController_Enter();
                }

                break;





            case BUTTON_ID_BACK:

                if(MenuEdit_IsActive())
                {
                    MenuEdit_Cancel();
                }
                else
                {
                    MenuController_Back();
                }

                break;




                default:

                    break;

            }



            ScreenManager_Render();

        }



//-----------------------------------------Button Test  End------------------------------------

    }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_ENTER_Pin BTN_DOWN_Pin BTN_UP_Pin BTN_BACK_Pin */
  GPIO_InitStruct.Pin = BTN_ENTER_Pin|BTN_DOWN_Pin|BTN_UP_Pin|BTN_BACK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
