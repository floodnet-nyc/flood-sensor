#ifndef __UTILITIES_CONF_H__
#define __UTILITIES_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_compiler.h"
#include "stm32_mem.h"
#include "stm32_tiny_vsnprintf.h"
#include "utilities_def.h"

#define VLEVEL_OFF 0
#define VLEVEL_ALWAYS 0
#define VLEVEL_L 1
#define VLEVEL_M 2
#define VLEVEL_H 3
#define TS_OFF 0
#define TS_ON 1
#define T_REG_OFF 0

#if defined(__CC_ARM)
#define UTIL_PLACE_IN_SECTION(__x__) __attribute__((section(__x__), zero_init))
#elif defined(__ICCARM__)
#define UTIL_PLACE_IN_SECTION(__x__) __attribute__((section(__x__)))
#else
#define UTIL_PLACE_IN_SECTION(__x__) __attribute__((section(__x__)))
#endif

#undef ALIGN
#ifdef WIN32
#define ALIGN(n)
#else
#define ALIGN(n) __attribute__((aligned(n)))
#endif

#define UTILS_INIT_CRITICAL_SECTION()
#define UTILS_ENTER_CRITICAL_SECTION()                                         \
  uint32_t primask_bit = __get_PRIMASK();                                      \
  __disable_irq()
#define UTILS_EXIT_CRITICAL_SECTION() __set_PRIMASK(primask_bit)
#define UTIL_SEQ_CONF_TASK_NBR CFG_SEQ_Task_NBR
#define UTIL_SEQ_CONF_PRIO_NBR CFG_SEQ_Prio_NBR
#define UTIL_SEQ_INIT_CRITICAL_SECTION() UTILS_INIT_CRITICAL_SECTION()
#define UTIL_SEQ_ENTER_CRITICAL_SECTION() UTILS_ENTER_CRITICAL_SECTION()
#define UTIL_SEQ_EXIT_CRITICAL_SECTION() UTILS_EXIT_CRITICAL_SECTION()
#define UTIL_SEQ_MEMSET8(dest, value, size) UTIL_MEM_set_8(dest, value, size)
#define UTIL_ADV_TRACE_CONDITIONNAL
#define UTIL_ADV_TRACE_UNCHUNK_MODE
#define UTIL_ADV_TRACE_DEBUG(...)
#define UTIL_ADV_TRACE_INIT_CRITICAL_SECTION() UTILS_INIT_CRITICAL_SECTION()
#define UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION() UTILS_ENTER_CRITICAL_SECTION()
#define UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION() UTILS_EXIT_CRITICAL_SECTION()
#define UTIL_ADV_TRACE_TMP_BUF_SIZE (512U)
#define UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE (15U)
#define UTIL_ADV_TRACE_FIFO_SIZE (1024U)
#define UTIL_ADV_TRACE_MEMSET8(dest, value, size)                              \
  UTIL_MEM_set_8((dest), (value), (size))
#define UTIL_ADV_TRACE_VSNPRINTF(...) tiny_vsnprintf_like(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
