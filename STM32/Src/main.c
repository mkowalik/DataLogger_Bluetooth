
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include "digital_out_driver.h"
#include "uart_driver.h"
#include "hc05_driver.h"
#include "bluetooth_protocol_middleware.h"

#include "string.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

UartDriver_TypeDef 			uart1Driver;

HC05Driver_TypeDef			hc05Driver;

DigitalOutDriver_TypeDef	led1Driver;
DigitalOutDriver_TypeDef	led2Driver;
DigitalOutDriver_TypeDef	led3Driver;

DigitalOutDriver_TypeDef	debugOut0;

DigitalOutDriver_TypeDef	hc05KeyDriver;

DigitalOutDriver_Pin_TypeDef hc05KeyPin = HC05_KEY_Pin;

DigitalOutDriver_Pin_TypeDef ledDebug1Pin = LD1_Pin;
DigitalOutDriver_Pin_TypeDef ledDebug2Pin = LD2_Pin;
DigitalOutDriver_Pin_TypeDef ledDebug3Pin = LD3_Pin;

DigitalOutDriver_Pin_TypeDef debugOut0Pin = DUBUG_OUT_0_Pin;

BluetoothProtocolMiddleware_TypeDef	btMiddleware;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

extern UART_HandleTypeDef huart1;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	DigitalOutDriver_setHigh(&debugOut0);
	if (huart == (&huart1)){
		if (UartDriver_receivedBytesCallback(&uart1Driver) != UartDriver_Status_OK){
			Error_Handler();
		}
	}
	DigitalOutDriver_setLow(&debugOut0);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if (huart == (&huart1)){
		if (UartDriver_transmitCompleteCallback(&uart1Driver) != UartDriver_Status_OK){
			Error_Handler();
		}
	}
}

void foo(uint8_t byte, void* dummy){

	DigitalOutDriver_toggle(&led3Driver);

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  DigitalOutDriver_init(&led1Driver, (DigitalOutDriver_Port_TypeDef*)LD1_GPIO_Port, &ledDebug1Pin, DigitalOutDriver_StartLevel_High);
  DigitalOutDriver_init(&led2Driver, (DigitalOutDriver_Port_TypeDef*)LD2_GPIO_Port, &ledDebug2Pin, DigitalOutDriver_StartLevel_High);
  DigitalOutDriver_init(&led3Driver, (DigitalOutDriver_Port_TypeDef*)LD3_GPIO_Port, &ledDebug3Pin, DigitalOutDriver_StartLevel_High);

  DigitalOutDriver_init(&debugOut0, (DigitalOutDriver_Port_TypeDef*)DUBUG_OUT_0_GPIO_Port, &debugOut0Pin, DigitalOutDriver_StartLevel_High);

  DigitalOutDriver_init(&hc05KeyDriver, (DigitalOutDriver_Port_TypeDef*)HC05_KEY_GPIO_Port, &hc05KeyPin, DigitalOutDriver_StartLevel_High);

  UartDriver_init(&uart1Driver, &huart1, 38400);

  HC05Driver_init(&hc05Driver, HC05Driver_Role_Slave, &uart1Driver, &hc05KeyDriver, HC05_DATA_BAUDRATE, HC05_DEVICE_NAME, HC05_PASSWORD);

//  BluetoothProtocolMiddleware_init(&btMiddleware, &hc05Driver);

  HC05Driver_Status_TypeDef st = HC05Driver_setReceiveDataCallback(&hc05Driver, foo, NULL, NULL);
/*  uint8_t buffer[] = {1, 2, 3, 4};

  UartDriver_sendBytes(&uartDriver, buffer, 4);
  UartDriver_setBaudRate(&uartDriver, 38400);
  UartDriver_sendBytes(&uartDriver, buffer, 4);*/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  DigitalOutDriver_setHigh(&led2Driver);
	  HAL_Delay(200);
	  DigitalOutDriver_setLow(&led2Driver);
	  HAL_Delay(400);
	  DigitalOutDriver_toggle(&led1Driver);
	  HAL_Delay(100);
//	  BluetoothProtocolMiddleware_worker(&btMiddleware);

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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
