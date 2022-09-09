#include "rtc.h"

RTC_HandleTypeDef hrtc;

void MX_RTC_Init(void) {
  RTC_AlarmTypeDef sAlarm = {0};
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_PREDIV_A;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_ONLY;
  if (HAL_RTC_Init(&hrtc) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_RTCEx_SetSSRU_IT(&hrtc) != HAL_OK) {
    Error_Handler();
  }
  sAlarm.BinaryAutoClr = RTC_ALARMSUBSECONDBIN_AUTOCLR_NO;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDBINMASK_NONE;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, 0) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *rtcHandle) {
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if (rtcHandle->Instance == RTC) {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
      Error_Handler();
    }
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
    HAL_NVIC_SetPriority(TAMP_STAMP_LSECSS_SSRU_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TAMP_STAMP_LSECSS_SSRU_IRQn);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *rtcHandle) {
  if (rtcHandle->Instance == RTC) {
    __HAL_RCC_RTC_DISABLE();
    __HAL_RCC_RTCAPB_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TAMP_STAMP_LSECSS_SSRU_IRQn);
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  }
}
