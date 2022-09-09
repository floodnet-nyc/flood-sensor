#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"
#include "sys_conf.h"

#if !defined(DISABLE_PROBE_GPIO)
#define PROBE_GPIO_WRITE(gpio, n, x)                                           \
  HAL_GPIO_WritePin(gpio, n, (GPIO_PinState)(x))
#define PROBE_GPIO_SET_LINE(gpio, n) LL_GPIO_SetOutputPin(gpio, n)
#define PROBE_GPIO_RST_LINE(gpio, n) LL_GPIO_ResetOutputPin(gpio, n)
#else
#define PROBE_GPIO_WRITE(gpio, n, x)
#define PROBE_GPIO_SET_LINE(gpio, n)
#define PROBE_GPIO_RST_LINE(gpio, n)
#endif


void DBG_Init(void);

#ifdef __cplusplus
}
#endif

#endif
