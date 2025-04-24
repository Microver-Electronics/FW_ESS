/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VFB_LOAD_ADC_Pin GPIO_PIN_1
#define VFB_LOAD_ADC_GPIO_Port GPIOA
#define VFB_BANK_A_ADC_Pin GPIO_PIN_2
#define VFB_BANK_A_ADC_GPIO_Port GPIOA
#define VFB_BANK_B_ADC_Pin GPIO_PIN_3
#define VFB_BANK_B_ADC_GPIO_Port GPIOA
#define VFB_CHARGE_ADC_Pin GPIO_PIN_4
#define VFB_CHARGE_ADC_GPIO_Port GPIOA
#define FLT_FAST_CHARGE_Pin GPIO_PIN_5
#define FLT_FAST_CHARGE_GPIO_Port GPIOA
#define FLT_CHARGE_Pin GPIO_PIN_6
#define FLT_CHARGE_GPIO_Port GPIOA
#define FLT_BLOCK_100A_Pin GPIO_PIN_7
#define FLT_BLOCK_100A_GPIO_Port GPIOA
#define EN_BLOCK_200A_Pin GPIO_PIN_0
#define EN_BLOCK_200A_GPIO_Port GPIOB
#define FLT_BLOCK_200A_Pin GPIO_PIN_1
#define FLT_BLOCK_200A_GPIO_Port GPIOB
#define WARNING_LOW_VOLTAGE_Pin GPIO_PIN_10
#define WARNING_LOW_VOLTAGE_GPIO_Port GPIOB
#define CTRL_LED_100_Pin GPIO_PIN_11
#define CTRL_LED_100_GPIO_Port GPIOB
#define CTRL_LED_50_Pin GPIO_PIN_12
#define CTRL_LED_50_GPIO_Port GPIOB
#define CTRL_LED_0_Pin GPIO_PIN_13
#define CTRL_LED_0_GPIO_Port GPIOB
#define CTRL_LED_CHARGING_Pin GPIO_PIN_14
#define CTRL_LED_CHARGING_GPIO_Port GPIOB
#define CTRL_LED_CHECK_CHARGER_Pin GPIO_PIN_15
#define CTRL_LED_CHECK_CHARGER_GPIO_Port GPIOB
#define EN_FAST_CHARGE_Pin GPIO_PIN_8
#define EN_FAST_CHARGE_GPIO_Port GPIOA
#define EN_CHARGE_Pin GPIO_PIN_9
#define EN_CHARGE_GPIO_Port GPIOA
#define EN_BLOCK_100A_Pin GPIO_PIN_10
#define EN_BLOCK_100A_GPIO_Port GPIOA
#define CTRL_SPK_Pin GPIO_PIN_15
#define CTRL_SPK_GPIO_Port GPIOA
#define LATCH_IN1_Pin GPIO_PIN_4
#define LATCH_IN1_GPIO_Port GPIOB
#define LATCH_IN2_Pin GPIO_PIN_5
#define LATCH_IN2_GPIO_Port GPIOB
#define LATCH_FB1_Pin GPIO_PIN_6
#define LATCH_FB1_GPIO_Port GPIOB
#define LATCH_FB2_Pin GPIO_PIN_7
#define LATCH_FB2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
