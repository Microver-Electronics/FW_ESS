/**
  ******************************************************************************
  * @file    battery_management.h
  * @brief   Battery management module header
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BATTERY_MANAGEMENT_H
#define __BATTERY_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define BATTERY_MIN_VOLTAGE  11.0f  // Minimum battery voltage (0%)
#define BATTERY_MAX_VOLTAGE  14.4f  // Maximum battery voltage (100%)
#define BATTERY_LOW_VOLTAGE  11.5f  // Low battery warning threshold

/* Exported function prototypes ----------------------------------------------*/

/**
  * @brief  Initialize the battery management module
  * @retval None
  */
void BATTERY_Init(void);

/**
  * @brief  Update battery state
  * @retval None
  */
void BATTERY_Update(void);

/**
  * @brief  Calculate battery level from voltage
  * @param  voltage: Battery voltage
  * @retval Battery level (0-100%)
  */
uint8_t BATTERY_CalculateLevel(float voltage);

/**
  * @brief  Check if battery is low
  * @param  voltage: Battery voltage
  * @retval 1 if battery is low, 0 otherwise
  */
uint8_t BATTERY_IsLow(float voltage);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERY_MANAGEMENT_H */
