#include "stm32wlxx_it.h"
#include "main.h"

extern DMA_HandleTypeDef hdma_lpuart1_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
extern SUBGHZ_HandleTypeDef hsubghz;

void NMI_Handler(void) {

  while (1) {
  }
}

void HardFault_Handler(void) {

  while (1) {
  }
}

void MemManage_Handler(void) {

  while (1) {
  }
}

void BusFault_Handler(void) {

  while (1) {
  }
}

void UsageFault_Handler(void) {

  while (1) {
  }
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) {}

void TAMP_STAMP_LSECSS_SSRU_IRQHandler(void) {

  HAL_RTCEx_SSRUIRQHandler(&hrtc);
}

void DMA1_Channel4_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_lpuart1_rx); }

void DMA1_Channel5_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_usart2_tx); }

void USART2_IRQHandler(void) { HAL_UART_IRQHandler(&huart2); }

void LPUART1_IRQHandler(void) { HAL_UART_IRQHandler(&hlpuart1); }

void RTC_Alarm_IRQHandler(void) { HAL_RTC_AlarmIRQHandler(&hrtc); }

void SUBGHZ_Radio_IRQHandler(void) { HAL_SUBGHZ_IRQHandler(&hsubghz); }
