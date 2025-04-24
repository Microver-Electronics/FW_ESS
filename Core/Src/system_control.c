/**
  ******************************************************************************
  * @file    system_control.c
  * @brief   System control module implementation
  ******************************************************************************
  * @attention
  *
  * MicroVer ESS Controller - STM32F103C8T6 Firmware
  * Created: 2025
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "system_control.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
static uint32_t systemTicks = 0;
static uint32_t lastStatusTime = 0;
static uint8_t ledStates[LED_COUNT] = {0};
static uint8_t currentChargeMode = CHARGE_OFF;
static uint8_t powerOutputEnabled = 0;

/* Private constants ---------------------------------------------------------*/
#define STATUS_INTERVAL 2000 // 2 seconds

/* Private function prototypes -----------------------------------------------*/
static void UpdateLEDs(void);
static void UpdateChargeMode(void);
static void UpdatePowerOutput(void);

/**
  * @brief  Initialize the system control module
  * @retval None
  */
void SYSTEM_Init(void)
{
  // Initial GPIO state already set by MX_GPIO_Init()

  // Set initial LED states to off
  for (int i = 0; i < LED_COUNT; i++) {
    ledStates[i] = 0;
  }

  // Apply initial states to hardware
  UpdateLEDs();
  UpdateChargeMode();
  UpdatePowerOutput();

  // Initialize relays in off state
  HAL_GPIO_WritePin(LATCH_IN1_GPIO_Port, LATCH_IN1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LATCH_IN2_GPIO_Port, LATCH_IN2_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  Update system status
  * @retval None
  */
void SYSTEM_Update(void)
{
  // Update hardware states if there were any changes
  UpdateLEDs();
  UpdateChargeMode();
  UpdatePowerOutput();
}

/**
  * @brief  Handle system timer ticks (called from timer ISR)
  * @retval None
  */
void SYSTEM_TimerTick(void)
{
  systemTicks++;
}

/**
  * @brief  Check if status should be sent periodically
  * @retval 1 if status should be sent, 0 otherwise
  */
uint8_t SYSTEM_ShouldSendStatus(void)
{
  if (systemTicks - lastStatusTime > STATUS_INTERVAL) {
    lastStatusTime = systemTicks;
    return 1;
  }
  return 0;
}

/**
  * @brief  Indicate error state
  * @retval None
  */
void SYSTEM_ErrorBlink(void)
{
  // Use all available LEDs to indicate error
  while (1) {
    // Toggle all LEDs
    HAL_GPIO_TogglePin(CTRL_LED_0_GPIO_Port, CTRL_LED_0_Pin);
    HAL_GPIO_TogglePin(CTRL_LED_50_GPIO_Port, CTRL_LED_50_Pin);
    HAL_GPIO_TogglePin(CTRL_LED_100_GPIO_Port, CTRL_LED_100_Pin);
    HAL_GPIO_TogglePin(CTRL_LED_CHARGING_GPIO_Port, CTRL_LED_CHARGING_Pin);
    HAL_GPIO_TogglePin(CTRL_LED_CHECK_CHARGER_GPIO_Port, CTRL_LED_CHECK_CHARGER_Pin);
    HAL_GPIO_TogglePin(WARNING_LOW_VOLTAGE_GPIO_Port, WARNING_LOW_VOLTAGE_Pin);
    HAL_Delay(100);
  }
}

/**
  * @brief  Set LED state
  * @param  ledIndex: LED index (0-5)
  * @param  state: 0 for off, 1 for on
  * @retval None
  */
void SYSTEM_SetLED(uint8_t ledIndex, uint8_t state)
{
  if (ledIndex < LED_COUNT) {
    ledStates[ledIndex] = state ? 1 : 0;
  }
}

/**
  * @brief  Set charge mode
  * @param  mode: 0 for off, 1 for normal, 2 for fast
  * @retval None
  */
void SYSTEM_SetChargeMode(uint8_t mode)
{
  if (mode <= CHARGE_FAST) {
    currentChargeMode = mode;
  }
}

/**
  * @brief  Set power output state
  * @param  state: 0 for off, 1 for on
  * @retval None
  */
void SYSTEM_SetPowerOutput(uint8_t state)
{
  powerOutputEnabled = state ? 1 : 0;
}

/**
  * @brief  Set relay mode
  * @param  mode: 0 for off, 1 for set, 2 for reset
  * @retval None
  */
void SYSTEM_SetRelayMode(uint8_t mode)
{
  switch (mode) {
    case RELAY_OFF:
      HAL_GPIO_WritePin(LATCH_IN1_GPIO_Port, LATCH_IN1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LATCH_IN2_GPIO_Port, LATCH_IN2_Pin, GPIO_PIN_RESET);
      break;

    case RELAY_SET:
      HAL_GPIO_WritePin(LATCH_IN2_GPIO_Port, LATCH_IN2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LATCH_IN1_GPIO_Port, LATCH_IN1_Pin, GPIO_PIN_SET);
      HAL_Delay(100); // Pulse for 100ms
      HAL_GPIO_WritePin(LATCH_IN1_GPIO_Port, LATCH_IN1_Pin, GPIO_PIN_RESET);
      break;

    case RELAY_RESET:
      HAL_GPIO_WritePin(LATCH_IN1_GPIO_Port, LATCH_IN1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LATCH_IN2_GPIO_Port, LATCH_IN2_Pin, GPIO_PIN_SET);
      HAL_Delay(100); // Pulse for 100ms
      HAL_GPIO_WritePin(LATCH_IN2_GPIO_Port, LATCH_IN2_Pin, GPIO_PIN_RESET);
      break;
  }
}

/**
  * @brief  Update LEDs based on current state
  * @retval None
  */
static void UpdateLEDs(void)
{
  // Update LED 0%
  HAL_GPIO_WritePin(CTRL_LED_0_GPIO_Port, CTRL_LED_0_Pin,
                    ledStates[LED_0_PERCENT] ? GPIO_PIN_SET : GPIO_PIN_RESET);

  // Update LED 50%
  HAL_GPIO_WritePin(CTRL_LED_50_GPIO_Port, CTRL_LED_50_Pin,
                    ledStates[LED_50_PERCENT] ? GPIO_PIN_SET : GPIO_PIN_RESET);

  // Update LED 100%
  HAL_GPIO_WritePin(CTRL_LED_100_GPIO_Port, CTRL_LED_100_Pin,
                    ledStates[LED_100_PERCENT] ? GPIO_PIN_SET : GPIO_PIN_RESET);

  // Update LED Charging
  HAL_GPIO_WritePin(CTRL_LED_CHARGING_GPIO_Port, CTRL_LED_CHARGING_Pin,
                    ledStates[LED_CHARGING] ? GPIO_PIN_SET : GPIO_PIN_RESET);

  // Update LED Check Charger
  HAL_GPIO_WritePin(CTRL_LED_CHECK_CHARGER_GPIO_Port, CTRL_LED_CHECK_CHARGER_Pin,
                    ledStates[LED_CHECK_CHARGER] ? GPIO_PIN_SET : GPIO_PIN_RESET);

  // Update Warning LED
  HAL_GPIO_WritePin(WARNING_LOW_VOLTAGE_GPIO_Port, WARNING_LOW_VOLTAGE_Pin,
                    ledStates[LED_WARNING] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  Update charge mode based on current setting
  * @retval None
  */
static void UpdateChargeMode(void)
{
  switch (currentChargeMode) {
    case CHARGE_OFF:
      // Disable both charge modes
      HAL_GPIO_WritePin(EN_CHARGE_GPIO_Port, EN_CHARGE_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(EN_FAST_CHARGE_GPIO_Port, EN_FAST_CHARGE_Pin, GPIO_PIN_RESET);
      break;

    case CHARGE_NORMAL:
      // Enable normal charge, disable fast charge
      HAL_GPIO_WritePin(EN_CHARGE_GPIO_Port, EN_CHARGE_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(EN_FAST_CHARGE_GPIO_Port, EN_FAST_CHARGE_Pin, GPIO_PIN_RESET);
      break;

    case CHARGE_FAST:
      // Enable fast charge, disable normal charge
      HAL_GPIO_WritePin(EN_CHARGE_GPIO_Port, EN_CHARGE_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(EN_FAST_CHARGE_GPIO_Port, EN_FAST_CHARGE_Pin, GPIO_PIN_SET);
      break;
  }
}

/**
  * @brief  Update power output based on current setting
  * @retval None
  */
static void UpdatePowerOutput(void)
{
  // Enable/disable power output by controlling the blocking MOSFETs
  if (powerOutputEnabled) {
    // Enable power output
    HAL_GPIO_WritePin(EN_BLOCK_100A_GPIO_Port, EN_BLOCK_100A_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EN_BLOCK_200A_GPIO_Port, EN_BLOCK_200A_Pin, GPIO_PIN_SET);
  } else {
    // Disable power output
    HAL_GPIO_WritePin(EN_BLOCK_100A_GPIO_Port, EN_BLOCK_100A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EN_BLOCK_200A_GPIO_Port, EN_BLOCK_200A_Pin, GPIO_PIN_RESET);
  }
}

/**
  * @brief  Set enable signal state
  * @param  signalIndex: 0=EN_FAST_CHARGE, 1=EN_CHARGE, 2=EN_BLOCK_100A, 3=EN_BLOCK_200A
  * @param  state: 0 for off, 1 for on
  * @retval None
  */
void SYSTEM_SetEnableSignal(uint8_t signalIndex, uint8_t state)
{
  GPIO_PinState pinState = state ? GPIO_PIN_SET : GPIO_PIN_RESET;

  switch (signalIndex) {
    case 0: // EN_FAST_CHARGE
      HAL_GPIO_WritePin(EN_FAST_CHARGE_GPIO_Port, EN_FAST_CHARGE_Pin, pinState);
      break;

    case 1: // EN_CHARGE
      HAL_GPIO_WritePin(EN_CHARGE_GPIO_Port, EN_CHARGE_Pin, pinState);
      break;

    case 2: // EN_BLOCK_100A
      HAL_GPIO_WritePin(EN_BLOCK_100A_GPIO_Port, EN_BLOCK_100A_Pin, pinState);
      break;

    case 3: // EN_BLOCK_200A
      HAL_GPIO_WritePin(EN_BLOCK_200A_GPIO_Port, EN_BLOCK_200A_Pin, pinState);
      break;

    default:
      break;
  }
}
