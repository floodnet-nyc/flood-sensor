#ifndef __STM32WLxx_IT_H
#define __STM32WLxx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif


void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TAMP_STAMP_LSECSS_SSRU_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void USART2_IRQHandler(void);
void LPUART1_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);
void SUBGHZ_Radio_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif
