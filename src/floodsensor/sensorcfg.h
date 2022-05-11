#ifndef SENSORCFG_H
#define SENSORCFG_H

#pragma once

// ------------------- Sensors -------------------------

//#define USE_MAXBOTIX
// #define USE_RG15
#define USE_TIPPING_BUCKET

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

// Rg-15 Acc reset
extern uint32_t MAX_COUNTER_RG15;

extern String RG15_OP_MODE; //allowed: "P", "Polling", "C", "Continuous"

// ------------------- Error Flags ----------------------

#endif