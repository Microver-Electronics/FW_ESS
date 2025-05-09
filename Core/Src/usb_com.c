/**
  ******************************************************************************
  * @file    usb_com.c
  * @brief   USB communication module implementation
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_com.h"
#include <stdio.h>
#include <string.h>

/* USB device instance declaration - this should match your USB middleware */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Private variables ---------------------------------------------------------*/
static char txBuffer[128];

/* System state string representations */
/*
static const char* stateStrings[] = {
  "STANDBY",
  "CHARGING",
  "DISCHARGING",
  "ERROR"
};
*/
/**
  * @brief  Send system status over USB
  * @param  state: Pointer to system state structure
  * @retval None
  */
void USB_SendStatus(SystemState_t* state)
{
  int length = 0;

  // Format status string: BAT:xx,STATE:x,FAULT:xx,V1:xxxx,V2:xxxx,V3:xxxx,V4:xxxx
  length = sprintf(txBuffer, "BAT:%d,STATE:%d,FAULT:%d,V1:%d,V2:%d,V3:%d,V4:%d\r\n",
                  state->batteryLevel,
                  state->state,
                  state->fault,
                  (int)(state->voltages[LOAD] * 1000),     // Convert to millivolts
                  (int)(state->voltages[CHARGE] * 1000),   // Convert to millivolts
                  (int)(state->voltages[BANK_A] * 1000),   // Convert to millivolts
                  (int)(state->voltages[BANK_B] * 1000));  // Convert to millivolts

  // Send via USB
  CDC_Transmit_FS((uint8_t*)txBuffer, length);
}

/**
  * @brief  Send error message over USB
  * @param  errorMsg: Error message string
  * @retval None
  */
void USB_SendError(const char* errorMsg)
{
  int length = 0;

  // Format error message
  length = sprintf(txBuffer, "ERROR:%s\r\n", errorMsg);

  // Send via USB
  CDC_Transmit_FS((uint8_t*)txBuffer, length);
}

/**
  * @brief  Handle received USB data
  * @param  buffer: Pointer to received data
  * @param  length: Length of received data
  * @retval None
  */
/*
void USB_HandleRxData(uint8_t* buffer, uint32_t length)
{
  // Call the main data received callback
  USB_DataReceived(buffer, length);
}
*/
/**
  * @brief  CDC receive callback - called by USB CDC driver
  * @param  buffer: Pointer to received data
  * @param  length: Length of received data
  * @retval Status
  */
uint8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  USB_HandleRxData(Buf, *Len);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  return (USBD_OK);
}
