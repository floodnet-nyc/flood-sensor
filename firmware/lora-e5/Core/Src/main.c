#include "main.h"
#include "app_lorawan.h"
#include "gpio.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "user_diskio.h"

void SystemClock_Config(void);

FATFS FatFs;
FRESULT fres;
FIL SFLASHPath[];
char buffer[512];
char str[100] = {"Hello World\0"};
int bw;

int main(void) {

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_LoRaWAN_Init();
  MX_SPI2_Init();

  MX_FATFS_Init();
    /* USER CODE BEGIN 2 */
    HAL_Delay(100);


    //format drive
    //f_mkfs("", FM_ANY, 0, buffer, sizeof(buffer));

    FATFS_LinkDriver(&USER_Driver, SFLASHPath);

    //Mount drive
    fres = f_mount(&FatFs, &SFLASHPath, 0);
    if (fres != FR_OK)
    {
//      APP_LOG();
      while(1);
    }


    fres = f_open(&SFLASHPath, "test", FA_CREATE_ALWAYS | FA_WRITE);
    if (fres != FR_OK)
    {
   	 while(1);
    }
    else
    {
  	  fres = f_write(&SFLASHPath, &str, sizeof(str), bw);
  	  fres = f_close(&SFLASHPath);
    }


    fres = f_open(&SFLASHPath, "test", FA_READ);
    if (fres != FR_OK)
    {
  	  while(1);
    }
    else
    {
  	  fres = f_read(&SFLASHPath, &buffer, 11, bw);
    }
    f_close(&SFLASHPath);


  while (1) {
//    MX_LoRaWAN_Process();

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
void assert_failed(uint8_t *file, uint32_t line) {}
#endif
