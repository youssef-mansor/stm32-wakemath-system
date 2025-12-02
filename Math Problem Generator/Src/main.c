/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stdio.h"
#include <string.h>
#include "liquidcrystal_i2c.h"
#include <stdlib.h> // For rand(), atoi()
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

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
const char KEYPAD_MAP[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'0', 'F', 'E', 'D'}
};

// Rows: PB1, PB0, PA7, PA6
GPIO_TypeDef* ROW_PORTS[4] = {GPIOB, GPIOB, GPIOA, GPIOA};
const uint16_t ROW_PINS[4] = {GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_7, GPIO_PIN_6};

// Cols: PA5, PA4, PA3, PA1
GPIO_TypeDef* COL_PORTS[4] = {GPIOA, GPIOA, GPIOA, GPIOA};
const uint16_t COL_PINS[4] = {GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_1};



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
char Keypad_Scan(void)
{
    // 1. Iterate through every ROW
    for(int r = 0; r < 4; r++)
    {
        // Set current Row to LOW (Active)
        HAL_GPIO_WritePin(ROW_PORTS[r], ROW_PINS[r], GPIO_PIN_RESET);

        // 2. Check every COL
        for(int c = 0; c < 4; c++)
        {
            // If Column is LOW, button is pressed (Input is pulled up, pulled low by row connection)
            if(HAL_GPIO_ReadPin(COL_PORTS[c], COL_PINS[c]) == GPIO_PIN_RESET)
            {
                // Wait for stable connection (Debounce)
                HAL_Delay(20); 

                // Check again to ensure it wasn't noise
                if(HAL_GPIO_ReadPin(COL_PORTS[c], COL_PINS[c]) == GPIO_PIN_RESET)
                {
                    // Block execution while button is held down (optional, prevents spamming)
                    while(HAL_GPIO_ReadPin(COL_PORTS[c], COL_PINS[c]) == GPIO_PIN_RESET);

                    // Set Row back to HIGH before returning
                    HAL_GPIO_WritePin(ROW_PORTS[r], ROW_PINS[r], GPIO_PIN_SET);
                    
                    return KEYPAD_MAP[r][c];
                }
            }
        }

        // Set Row back to HIGH (Inactive)
        HAL_GPIO_WritePin(ROW_PORTS[r], ROW_PINS[r], GPIO_PIN_SET);
    }

    return 0; // No key pressed
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Force Reset I2C Bus
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // Startup Message
  /* Initialize LCD */
  HD44780_Init(2); 
  HD44780_Clear();
  HD44780_SetCursor(0,0);
  HD44780_PrintStr("Math Game Init..");
  HAL_Delay(1000);

  char msg[50];
  char key_pressed;
  
  // Game Variables
  int num1, num2, expected_sum;
  int op_selector;
  char op_char;
  char input_buffer[5]; // To store user input string
  int input_index = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		// 1. Select Operation (0: Add, 1: Sub, 2: Mul, 3: Div)
		op_selector = rand() % 4;

		switch(op_selector)
		{
				case 0: // Addition
						num1 = (rand() % 9) + 1; // 1-9
						num2 = (rand() % 9) + 1; // 1-9
						expected_sum = num1 + num2;
						op_char = '+';
						break;

				case 1: // Subtraction
						num1 = (rand() % 9) + 1;
						num2 = (rand() % 9) + 1;
						// Ensure positive result for simple typing
						if (num2 > num1) { 
								int temp = num1; 
								num1 = num2; 
								num2 = temp; 
						}
						expected_sum = num1 - num2;
						op_char = '-';
						break;

				case 2: // Multiplication
						num1 = (rand() % 9) + 1;
						num2 = (rand() % 9) + 1;
						expected_sum = num1 * num2;
						op_char = '*';
						break;

				case 3: // Division
						expected_sum = (rand() % 9) + 1; // The answer (1-9)
						num2 = (rand() % 9) + 1;         // The divisor (1-9)
						num1 = expected_sum * num2;      // The dividend
						op_char = '/';
						break;
		}
		
		// Reset Input Buffer
		memset(input_buffer, 0, sizeof(input_buffer));
		input_index = 0;

		// 2. Display Problem
		HD44780_Clear();
		HD44780_SetCursor(0,0);
		sprintf(msg, "%d %c %d = ?", num1, op_char, num2);
		HD44780_PrintStr(msg);
		
		HD44780_SetCursor(0,1);
		HD44780_PrintStr("Ans: ");

		// 3. Wait for Input Loop
		int answer_submitted = 0;
		while(!answer_submitted)
		{
				key_pressed = Keypad_Scan();
				
				if(key_pressed != 0)
				{
						// Handle Numeric Inputs
						if(key_pressed >= '0' && key_pressed <= '9')
						{
								if(input_index < 3) // Prevent overflow
								{
										input_buffer[input_index] = key_pressed;
										input_index++;
										
										// Display typed number
										HD44780_PrintStr(&key_pressed); 
								}
						}
						// Handle 'C' for Clear
						else if (key_pressed == 'C')
						{
								memset(input_buffer, 0, sizeof(input_buffer));
								input_index = 0;
								HD44780_SetCursor(5, 1); // Move back to after "Ans: "
								HD44780_PrintStr("    "); // Clear displayed text
								HD44780_SetCursor(5, 1);
						}
						// Handle 'D' for Enter/Submit
						else if (key_pressed == 'D')
						{
								answer_submitted = 1;
						}
						
						HAL_Delay(150); // Small UI debounce
				}
		}

		// 4. Check Answer
		int user_val = atoi(input_buffer);
		
		HD44780_Clear();
		HD44780_SetCursor(0,0);
		
		if(user_val == expected_sum)
		{
				HD44780_PrintStr("Correct!");
				// Optional: Send to UART for logging
				HAL_UART_Transmit(&huart2, (uint8_t*)"Correct Answer\r\n", 16, 100);
		}
		else
		{
				sprintf(msg, "Wrong! Ans:%d", expected_sum);
				HD44780_PrintStr(msg);
		}
		
		HAL_Delay(2000); // Wait before next question
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
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
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA3 PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 LD3_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
