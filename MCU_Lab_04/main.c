/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include <stdio.h>
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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
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
    char *names[2] = {
        "Piyawut",
        "Hanu"
    };

    /* เปลี่ยนเป็นรหัสนักศึกษาจริง */
    char *studentIds[2] = {
        "1168304620051",
        "1168304620036"
    };

    const char *welcomeText = "<< Press button to show >>";

    int8_t memberIndex = -1;   /* -1 หมายถึงหน้าแรก */
    uint8_t showHome = 1;

    LCD_ScrollState scroll = {0, 1, 0};

    /* index 0 = ปุ่มขวา PA0, index 1 = ปุ่มซ้าย PA1 */
    const uint16_t buttonPins[2] = {
        GPIO_PIN_0,
        GPIO_PIN_1
    };

    GPIO_PinState lastRaw[2] = {
        GPIO_PIN_SET,
        GPIO_PIN_SET
    };

    GPIO_PinState stable[2] = {
        GPIO_PIN_SET,
        GPIO_PIN_SET
    };

    uint32_t changedAt[2] = {0, 0};
    /* USER CODE END 1 */

    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();

    /* USER CODE BEGIN 2 */

    /* ตั้ง PA0 และ PA1 เป็น Input Pull-up */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef buttonConfig = {0};
    buttonConfig.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    buttonConfig.Mode = GPIO_MODE_INPUT;
    buttonConfig.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &buttonConfig);

    lcd_init();
    lcd_backlight_on();
    lcd_clear();

    lcd_put_cur(0, 0);
    lcd_send_string("Member List");

    lcd_scroll_bounce(
        &scroll, welcomeText, 1, 300, 1);

    /* USER CODE END 2 */

    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        uint32_t now = HAL_GetTick();
        uint8_t pressed[2] = {0, 0};

        /* อ่านปุ่มและกรองหน้าสัมผัสเด้ง 30 ms */
        for (uint8_t i = 0; i < 2; i++)
        {
            GPIO_PinState raw =
                HAL_GPIO_ReadPin(GPIOA, buttonPins[i]);

            if (raw != lastRaw[i])
            {
                lastRaw[i] = raw;
                changedAt[i] = now;
            }

            if (((uint32_t)(now - changedAt[i]) >= 30U)
                && (raw != stable[i]))
            {
                stable[i] = raw;

                /* รับเฉพาะจังหวะเริ่มกด */
                if (stable[i] == GPIO_PIN_RESET)
                {
                    pressed[i] = 1;
                }
            }
        }

        /* ปุ่ม 2: กลับหน้าแรก
           หากกดพร้อมกัน ให้ปุ่มกลับหน้าแรกมีสิทธิ์ก่อน */
        if (pressed[1])
        {
            showHome = 1;
            memberIndex = -1;

            lcd_clear();
            lcd_put_cur(0, 0);
            lcd_send_string("Member List");

            lcd_scroll_bounce(
                &scroll, welcomeText, 1, 300, 1);
        }
        /* ปุ่ม 1: แสดงสมาชิกคนถัดไป */
        else if (pressed[0])
        {
            if (showHome)
            {
                memberIndex = 0;
                showHome = 0;
            }
            else
            {
                memberIndex = (memberIndex + 1) % 2;
            }

            lcd_clear();

            lcd_put_cur(0, 0);
            lcd_send_string(names[memberIndex]);

            lcd_put_cur(1, 0);
            lcd_send_string(studentIds[memberIndex]);
        }

        /* เลื่อนข้อความเฉพาะเมื่ออยู่หน้าแรก */
        if (showHome)
        {
            lcd_scroll_bounce(
                &scroll, welcomeText, 1, 300, 0);
        }

        HAL_Delay(1);
    }
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
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
