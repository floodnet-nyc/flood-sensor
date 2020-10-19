#ifndef SENSORCFG_H
#define SENSORCFG_H

#pragma once

// ------------------   Pins   -------------------------

#define VBATPIN A7
#define cardSelect 10

// ----------------   Constants   ----------------------

// SD chip select pin
#define chipSelect 10

// ----------------   Variables   ----------------------

// Duty cycle
extern unsigned int TX_INTERVAL;

/*  Sensor Operation Modes:
      Mode 1: Single reading
      Mode 2:

*/
extern unsigned int sensorMode;

// Currently sensor sampling rate is minimum = 20 microseconds
extern unsigned int sensor_sampling_rate;

// Number of readings for a sensor mode
extern unsigned int sensor_numberOfReadings;

// ------------------- Error Flags ----------------------

extern int SD_ERROR;

#endif
