/**
  ******************************************************************************
  * @file    battery_management.c
  * @brief   Battery management module implementation
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "battery_management.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t batteryLowFlag = 0;

/**
  * @brief  Initialize the battery management module
  * @retval None
  */
void BATTERY_Init(void)
{
  // Initialize GPIO for low battery warning
  // (already done in MX_GPIO_Init())

  // Reset low battery flag
  batteryLowFlag = 0;
}

/**
  * @brief  Update battery state
  * @retval None
  */
void BATTERY_Update(void)
{
  // This function is called periodically to update battery state
  // and handle low battery warning
  // The actual voltage measurement is done elsewhere (ADC_ReadAll)
}

/**
  * @brief  Calculate battery level from voltage
  * @param  voltage: Battery voltage
  * @retval Battery level (0-100%)
  */
uint8_t BATTERY_CalculateLevel(float voltage)
{
  int level;

  // Check if voltage is below minimum
  if (voltage <= BATTERY_MIN_VOLTAGE) {
    return 0;
  }

  // Check if voltage is above maximum
  if (voltage >= BATTERY_MAX_VOLTAGE) {
    return 100;
  }

  // Calculate level as percentage between min and max
  level = (int)((voltage - BATTERY_MIN_VOLTAGE) /
                (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100.0f);

  // Ensure result is within 0-100 range
  if (level < 0) level = 0;
  if (level > 100) level = 100;

  // Check if battery is low
  batteryLowFlag = BATTERY_IsLow(voltage);

  // Update warning LED if needed
  HAL_GPIO_WritePin(WARNING_LOW_VOLTAGE_GPIO_Port, WARNING_LOW_VOLTAGE_Pin,
                    batteryLowFlag ? GPIO_PIN_SET : GPIO_PIN_RESET);

  return (uint8_t)level;
}

/**
  * @brief  Check if battery is low
  * @param  voltage: Battery voltage
  * @retval 1 if battery is low, 0 otherwise
  */
uint8_t BATTERY_IsLow(float voltage)
{
  // Return 1 if voltage is below low threshold
  return (voltage < BATTERY_LOW_VOLTAGE) ? 1 : 0;
}
