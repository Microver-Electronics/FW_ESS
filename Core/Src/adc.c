/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc2;

/* ADC2 init function */
void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */
    /* ADC2 clock enable */
    __HAL_RCC_ADC2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA1     ------> ADC2_IN1
    PA2     ------> ADC2_IN2
    PA3     ------> ADC2_IN3
    PA4     ------> ADC2_IN4
    */
    GPIO_InitStruct.Pin = VFB_LOAD_ADC_Pin|VFB_BANK_A_ADC_Pin|VFB_BANK_B_ADC_Pin|VFB_CHARGE_ADC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC2_CLK_DISABLE();

    /**ADC2 GPIO Configuration
    PA1     ------> ADC2_IN1
    PA2     ------> ADC2_IN2
    PA3     ------> ADC2_IN3
    PA4     ------> ADC2_IN4
    */
    HAL_GPIO_DeInit(GPIOA, VFB_LOAD_ADC_Pin|VFB_BANK_A_ADC_Pin|VFB_BANK_B_ADC_Pin|VFB_CHARGE_ADC_Pin);

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Read a specific ADC channel
  * @param  channel: ADC channel to read
  * @retval Raw ADC value
  */
uint16_t ADC_ReadChannel(uint32_t channel)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  uint16_t adcValue = 0;

  // Configure ADC channel
  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  // Start ADC conversion
  HAL_ADC_Start(&hadc2);

  // Wait for conversion to complete
  if (HAL_ADC_PollForConversion(&hadc2, 100) == HAL_OK) {
    // Read ADC value
    adcValue = HAL_ADC_GetValue(&hadc2);
  }

  // Stop ADC conversion
  HAL_ADC_Stop(&hadc2);

  return adcValue;
}

/**
  * @brief  Convert ADC raw value to actual voltage
  * @param  adcValue: Raw ADC value
  * @retval Voltage in volts
  */
float ADC_ConvertToVoltage(uint16_t adcValue)
{
  // Convert ADC value to voltage using the voltage divider ratio
  // First convert to the ADC input voltage
  float voltage = (float)adcValue * ADC_REFERENCE / ADC_RESOLUTION;

  // Then apply the voltage divider ratio to get the actual voltage
  return voltage * VOLTAGE_DIVIDER_RATIO;
}

/**
  * @brief  Read all ADC channels for system voltages
  * @param  voltages: Array to store the voltage values (in volts)
  * @retval None
  */
void ADC_ReadAll(float* voltages)
{
  // Read LOAD voltage (ADC channel 1 - PA1)
  voltages[LOAD] = ADC_ConvertToVoltage(ADC_ReadChannel(ADC_CHANNEL_1));

  // Read BANK_A voltage (ADC channel 2 - PA2)
  voltages[BANK_A] = ADC_ConvertToVoltage(ADC_ReadChannel(ADC_CHANNEL_2));

  // Read BANK_B voltage (ADC channel 3 - PA3)
  voltages[BANK_B] = ADC_ConvertToVoltage(ADC_ReadChannel(ADC_CHANNEL_3));

  // Read CHARGE voltage (ADC channel 4 - PA4)
  voltages[CHARGE] = ADC_ConvertToVoltage(ADC_ReadChannel(ADC_CHANNEL_4));
}
/* USER CODE END 1 */
