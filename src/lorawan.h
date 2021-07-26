#ifndef LORAWAN_H
#define LORAWAN_H

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Arduino.h>
#include "ttncredentials.h"


// This EUI must be in little-endian format
static u1_t PROGMEM APPEUI[8] = TTN_APPEUI ;
// This should also be in little endian format
static const u1_t PROGMEM DEVEUI[8] = TTN_DEVEUI ;
// This key should be in big endian format
static const u1_t PROGMEM APPKEY[16] = TTN_APPKEY ;
static osjob_t initjob;
static bool UPDATE_CONFIG = true;
static unsigned char cfg_packet[7];      // CFG Uplink Packet
static unsigned char lora_packet[5];     // Regular Uplink Packet     // Set to true at start and when there is a change in sensor cfg; used to send sensor cfg via uplink
void execRuntimeJobs(void);
 void setupLMIC();
void prepare_lora_packet(void);
void prepare_cfg_packet(void);
bool sendUplink(unsigned char txPacket[]);
#endif
