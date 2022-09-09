#ifndef __TIMER_IF_H__
#define __TIMER_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32_timer.h"
#include "stm32_systime.h"


UTIL_TIMER_Status_t TIMER_IF_Init(void);
UTIL_TIMER_Status_t TIMER_IF_StartTimer(uint32_t timeout);
UTIL_TIMER_Status_t TIMER_IF_StopTimer(void);
uint32_t TIMER_IF_SetTimerContext(void);
uint32_t TIMER_IF_GetTimerContext(void);
uint32_t TIMER_IF_GetTimerElapsedTime(void);
uint32_t TIMER_IF_GetTimerValue(void);
uint32_t TIMER_IF_GetMinimumTimeout(void);
void TIMER_IF_DelayMs(uint32_t delay);
uint32_t TIMER_IF_Convert_ms2Tick(uint32_t timeMilliSec);
uint32_t TIMER_IF_Convert_Tick2ms(uint32_t tick);
uint32_t TIMER_IF_GetTime(uint16_t *subSeconds);
void TIMER_IF_BkUp_Write_Seconds(uint32_t Seconds);
uint32_t TIMER_IF_BkUp_Read_Seconds(void);
void TIMER_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds);
uint32_t TIMER_IF_BkUp_Read_SubSeconds(void);


#ifdef __cplusplus
}
#endif

#endif
