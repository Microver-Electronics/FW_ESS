/**
  ******************************************************************************
  * @file    fault_handling.h
  * @brief   Fault handling module header
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FAULT_HANDLING_H
#define __FAULT_HANDLING_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define FAULT_TIMEOUT 3000 // Timeout for fault clearing in ms

/* Exported function prototypes ----------------------------------------------*/

/**
  * @brief  Initialize the fault handling module
  * @retval None
  */
void FAULT_Init(void);

/**
  * @brief  Check for faults
  * @retval None
  */
void FAULT_Check(void);

/**
  * @brief  Set fault flag
  * @param  fault: Fault flag to set
  * @retval None
  */
void FAULT_SetFaultFlag(uint8_t fault);

/**
  * @brief  Clear fault flag
  * @param  fault: Fault flag to clear
  * @retval None
  */
void FAULT_ClearFaultFlag(uint8_t fault);

/**
  * @brief  Get current fault state
  * @retval Current fault flags
  */
uint8_t FAULT_GetState(void);

#ifdef __cplusplus
}
#endif

#endif /* __FAULT_HANDLING_H */
