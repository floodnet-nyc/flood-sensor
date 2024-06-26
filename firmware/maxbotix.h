#ifndef MAXBOTIX_H
#define MAXBOTIX_H

#include "Arduino.h"

#define triggerPin GPIO5

void setup_maxbotix(unsigned int mode, unsigned int sampling_rate , unsigned int numberOfReadings);
uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate, unsigned int sensor_numberOfReadings);
uint16_t sensor_singleread(void);

#endif
