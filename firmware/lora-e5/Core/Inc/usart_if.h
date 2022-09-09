#include "stm32_adv_trace.h"
#include "usart.h"
#include "dma.h"

#ifndef __USART_IF_H__
#define __USART_IF_H__

#ifdef __cplusplus
extern "C" {
#endif


UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *));
UTIL_ADV_TRACE_Status_t vcom_ReceiveInit(void (*RxCb)(uint8_t *rxChar, uint16_t size, uint8_t error));
UTIL_ADV_TRACE_Status_t vcom_DeInit(void);
void vcom_Trace(uint8_t *p_data, uint16_t size);
UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size);
void vcom_IRQHandler(void);
void vcom_DMA_TX_IRQHandler(void);
void vcom_Resume(void);


#ifdef __cplusplus
}
#endif

#endif
