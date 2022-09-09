#include "subghz.h"

SUBGHZ_HandleTypeDef hsubghz;

void MX_SUBGHZ_Init(void) {
  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_8;
  if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_SUBGHZ_MspInit(SUBGHZ_HandleTypeDef *subghzHandle) {
  __HAL_RCC_SUBGHZSPI_CLK_ENABLE();
  HAL_NVIC_SetPriority(SUBGHZ_Radio_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
}

void HAL_SUBGHZ_MspDeInit(SUBGHZ_HandleTypeDef *subghzHandle) {
  __HAL_RCC_SUBGHZSPI_CLK_DISABLE();
  HAL_NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
}
