#ifndef SENSORCFG_H
#define SENSORCFG_H

#pragma once

// ------------------   Pins   -------------------------


// ----------------   Constants   ----------------------


// ----------------   Variables   ----------------------

// Duty cycle
extern uint32_t appTxDutyCycle;

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

#endif
