#include "main.h"

#include "app_fatfs.h"
#include "app_lorawan.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "gpio.h"
#include "spi.h"
#include "sys_app.h"
#include "user_diskio.h"
#include "spi.h"

void SystemClock_Config(void);

FATFS FatFs;
FRESULT fres;
int32_t ProcessStatus = 0;

int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_LoRaWAN_Init();
	MX_SPI2_Init();
	uint8_t b;
	W25Q_read_chipID(&b);
//	APP_LOG(TS_ON, VLEVEL_M, "Initializing FATFS...\n");
//	MX_FATFS_Init();
//	HAL_Delay(100);
//	APP_LOG(TS_ON, VLEVEL_M, "FATFS init successful.\n");
//
//	// Mount drive
//	fres = f_mount(&FatFs, "", 0);
//	if (fres != FR_OK) {
//		APP_LOG(TS_ON, VLEVEL_M, "mount failed.\n");
//		while (1)
//			;
//	} else {
//		APP_LOG(TS_ON, VLEVEL_M, "mounted fatfs.\n");
//	}
//
//	uint32_t freeClust;
//	FATFS* fs_ptr = &FatFs;
//	fres = f_getfree("", &freeClust,
//			&fs_ptr);  // Warning! This fills fs.n_fatent and fs.csize!
//	if (fres != FR_OK) {
//		APP_LOG(TS_ON, VLEVEL_M, "f_getfree() failed, res = %d\r\n", fres);
//		while (1)
//			;
//	}
//	APP_LOG(TS_ON, VLEVEL_M, "f_getfree() done!\r\n");
//
//	uint32_t totalBlocks = (FatFs.n_fatent - 2) * FatFs.csize;
//	uint32_t freeBlocks = freeClust * FatFs.csize;
//
//	APP_LOG(TS_ON, VLEVEL_M, "Total blocks: %lu (%lu Mb)\r\n", totalBlocks,
//			totalBlocks / 2000);
//	APP_LOG(TS_ON, VLEVEL_M, "Free blocks: %lu (%lu Mb)\r\n", freeBlocks,
//			freeBlocks / 2000);
//
//	DIR dir;
//	fres = f_opendir(&dir, "/");
//	if (fres != FR_OK) {
//		APP_LOG(TS_ON, VLEVEL_M, "open dir failed.\n");
//		while (1)
//			;
//	} else {
//		APP_LOG(TS_ON, VLEVEL_M, "open dir success.\n");
//	}
//	//
//	//	fres = f_closedir(&dir);
//	//	if(fres != FR_OK) {
//	//		APP_LOG(TS_ON, VLEVEL_M, "close dir failed.\n");
//	//		while(1);
//	//	} else {
//	//		APP_LOG(TS_ON, VLEVEL_M, "close dir success.\n");
//	//	}
//
//	FIL logFile;
//	fres = f_open(&logFile, "log.txt", FA_OPEN_APPEND | FA_WRITE);
//	if (fres != FR_OK) {
//		APP_LOG(TS_ON, VLEVEL_M, "file open failed.\n");
//		while (1)
//			;
//	} else {
//		APP_LOG(TS_ON, VLEVEL_M, "file opened.\n");
//	}

	while (1) {
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType =
		RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3 | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
		RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

void Error_Handler(void) {
	__disable_irq();
	while (1) {
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {}
#endif
