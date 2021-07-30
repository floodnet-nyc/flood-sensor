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

// ----------------   FeatherWing   ----------------------
// #define LOGGER //Uncomment if FeatherWing is connected

// Duty cycle
extern unsigned int TX_INTERVAL;

/*    Sensor Operation Modes:
            Mode 1: Mean
            Mode 2: Median
            Mode 3: Mode
*/
extern unsigned int sensorMode;

// Time between the readings
extern unsigned int sensor_sampling_rate;

// Number of readings for a given sensor mode
extern unsigned int sensor_numberOfReadings;

// ------------------- Error Flags ----------------------

extern int SD_ERROR;

#endif
