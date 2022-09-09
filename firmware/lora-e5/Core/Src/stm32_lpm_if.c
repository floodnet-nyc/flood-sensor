#include "stm32_lpm_if.h"
#include "platform.h"
#include "stm32_lpm.h"
#include "usart_if.h"


const struct UTIL_LPM_Driver_s UTIL_PowerDriver = {
    PWR_EnterSleepMode, PWR_ExitSleepMode,
    PWR_EnterStopMode,  PWR_ExitStopMode,
    PWR_EnterOffMode,   PWR_ExitOffMode,
};

void PWR_EnterOffMode(void) {}

void PWR_ExitOffMode(void) {}

void PWR_EnterStopMode(void) {
  HAL_SuspendTick();
  LL_PWR_ClearFlag_C1STOP_C1STB();
  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

void PWR_ExitStopMode(void) {
  HAL_ResumeTick();
  vcom_Resume();
}

void PWR_EnterSleepMode(void) {
  HAL_SuspendTick();
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void PWR_ExitSleepMode(void) { HAL_ResumeTick(); }
