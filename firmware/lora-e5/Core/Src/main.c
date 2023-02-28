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
#include "sys_sensors.h"
#include "lora_app.h"

void SystemClock_Config(void);

FATFS FatFs;
FRESULT fres;
int32_t ProcessStatus = 0;

uint8_t *flash_UID;
double flash_pct;
bool run_flash_test;
bool run_maxbotix_test;


int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_SPI2_Init();
  	SystemApp_Init();	
	/*		Test Procedure		
	 *	- Start
	 *	- Initialize HAL, Clocks, GPIO, SPI, and LoRaWAN stack
	 *	- Start blinking LED
	 *	- Check for MaxBotix
	 *		- Check for valid readings 
	 *	- Check Flash Chip ID via SPI
	 *	- Start joining LoRaWAN
	 *	- When joined, stop blinking and drive LED ON always
	 *	- Send test payload <Maxbotix(Y/N), Flash ChipID(Y/N)>
	 *	- End 
	 */

	run_flash_test = RUN_W25Q_test_procedure(flash_UID, &flash_pct);
	if(!run_flash_test) { APP_LOG(TS_OFF, VLEVEL_M, "SPI Flash IC test failed!\n");}
	else {APP_LOG(TS_OFF, VLEVEL_M, "success!\n");}
	APP_LOG(TS_OFF, VLEVEL_M, "UID(64) is: ");
	uint8_t *print_UID;
	print_UID = flash_UID;
	for (int i=0; i<8; i++)
		APP_LOG(TS_OFF, VLEVEL_M, "%X", *print_UID++);
	APP_LOG(TS_OFF, VLEVEL_M, "\n");	
	
	run_maxbotix_test = Test_Maxbotix_Init(); 
	if(!run_maxbotix_test) { APP_LOG(TS_OFF, VLEVEL_M, "Maxbotix Readings test failed!\n");}
	else {APP_LOG(TS_OFF, VLEVEL_M, "success!\n");}
	
	MX_LoRaWAN_Init();

	while (1) {
		MX_LoRaWAN_Process();
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
