/**
  ******************************************************************************
  * @file    usb_com.h
  * @brief   USB communication module header
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_COM_H
#define __USB_COM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_cdc_if.h"

/* Exported function prototypes ----------------------------------------------*/

/**
  * @brief  Send system status over USB
  * @param  state: Pointer to system state structure
  * @retval None
  */
void USB_SendStatus(SystemState_t* state);

/**
  * @brief  Send error message over USB
  * @param  errorMsg: Error message string
  * @retval None
  */
void USB_SendError(const char* errorMsg);

/**
  * @brief  Handle received USB data
  * @param  buffer: Pointer to received data
  * @param  length: Length of received data
  * @retval None
  */
void USB_HandleRxData(uint8_t* buffer, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* __USB_COM_H */
