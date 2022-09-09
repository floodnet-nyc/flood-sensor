#include "stm32wlxx_LoRa_E5_radio.h"

int32_t BSP_RADIO_Init(void) {
  GPIO_InitTypeDef gpio_init_structure = {0};

  RF_SW_CTRL1_GPIO_CLK_ENABLE();

  gpio_init_structure.Pin = RF_SW_CTRL1_PIN;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(RF_SW_CTRL1_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin = RF_SW_CTRL2_PIN;
  HAL_GPIO_Init(RF_SW_CTRL2_GPIO_PORT, &gpio_init_structure);

  HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

int32_t BSP_RADIO_DeInit(void) {
  RF_SW_RX_GPIO_CLK_DISABLE();

  HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);

  HAL_GPIO_DeInit(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN);
  HAL_GPIO_DeInit(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN);

  return BSP_ERROR_NONE;
}

int32_t BSP_RADIO_ConfigRFSwitch(BSP_RADIO_Switch_TypeDef Config) {
  switch (Config) {
  case RADIO_SWITCH_OFF: {

    HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);
    break;
  }
  case RADIO_SWITCH_RX: {

    HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);
    break;
  }
  case RADIO_SWITCH_RFO_LP: {

    HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_SET);
    break;
  }
  case RADIO_SWITCH_RFO_HP: {

    HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_SET);
    break;
  }
  default:
    break;
  }

  return BSP_ERROR_NONE;
}

int32_t BSP_RADIO_GetTxConfig(void) { return RADIO_CONF_RFO_HP; }

int32_t BSP_RADIO_IsTCXO(void) { return RADIO_CONF_TCXO_SUPPORTED; }

int32_t BSP_RADIO_IsDCDC(void) { return RADIO_CONF_DCDC_SUPPORTED; }

int32_t
BSP_RADIO_GetRFOMaxPowerConfig(BSP_RADIO_RFOMaxPowerConfig_TypeDef Config) {
  int32_t ret;

  if (Config == RADIO_RFO_LP_MAXPOWER) {
    ret = RADIO_CONF_RFO_LP_MAX_15_dBm;
  } else {
    ret = RADIO_CONF_RFO_HP_MAX_22_dBm;
  }

  return ret;
}
