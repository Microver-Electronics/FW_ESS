/**
  ******************************************************************************
  * @file    system_control.h
  * @brief   System control module header
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTEM_CONTROL_H
#define __SYSTEM_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported function prototypes ----------------------------------------------*/

/**
  * @brief  Initialize the system control module
  * @retval None
  */
void SYSTEM_Init(void);

/**
  * @brief  Update system status
  * @retval None
  */
void SYSTEM_Update(void);

/**
  * @brief  Handle system timer ticks (called from timer ISR)
  * @retval None
  */
void SYSTEM_TimerTick(void);

/**
  * @brief  Check if status should be sent periodically
  * @retval 1 if status should be sent, 0 otherwise
  */
uint8_t SYSTEM_ShouldSendStatus(void);

/**
  * @brief  Blink LED to indicate error
  * @retval None
  */
void SYSTEM_ErrorBlink(void);

/**
  * @brief  Set LED state
  * @param  ledIndex: LED index (0-5)
  * @param  state: 0 for off, 1 for on
  * @retval None
  */
void SYSTEM_SetLED(uint8_t ledIndex, uint8_t state);

/**
  * @brief  Set charge mode
  * @param  mode: 0 for off, 1 for normal, 2 for fast
  * @retval None
  */
void SYSTEM_SetChargeMode(uint8_t mode);

/**
  * @brief  Set power output state
  * @param  state: 0 for off, 1 for on
  * @retval None
  */
void SYSTEM_SetPowerOutput(uint8_t state);

/**
  * @brief  Set relay mode
  * @param  mode: 0 for off, 1 for set, 2 for reset
  * @retval None
  */
void SYSTEM_SetRelayMode(uint8_t mode);

/**
  * @brief  Set enable signal state
  * @param  signalIndex: 0=EN_FAST_CHARGE, 1=EN_CHARGE, 2=EN_BLOCK_100A, 3=EN_BLOCK_200A
  * @param  state: 0 for off, 1 for on
  * @retval None
  */
void SYSTEM_SetEnableSignal(uint8_t signalIndex, uint8_t state);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_CONTROL_H */
