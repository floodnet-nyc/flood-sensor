#ifndef __ADC_IF_H__
#define __ADC_IF_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "adc.h"
#include "platform.h"

void SYS_InitMeasurement(void);
void SYS_DeInitMeasurement(void);
uint16_t SYS_GetBatteryLevel(void);


#ifdef __cplusplus
}
#endif

#endif
