#include "usart_if.h"


extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart2;

uint8_t charRx;

const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver = {
    vcom_Init,
    vcom_DeInit,
    vcom_ReceiveInit,
    vcom_Trace_DMA,
};

static void (*TxCpltCallback)(void *);
static void (*RxCpltCallback)(uint8_t *rxChar, uint16_t size, uint8_t error);

UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *)) {

  TxCpltCallback = cb;
  MX_DMA_Init();
  MX_USART2_UART_Init();
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_27);
  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t vcom_DeInit(void) {

  __HAL_RCC_USART2_FORCE_RESET();
  __HAL_RCC_USART2_RELEASE_RESET();

  HAL_UART_MspDeInit(&huart2);

  HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);

  return UTIL_ADV_TRACE_OK;
}

void vcom_Trace(uint8_t *p_data, uint16_t size) {

  HAL_UART_Transmit(&huart2, p_data, size, 1000);
}

UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size) {

  HAL_UART_Transmit_DMA(&huart2, p_data, size);
  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t
vcom_ReceiveInit(void (*RxCb)(uint8_t *rxChar, uint16_t size, uint8_t error)) {

  UART_WakeUpTypeDef WakeUpSelection;

  RxCpltCallback = RxCb;

  WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;

  HAL_UARTEx_StopModeWakeUpSourceConfig(&huart2, WakeUpSelection);

  while (__HAL_UART_GET_FLAG(&huart2, USART_ISR_BUSY) == SET)
    ;

  while (__HAL_UART_GET_FLAG(&huart2, USART_ISR_REACK) == RESET)
    ;

  __HAL_UART_ENABLE_IT(&huart2, UART_IT_WUF);

  HAL_UARTEx_EnableStopMode(&huart2);

  HAL_UART_Receive_IT(&huart2, &charRx, 1);

  return UTIL_ADV_TRACE_OK;
}

void vcom_Resume(void) {

  if (HAL_UART_Init(&huart2) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

  if (huart->Instance == USART2) {
    TxCpltCallback(NULL);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  if (huart->Instance == USART2) {
    if ((NULL != RxCpltCallback) && (HAL_UART_ERROR_NONE == huart->ErrorCode)) {
      RxCpltCallback(&charRx, 1, 0);
    }
    HAL_UART_Receive_IT(huart, &charRx, 1);
  }
}
