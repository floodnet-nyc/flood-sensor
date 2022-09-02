#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32wlxx_hal.h"

void Error_Handler(void);

#define RTC_PREDIV_A ((1<<(15-RTC_N_PREDIV_S))-1)
#define RTC_N_PREDIV_S 10
#define RTC_PREDIV_S ((1<<RTC_N_PREDIV_S)-1)
#define ADC_IN_Pin GPIO_PIN_3
#define ADC_IN_GPIO_Port GPIOB
#define ADC_SW_Pin GPIO_PIN_7
#define ADC_SW_GPIO_Port GPIOB
#define MB_OC_Pin GPIO_PIN_5
#define MB_OC_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_6
#define LED_GPIO_Port GPIOB
#define MB_RX_Pin GPIO_PIN_1
#define MB_RX_GPIO_Port GPIOC
#define MB_CTL_Pin GPIO_PIN_0
#define MB_CTL_GPIO_Port GPIOC
#define MB_PWR_Pin GPIO_PIN_0
#define MB_PWR_GPIO_Port GPIOA

#ifdef __cplusplus
}
#endif

#endif 
