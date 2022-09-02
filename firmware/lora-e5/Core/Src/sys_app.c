#include "sys_app.h"

#include <stdio.h>

#include "adc_if.h"
#include "platform.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#include "stm32_systime.h"
#include "sys_debug.h"
#include "sys_sensors.h"
#include "timer_if.h"
#include "utilities_def.h"

#define MAX_TS_SIZE (int)16
#define LORAWAN_MAX_BAT 254

static uint8_t SYS_TimerInitialisedFlag = 0;

static void TimestampNow(uint8_t *buff, uint16_t *size);
static void tiny_snprintf_like(char *buf, uint32_t maxsize,
			       const char *strFormat, ...);

void SystemApp_Init(void) {
	__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
	UTIL_TIMER_Init();
	SYS_TimerInitialisedFlag = 1;
	DBG_Init();
	UTIL_ADV_TRACE_Init();
	UTIL_ADV_TRACE_RegisterTimeStampFunction(TimestampNow);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	UTIL_ADV_TRACE_SetVerboseLevel(VERBOSE_LEVEL);
	SYS_InitMeasurement();
	MaxBotix_Init();
	UTIL_LPM_Init();
	UTIL_LPM_SetOffMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#if defined(LOW_POWER_DISABLE) && (LOW_POWER_DISABLE == 1)
	UTIL_LPM_SetStopMode((1 << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#elif !defined(LOW_POWER_DISABLE)
#error LOW_POWER_DISABLE not defined
#endif
}

void UTIL_SEQ_Idle(void) { UTIL_LPM_EnterLowPower(); }

uint16_t getSensorReading(uint8_t mode, uint16_t sampling_rate,
                                   uint8_t number_of_samples){
	uint16_t val = Maxbotix_Read_Using_Modes(mode, sampling_rate, number_of_samples);
	return val;
}

void getBatteryLevel(void) {
	uint16_t val;
	val = SYS_GetBatteryLevel();
	APP_PRINTF((const char*)&val);
}

void GetUniqueId(uint8_t *id) {
	uint32_t val = 0;
	val = LL_FLASH_GetUDN();
	if (val == 0xFFFFFFFF) {
		uint32_t ID_1_3_val = HAL_GetUIDw0() + HAL_GetUIDw2();
		uint32_t ID_2_val = HAL_GetUIDw1();

		id[7] = (ID_1_3_val) >> 24;
		id[6] = (ID_1_3_val) >> 16;
		id[5] = (ID_1_3_val) >> 8;
		id[4] = (ID_1_3_val);
		id[3] = (ID_2_val) >> 24;
		id[2] = (ID_2_val) >> 16;
		id[1] = (ID_2_val) >> 8;
		id[0] = (ID_2_val);
	} else {
		id[7] = val & 0xFF;
		id[6] = (val >> 8) & 0xFF;
		id[5] = (val >> 16) & 0xFF;
		id[4] = (val >> 24) & 0xFF;
		val = LL_FLASH_GetDeviceID();
		id[3] = val & 0xFF;
		val = LL_FLASH_GetSTCompanyID();
		id[2] = val & 0xFF;
		id[1] = (val >> 8) & 0xFF;
		id[0] = (val >> 16) & 0xFF;
	}
}

uint32_t GetDevAddr(void) {
	uint32_t val = 0;
	val = LL_FLASH_GetUDN();
	if (val == 0xFFFFFFFF) {
		val = ((HAL_GetUIDw0()) ^ (HAL_GetUIDw1()) ^ (HAL_GetUIDw2()));
	}
	return val;
}

static void TimestampNow(uint8_t *buff, uint16_t *size) {
	SysTime_t curtime = SysTimeGet();
	tiny_snprintf_like((char *)buff, MAX_TS_SIZE,
			   "%ds%03d:", curtime.Seconds, curtime.SubSeconds);
	*size = strlen((char *)buff);
}

void UTIL_ADV_TRACE_PreSendHook(void) {
	UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_DISABLE);
}

void UTIL_ADV_TRACE_PostSendHook(void) {
	UTIL_LPM_SetStopMode((1 << CFG_LPM_UART_TX_Id), UTIL_LPM_ENABLE);
}

static void tiny_snprintf_like(char *buf, uint32_t maxsize,
			       const char *strFormat, ...) {
	va_list vaArgs;
	va_start(vaArgs, strFormat);
	UTIL_ADV_TRACE_VSNPRINTF(buf, maxsize, strFormat, vaArgs);
	va_end(vaArgs);
}

uint32_t HAL_GetTick(void) {
	uint32_t ret = 0;
	if (SYS_TimerInitialisedFlag == 0) {
	} else {
		ret = TIMER_IF_GetTimerValue();
	}
	return ret;
}

void HAL_Delay(__IO uint32_t Delay) { TIMER_IF_DelayMs(Delay); }
