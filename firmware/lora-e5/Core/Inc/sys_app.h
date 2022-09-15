#ifndef __SYS_APP_H__
#define __SYS_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stm32_adv_trace.h"
#include "sys_conf.h"

#define APP_PPRINTF(...)                                                       \
  do {                                                                         \
  } while (UTIL_ADV_TRACE_OK !=                                                \
           UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF,         \
                                     __VA_ARGS__)) /* Polling Mode */
#define APP_TPRINTF(...)                                                       \
  do {                                                                         \
    {                                                                          \
      UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_ON, __VA_ARGS__); \
    }                                                                          \
  } while (0);
#define APP_PRINTF(...)                                                        \
  do {                                                                         \
    {                                                                          \
      UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF,              \
                                __VA_ARGS__);                                  \
    }                                                                          \
  } while (0);

#if defined(APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
#define APP_LOG(TS, VL, ...)                                                   \
  do {                                                                         \
    { UTIL_ADV_TRACE_COND_FSend(VL, T_REG_OFF, TS, __VA_ARGS__); }             \
  } while (0);
#elif defined(APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0)
#define APP_LOG(TS, VL, ...)
#else
#error "APP_LOG_ENABLED not defined or out of range <0,1>"
#endif

void SystemApp_Init(void);
uint16_t getSensorReading(uint8_t averaging_method, uint16_t sampling_rate,
                          uint8_t number_of_samples, uint32_t timeout_mb,
                          uint8_t number_of_tries);
void GetUniqueId(uint8_t *id);
uint32_t GetDevAddr(void);

#ifdef __cplusplus
}
#endif

#endif
