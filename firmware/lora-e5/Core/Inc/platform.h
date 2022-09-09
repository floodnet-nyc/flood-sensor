#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif


#define USE_BSP_DRIVER

#include <stdbool.h>
#include "stm32wlxx.h"
#include "main.h"
#include "stm32wlxx_ll_gpio.h"
#if defined(USE_BSP_DRIVER)

#include "stm32wlxx_LoRa_E5.h"
#include "stm32wlxx_LoRa_E5_radio.h"
#endif


#ifdef __cplusplus
}
#endif

#endif
