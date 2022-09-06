#include "adc_if.h"
#include "sys_app.h"

extern ADC_HandleTypeDef hadc;

static uint32_t ADC_ReadChannels(uint32_t channel);

void SYS_InitMeasurement(void) { hadc.Instance = ADC; }

void SYS_DeInitMeasurement(void) {}

uint16_t SYS_GetBatteryLevel() {
  HAL_GPIO_WritePin(GPIOB, ADC_SW_Pin, GPIO_PIN_RESET);
  uint16_t batt_mV;
  batt_mV = (uint16_t)ADC_ReadChannels(ADC_CHANNEL_2);
  APP_LOG(TS_OFF, VLEVEL_L, "adc reading: %d\n", batt_mV);
  return (uint16_t)batt_mV;
}

static uint32_t ADC_ReadChannels(uint32_t channel) {
  uint32_t ADCxConvertedValues = 0;
  ADC_ChannelConfTypeDef sConfig = {0};
  MX_ADC_Init();
  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK) {
    Error_Handler();
  }
  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_ADC_Start(&hadc) != HAL_OK) {
    Error_Handler();
  }
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
  HAL_ADC_Stop(&hadc);
  ADCxConvertedValues = HAL_ADC_GetValue(&hadc);
  HAL_ADC_DeInit(&hadc);
  return ADCxConvertedValues;
}
