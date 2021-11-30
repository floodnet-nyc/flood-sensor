#ifndef SENSORCFG_H
#define SENSORCFG_H

#pragma once

// ------------------- Sensors -------------------------

//#define USE_MAXBOTIX
#define USE_RG15

// ------------------   Pins   -------------------------


// ----------------   Constants   ----------------------


// ----------------   Variables   ----------------------

// Duty cycle
extern uint16_t TX_INTERVAL; // LoRaWAN uplink duty-cycle in seconds


/*    Sensor Operation Modes:
 Mode 1: Mean
 Mode 2: Median
 Mode 3: Mode
 */
extern unsigned int SENSOR_MODE;

// Time between the readings
extern unsigned int SENSOR_SAMPLING_RATE;

// Number of readings for a given sensor mode
extern unsigned int SENSOR_NUMBER_OF_READINGS;

// Rg-15 Acc reset
extern uint32_t MAX_COUNTER_RG15;

extern String RG15_OP_MODE; //allowed: "P", "Polling", "C", "Continuous"

// ------------------- Error Flags ----------------------

#endif
