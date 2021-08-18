#ifndef MAXBOTIX_H
#define MAXBOTIX_H

#include "Arduino.h"
#include "softSerial.h"

#define readPin GPIO0       /* RX on new PCB - v2 (TX on old PCB - v1) is connected to Pin 5 of US*/
#define triggerPin GPIO5    /* TX on new PCB - v2 (RX on old PCB - v1) is connected to Pin 4 of US*/



void setup_maxbotix(unsigned int mode, unsigned int sampling_rate , unsigned int numberOfReadings);
uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate, unsigned int sensor_numberOfReadings);
uint16_t sensor_singleread(void);

#endif
