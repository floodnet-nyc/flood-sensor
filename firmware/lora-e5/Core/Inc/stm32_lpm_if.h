#ifndef __STM32_LPM_IF_H__
#define __STM32_LPM_IF_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32_lpm.h"


void PWR_EnterOffMode(void);
void PWR_ExitOffMode(void);
void PWR_EnterStopMode(void);
void PWR_ExitStopMode(void);
void PWR_EnterSleepMode(void);
void PWR_ExitSleepMode(void);


#ifdef __cplusplus
}
#endif

#endif
