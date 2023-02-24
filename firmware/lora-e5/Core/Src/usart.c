#include "usart.h"

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_lpuart1_rx;
DMA_HandleTypeDef hdma_usart2_tx;

void MX_LPUART1_UART_Init(void) {
	hlpuart1.Instance = LPUART1;
	hlpuart1.Init.BaudRate = 9600;
	hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
	hlpuart1.Init.StopBits = UART_STOPBITS_1;
	hlpuart1.Init.Parity = UART_PARITY_NONE;
	hlpuart1.Init.Mode = UART_MODE_RX;
	hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_SWAP_INIT;
	hlpuart1.AdvancedInit.Swap = UART_ADVFEATURE_SWAP_ENABLE;
	if (HAL_HalfDuplex_Init(&hlpuart1) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) !=
			HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) !=
			HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_EnableFifoMode(&hlpuart1) != HAL_OK) {
		Error_Handler();
	}
}

void MX_USART2_UART_Init(void) {
	huart2.Instance = USART2;
#ifdef GPS_TESTER
	huart2.Init.BaudRate = 9600;
#else 
	huart2.Init.BaudRate = 115200;
#endif
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) !=
			HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) !=
			HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_EnableFifoMode(&huart2) != HAL_OK) {
		Error_Handler();
	}
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if (uartHandle->Instance == LPUART1) {
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
		PeriphClkInitStruct.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_LSE;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
			Error_Handler();
		}

		__HAL_RCC_LPUART1_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

		GPIO_InitStruct.Pin = MB_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
		HAL_GPIO_Init(MB_RX_GPIO_Port, &GPIO_InitStruct);

		hdma_lpuart1_rx.Instance = DMA1_Channel4;
		hdma_lpuart1_rx.Init.Request = DMA_REQUEST_LPUART1_RX;
		hdma_lpuart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_lpuart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_lpuart1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_lpuart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_lpuart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_lpuart1_rx.Init.Mode = DMA_CIRCULAR;
		hdma_lpuart1_rx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_lpuart1_rx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmarx, hdma_lpuart1_rx);

		HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(LPUART1_IRQn);

	} else if (uartHandle->Instance == USART2) {
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
		PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
			Error_Handler();
		}

		__HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		hdma_usart2_tx.Instance = DMA1_Channel5;
		hdma_usart2_tx.Init.Request = DMA_REQUEST_USART2_TX;
		hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_tx.Init.Mode = DMA_NORMAL;
		hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(uartHandle, hdmatx, hdma_usart2_tx);

		HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle) {
	if (uartHandle->Instance == LPUART1) {
		__HAL_RCC_LPUART1_CLK_DISABLE();

		HAL_GPIO_DeInit(MB_RX_GPIO_Port, MB_RX_Pin);

		HAL_DMA_DeInit(uartHandle->hdmarx);

		HAL_NVIC_DisableIRQ(LPUART1_IRQn);

	} else if (uartHandle->Instance == USART2) {
		__HAL_RCC_USART2_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3 | GPIO_PIN_2);

		HAL_DMA_DeInit(uartHandle->hdmatx);

		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}
}
