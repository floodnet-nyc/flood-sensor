#include "spi.h"
#include "sys_app.h"
#include "main.h"

#define DUMMY_BYTE 0xA5

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi2_rx;

static uint8_t W25Q_send_receive(uint8_t data);

void MX_SPI2_Init(void) {
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;
	hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi2) != HAL_OK) {
		Error_Handler();
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if (spiHandle->Instance == SPI2) {
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S2;
		PeriphClkInitStruct.I2s2ClockSelection = RCC_I2S2CLKSOURCE_HSI;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
			Error_Handler();
		}

		__HAL_RCC_SPI2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**SPI2 GPIO Configuration
		  PB14     ------> SPI2_MISO
		  PA10     ------> SPI2_MOSI
		  PB13     ------> SPI2_SCK
		  */
		GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		hdma_spi2_tx.Instance = DMA1_Channel1;
		hdma_spi2_tx.Init.Request = DMA_REQUEST_SPI2_TX;
		hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_spi2_tx.Init.Mode = DMA_NORMAL;
		hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(spiHandle, hdmatx, hdma_spi2_tx);

		hdma_spi2_rx.Instance = DMA1_Channel2;
		hdma_spi2_rx.Init.Request = DMA_REQUEST_SPI2_RX;
		hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_spi2_rx.Init.Mode = DMA_NORMAL;
		hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK) {
			Error_Handler();
		}

		__HAL_LINKDMA(spiHandle, hdmarx, hdma_spi2_rx);

		HAL_NVIC_SetPriority(SPI2_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle) {
	if (spiHandle->Instance == SPI2) {
		
		__HAL_RCC_SPI2_CLK_DISABLE();

		/**SPI2 GPIO Configuration
		  PB14     ------> SPI2_MISO
		  PA10     ------> SPI2_MOSI
		  PB13     ------> SPI2_SCK
		  */
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14 | GPIO_PIN_13);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
		HAL_DMA_DeInit(spiHandle->hdmatx);
		HAL_DMA_DeInit(spiHandle->hdmarx);
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
		
	}
}

static uint8_t W25Q_send_receive(uint8_t data){
	uint8_t ret;
	HAL_SPI_TransmitReceive(&hspi2, &data, &ret, 1, 100);
	return ret;
}

W25Q_status W25Q_read_chipID(uint8_t *buff){
	uint8_t state;
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);
	state = W25Q_send_receive(JEDEC_ID);
	if (state!=HAL_OK){
		APP_LOG(TS_OFF, VLEVEL_M, "send receive failed!\n");
	}
	uint32_t temp0 = W25Q_send_receive(DUMMY_BYTE);
	uint32_t temp1 = W25Q_send_receive(DUMMY_BYTE);
	uint32_t temp2 = W25Q_send_receive(DUMMY_BYTE);
	uint32_t temp = (temp0<<16) | (temp1<<8) | temp2;
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);
	APP_LOG(TS_OFF, VLEVEL_M, "chipID: %d\n", temp0);
	APP_LOG(TS_OFF, VLEVEL_M, "chipID: %d\n", temp1);
	APP_LOG(TS_OFF, VLEVEL_M, "chipID: %d\n", temp2);
	APP_LOG(TS_OFF, VLEVEL_M, "chipID: %d\n", temp);
	APP_LOG(TS_OFF, VLEVEL_M, "chip is %s\n", (temp0=0xEF)?"detected!":"not detected...!");
	return state;
}

W25Q_status W25Q_test_procedure(void){
	/*		----- Test procedure to calculate IC good sector/pages % -----
	 *	- read mfg chip id and validate
	 *	- read unique chip id 					(to be sent via uplink)
	 *	- write all 1's and read them - check for stuck bits
	 *	- repeat with 0's
	 *	- Calculate IC's good sectors/pages percentage 		(to be sent via uplink)
	 *	
	 */
}
