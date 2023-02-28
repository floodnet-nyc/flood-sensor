#include "spi.h"

#include "main.h"
#include "sys_app.h"

#define DUMMY_BYTE 0xA5

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi2_rx;
W25Q_status_reg w25q_reg;

static void W25Q_wait_until_write_end(void);
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

static uint8_t W25Q_send_receive(uint8_t data) {
	uint8_t ret;
	HAL_SPI_TransmitReceive(&hspi2, &data, &ret, 1, 100);
	return ret;
}

bool W25Q_verify_mfg_chipID(void) {
	uint8_t state;
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);
	state = W25Q_send_receive(JEDEC_ID);
	if (state != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "send receive failed!\n");
		return false;
	}
	uint32_t temp0 = W25Q_send_receive(DUMMY_BYTE);
	uint32_t temp1 = W25Q_send_receive(DUMMY_BYTE);
	uint32_t temp2 = W25Q_send_receive(DUMMY_BYTE);
	uint32_t temp = (temp0 << 16) | (temp1 << 8) | temp2;
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);
	APP_LOG(TS_OFF, VLEVEL_M, "chip is %s\n",
			(temp0 = 0xEF) ? "detected!" : "not detected...!");
	APP_LOG(TS_OFF, VLEVEL_M, "Manufacturer ID\t\t: 0x%X\n", temp0);
	APP_LOG(TS_OFF, VLEVEL_M, "Device ID(ID15-ID0)\t\t: 0x%X%X\n", temp1, temp2);
	APP_LOG(TS_OFF, VLEVEL_M, "JECID: %X\n", temp);	
	//if (temp0 != ID1 || temp1 != ID2 || temp2 != ID3 || temp != ID4) return false;
	return true;
}

static void W25Q_wait_until_write_end(void){
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);
	W25Q_send_receive(READ_STATUS_REG_1);
	uint8_t w25q_status;
	do{
		w25q_status = W25Q_send_receive(DUMMY_BYTE);
		HAL_Delay(1);
	} while((w25q_status & 0x01U) == 1);
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);
}

bool W25Q_calc_flash_pct(double* pct) {
	/*
	 *	---- Procedure to calculate good sectors/pages % of Flash IC --------
	 *	- write enable
	 *	- write all 1's page-by-page
	 *	- then wrtire all 0's page-by-page
	 *	- check for stuck bits page-by-page and calculate %
	 *
	 */
	APP_LOG(TS_ON, VLEVEL_M, "Attempting write enable...\n");
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);
	W25Q_send_receive(WRITE_ENABLE);
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);		
	uint32_t page_add = 0;
	uint8_t BYTE_FF = 0xFFU;
	for(int i=0;i<W25Q_TOTAL_PAGES;i++){
		W25Q_wait_until_write_end();
		HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);		
		W25Q_send_receive(PAGE_PROGRAM);
		W25Q_send_receive((page_add & 0xFF0000)>>16);
		W25Q_send_receive((page_add & 0xFF00)>>8);
		W25Q_send_receive((page_add & 0xFF));
		HAL_SPI_Transmit(&hspi2, &BYTE_FF, 256, 100);
		HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);
		W25Q_wait_until_write_end();
		page_add += 256;
	}

	page_add = 0;
	uint8_t BYTE_00 = 0x00U;
	for(int i=0;i<W25Q_TOTAL_PAGES;i++){
		W25Q_wait_until_write_end();
		HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);		
		W25Q_send_receive(PAGE_PROGRAM);
		W25Q_send_receive((page_add & 0xFF0000)>>16);
		W25Q_send_receive((page_add & 0xFF00)>>8);
		W25Q_send_receive((page_add & 0xFF));
		HAL_SPI_Transmit(&hspi2, &BYTE_00, 256, 100);
		HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);
		W25Q_wait_until_write_end();
		page_add += 256;
	}

	page_add = 0;
	uint8_t *rBuffer;
	uint16_t bad_pages_counter = 0;
	for(int i=0;i<W25Q_TOTAL_PAGES;i++){
		HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);		
		W25Q_send_receive(FAST_READ);
		W25Q_send_receive((page_add & 0xFF0000)>>16);
		W25Q_send_receive((page_add & 0xFF00)>>8);
		W25Q_send_receive((page_add & 0xFF));
		HAL_SPI_Receive(&hspi2, rBuffer, 256, 100);
		while( rBuffer!=NULL ){
			if(*rBuffer++ == 0xFF) { bad_pages_counter++; }
		}
		HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_SET);
		page_add += 256;
	}
	APP_LOG(TS_ON, VLEVEL_M, "bad pages are %d out of 4096!\n", bad_pages_counter);
	if (bad_pages_counter == 0)
		*pct = (double) 100;
	else
		*pct = (double) 100 * ((W25Q_TOTAL_PAGES - bad_pages_counter)/W25Q_TOTAL_PAGES);
	APP_LOG(TS_OFF, VLEVEL_M, "Bad pages percentage is: %f\n", &pct);
	return true;
}

bool W25Q_read_unique_chipID(uint8_t* buff){
	APP_LOG(TS_ON, VLEVEL_M, "Extracting unique chipID (UID63-UID0)...\n");
	HAL_GPIO_WritePin(W25Q_CS_Port, W25Q_CS_Pin, GPIO_PIN_RESET);
	uint8_t state;
	state = W25Q_send_receive(UNIQUE_ID);
	if (state != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "send receive failed!\n");
		return false;
	}
	for(int i=0; i<4; i++)
		W25Q_send_receive(DUMMY_BYTE);
	uint8_t *pBuff; 
	pBuff = buff;
	for(int i=0; i<8; i++)
		*pBuff++ = W25Q_send_receive(DUMMY_BYTE);
	return true;	
}

bool RUN_W25Q_test_procedure(uint8_t* buff, double* pct) {
	/*		
	 *	----- Test procedure to calculate IC good sector/pages % ------------
	 *	- read mfg chip id and validate
	 *	- read unique chip id 	(to be sent via uplink)
	 *	- write all 1's and read them - check for stuck bits
	 *	- repeat with 0's
	 *	- Calculate IC's good sectors/pages % 	(to be sent via uplink)
	 *
	 */
	bool mfg_verified = W25Q_verify_mfg_chipID();
	if (!mfg_verified) return false; 	/* failure */

	bool uid_extracted = W25Q_read_unique_chipID(buff);
	if (!uid_extracted) return false; 	/* failure */

	bool pct_calculated = W25Q_calc_flash_pct(pct);
	if (!pct_calculated) return false; 	/* failure */

	return true; 				/* success */
}
