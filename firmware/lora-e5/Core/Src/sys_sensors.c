#include "sys_sensors.h"
#include "ctype.h"
#include "functions.h"
#include "math.h"
#include "platform.h"
#include "stdint.h"
#include "stdio.h"
#include "stm32wlxx_ll_dma.h"
#include "sys_app.h"
#include "sys_conf.h"
#include "usart.h"

extern UART_HandleTypeDef hlpuart1;
volatile uint16_t readings_arr[30] = {0};
size_t arr_len = sizeof(readings_arr) / sizeof(readings_arr[0]);

void MaxBotix_Init(void) {
  MX_LPUART1_UART_Init();
  APP_LOG(TS_ON, VLEVEL_L, "\n\nLPUART1 has been initialized.\n\n");
  HAL_GPIO_WritePin(MB_PWR_GPIO_Port, MB_PWR_Pin, GPIO_PIN_RESET);
  //  todo: monitor MB_OC_Pin
  HAL_GPIO_WritePin(MB_CTL_GPIO_Port, MB_CTL_Pin, GPIO_PIN_RESET);
}

uint16_t Maxbotix_Single_Read(uint32_t timeout_mb, uint8_t number_of_tries) {
  unsigned char RxBuffer[4];
  bool newData = false;
  uint16_t dist_mm;
  unsigned char rx;
  uint8_t tries = 0;
  do {
    dist_mm = 0;
    HAL_GPIO_WritePin(MB_CTL_GPIO_Port, MB_CTL_Pin, GPIO_PIN_SET);
    HAL_Delay(1); // todo: implement HAL_Delay_Microseconds()
    HAL_GPIO_WritePin(MB_CTL_GPIO_Port, MB_CTL_Pin, GPIO_PIN_RESET);
    if (HAL_UART_Receive(&hlpuart1, (uint8_t *)&rx, 1, 20) == HAL_OK) {
      if ((char)rx == 'R') {
        if (HAL_UART_Receive(&hlpuart1, (uint8_t *)&RxBuffer, 4, timeout_mb) ==
            HAL_OK) {
          if (isdigit(RxBuffer[3])) {
            newData = true;
          }
          if (newData) {
            for (int i = 0; i < 4; i++)
              dist_mm += (RxBuffer[3 - i] - '0') * pow(10, i);
            if (dist_mm==0)
            	newData = false;
          }
        }
      } else {
        newData = false;
      }
    } else {
      tries++;
    }
  } while (newData == false && tries < number_of_tries - 1);
  APP_LOG(TS_OFF, VLEVEL_L, "dist_mm: %d\n", dist_mm);
  return dist_mm;
}

bool Test_Maxbotix_Init(void)
{
  /*		
	 *	----- Test procedure to validate Maxbotix readings % ------------
	 *	- read 100 readings from Maxbotix 
	 *	- check to make sure values within range of acceptable Maxbotix values
	 *	- if 100 readings are found within range, the test has passed
	 *	- if not something is wrong with the sensor, the test has failed
	 *
	 */
	uint8_t valid_readings = 0;
	uint8_t invalid_readings = 0;
	bool test_status = true;

	HAL_GPIO_WritePin(MB_PWR_GPIO_Port, MB_PWR_Pin, GPIO_PIN_SET);
	do {
		uint16_t reading = 0;
		reading = Maxbotix_Single_Read(150, 7);
		HAL_Delay(150);

		if ((reading >= 300) && (reading <= 10000))
		{
			valid_readings++;
		}
		else
		{
			invalid_readings++; //first reading after initialization will always be 0
		}

	} while ((valid_readings + invalid_readings) != 101);

	if (valid_readings == 100)
	{
		APP_LOG(TS_ON, VLEVEL_L, "100 Successful Maxbotix Readings Detected. Sensor is Reading Values\n");
	}
	else
	{
		test_status = false;
		APP_LOG(TS_ON, VLEVEL_L, "Sensor is reading improper values. Sensor has not initialized correctly\n");
	}
  return test_status; 
}

uint16_t Maxbotix_Read_Using_Modes(uint8_t averaging_method,
                                   uint16_t sampling_rate,
                                   uint8_t number_of_samples,
                                   uint32_t timeout_mb,
                                   uint8_t number_of_tries) {
  uint16_t dist_mm = 0;
  HAL_GPIO_WritePin(MB_PWR_GPIO_Port, MB_PWR_Pin, GPIO_PIN_SET);
  for (int i = 0; i < number_of_samples; i++) {
    readings_arr[i] = Maxbotix_Single_Read(timeout_mb, number_of_tries);
    HAL_Delay(sampling_rate);
  }
  HAL_GPIO_WritePin(MB_PWR_GPIO_Port, MB_PWR_Pin, GPIO_PIN_SET);
  switch (averaging_method) {
  case 1: /* Mean */
    dist_mm = mean(number_of_samples);
    break;
  case 2: /* Median */
    dist_mm = median(number_of_samples);
    break;
  case 3: /* Mode */
    dist_mm = mode_(number_of_samples);
    break;
  default: /* should never reach here */
    dist_mm = Maxbotix_Single_Read(timeout_mb, number_of_tries);
    break;
  }
  return dist_mm;
}
