#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void MaxBotix_Init(void);
uint16_t Maxbotix_Read_Using_Modes(uint8_t averaging_method,
                                   uint16_t sampling_rate,
                                   uint8_t number_of_samples,
                                   uint32_t timeout_mb, /* milliseconds */
                                   uint8_t number_of_tries);
extern volatile uint16_t readings_arr[30];

#ifdef __cplusplus
}
#endif

#endif
