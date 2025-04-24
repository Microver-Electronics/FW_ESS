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
#include "adc.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "system_control.h"
#include "battery_management.h"
#include "fault_handling.h"
#include "usb_com.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
SystemState_t systemState = {
  .batteryLevel = 0,
  .state = STATE_STANDBY,
  .fault = FAULT_NONE,
  .voltages = {0, 0, 0, 0}
};

uint8_t receiveBuffer[64];
uint8_t commandReady = 0;
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
  MX_ADC2_Init();
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/* Process incoming commands */
	if (commandReady) {
	  ProcessCommand();
	  commandReady = 0;
	}

	/* Update system state */
	SYSTEM_Update();

	/* Check for faults */
	FAULT_Check();

	/* Update battery state */
	BATTERY_Update();

	/* Send periodic status if needed */
	if (SYSTEM_ShouldSendStatus()) {
	  USB_SendStatus(&systemState);
	}
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// Add this to main.c in the ProcessCommand function

/**
  * @brief  Process received command from USB
  * @note   Parse and execute commands from the PC application
  * @retval None
  */
void ProcessCommand(void)
{
  char cmd = receiveBuffer[0];

  switch(cmd) {
    case 'S': // Status request
      // Update all values and send status
      ADC_ReadAll(systemState.voltages);
      systemState.batteryLevel = BATTERY_CalculateLevel(systemState.voltages[BANK_A]);
      USB_SendStatus(&systemState);
      break;

    case 'L': // LED control (L0-5)(0-1)
      if (receiveBuffer[1] >= '0' && receiveBuffer[1] <= '5' &&
          receiveBuffer[2] >= '0' && receiveBuffer[2] <= '1') {
        uint8_t ledIndex = receiveBuffer[1] - '0';
        uint8_t ledState = receiveBuffer[2] - '0';
        SYSTEM_SetLED(ledIndex, ledState);
      }
      break;

    case 'C': // Charge control (C0-2)
      if (receiveBuffer[1] >= '0' && receiveBuffer[1] <= '2') {
        uint8_t chargeMode = receiveBuffer[1] - '0';
        SYSTEM_SetChargeMode(chargeMode);
      }
      break;

    case 'P': // Power output control (P0-1)
      if (receiveBuffer[1] >= '0' && receiveBuffer[1] <= '1') {
        uint8_t powerState = receiveBuffer[1] - '0';
        SYSTEM_SetPowerOutput(powerState);
      }
      break;

    case 'R': // Relay control (R0-2)
      if (receiveBuffer[1] >= '0' && receiveBuffer[1] <= '2') {
        uint8_t relayMode = receiveBuffer[1] - '0';
        SYSTEM_SetRelayMode(relayMode);
      }
      break;

    case 'E': // Individual enable signal control (E0-3)(0-1)
      if (receiveBuffer[1] >= '0' && receiveBuffer[1] <= '3' &&
          receiveBuffer[2] >= '0' && receiveBuffer[2] <= '1') {
        uint8_t signalIndex = receiveBuffer[1] - '0';
        uint8_t signalState = receiveBuffer[2] - '0';
        SYSTEM_SetEnableSignal(signalIndex, signalState);
      }
      break;

    default:
      break;
  }
}

/**
  * @brief  USB CDC Received data callback
  * @note   Called by USB CDC when new data is received
  * @param  buffer: Pointer to received data
  * @param  length: Number of received bytes
  * @retval None
  */
void USB_DataReceived(uint8_t* buffer, uint32_t length)
{
  if (length > 0 && length < sizeof(receiveBuffer)) {
    memcpy(receiveBuffer, buffer, length);
    receiveBuffer[length] = 0; // Null-terminate
    commandReady = 1;
  }
}

/**
  * @brief  Timer elapsed callback
  * @note   This function is called when the timer period elapses
  * @param  htim: Timer handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
    // 1ms timer for system timing
    SYSTEM_TimerTick();
  }
}

/**
  * @brief  GPIO EXTI callback
  * @note   Called when configured GPIO interrupts occur
  * @param  GPIO_Pin: Specifies the pin that triggered the interrupt
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == FLT_BLOCK_100A_Pin) {
    FAULT_SetFaultFlag(FAULT_BLOCK_100A);
  }
  else if (GPIO_Pin == FLT_BLOCK_200A_Pin) {
    FAULT_SetFaultFlag(FAULT_BLOCK_200A);
  }
  else if (GPIO_Pin == FLT_CHARGE_Pin) {
    FAULT_SetFaultFlag(FAULT_CHARGE);
  }
  else if (GPIO_Pin == FLT_FAST_CHARGE_Pin) {
    FAULT_SetFaultFlag(FAULT_FAST_CHARGE);
  }
}
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
