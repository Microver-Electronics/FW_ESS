/**
  ******************************************************************************
  * @file    fault_handling.c
  * @brief   Fault handling module implementation
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "fault_handling.h"
#include "system_control.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t faultState = FAULT_NONE;
static uint32_t faultTimestamps[4] = {0}; // Timestamps for each fault bit

/**
  * @brief  Initialize the fault handling module
  * @retval None
  */
void FAULT_Init(void)
{
  // Initialize fault state
  faultState = FAULT_NONE;

  // Clear fault timestamps
  for (int i = 0; i < 4; i++) {
    faultTimestamps[i] = 0;
  }
}

/**
  * @brief  Check for faults
  * @retval None
  */
void FAULT_Check(void)
{
  uint8_t currentFaults = FAULT_NONE;
  uint32_t currentTime = HAL_GetTick();

  // Check BLOCK_200A fault line (PB1)
  if (HAL_GPIO_ReadPin(FLT_BLOCK_200A_GPIO_Port, FLT_BLOCK_200A_Pin) == GPIO_PIN_RESET) {
    currentFaults |= FAULT_BLOCK_200A;
  }

  // Check BLOCK_100A fault line (PA7)
  if (HAL_GPIO_ReadPin(FLT_BLOCK_100A_GPIO_Port, FLT_BLOCK_100A_Pin) == GPIO_PIN_RESET) {
    currentFaults |= FAULT_BLOCK_100A;
  }

  // Check CHARGE fault line (PA6)
  if (HAL_GPIO_ReadPin(FLT_CHARGE_GPIO_Port, FLT_CHARGE_Pin) == GPIO_PIN_RESET) {
    currentFaults |= FAULT_CHARGE;
  }

  // Check FAST_CHARGE fault line (PA5)
  if (HAL_GPIO_ReadPin(FLT_FAST_CHARGE_GPIO_Port, FLT_FAST_CHARGE_Pin) == GPIO_PIN_RESET) {
    currentFaults |= FAULT_FAST_CHARGE;
  }

  // Update fault state
  for (int i = 0; i < 4; i++) {
    uint8_t faultBit = (1 << i);

    // If fault is active
    if (currentFaults & faultBit) {
      // Set fault state
      faultState |= faultBit;
      // Update timestamp
      faultTimestamps[i] = currentTime;
    }
    // If fault is not active but was active
    else if (faultState & faultBit) {
      // Check if timeout has elapsed
      if (currentTime - faultTimestamps[i] > FAULT_TIMEOUT) {
        // Clear fault state
        faultState &= ~faultBit;
      }
    }
  }
}

/**
  * @brief  Set fault flag
  * @param  fault: Fault flag to set
  * @retval None
  */
void FAULT_SetFaultFlag(uint8_t fault)
{
  faultState |= fault;

  // Update timestamp for each bit that was set
  uint32_t currentTime = HAL_GetTick();
  for (int i = 0; i < 4; i++) {
    if (fault & (1 << i)) {
      faultTimestamps[i] = currentTime;
    }
  }
}

/**
  * @brief  Clear fault flag
  * @param  fault: Fault flag to clear
  * @retval None
  */
void FAULT_ClearFaultFlag(uint8_t fault)
{
  faultState &= ~fault;
}

/**
  * @brief  Get current fault state
  * @retval Current fault flags
  */
uint8_t FAULT_GetState(void)
{
  return faultState;
}
