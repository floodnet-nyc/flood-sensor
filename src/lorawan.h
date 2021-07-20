#ifndef LORAWAN_H
#define LORAWAN_H

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Arduino.h>

void lorawan_runloop_once(void);
void setup_lorawan(unsigned int packet_interval); //seconds

#endif
