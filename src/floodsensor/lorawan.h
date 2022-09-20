#ifndef LORAWAN_H
#define LORAWAN_H

#pragma once

#include "LoRaWan_APP.h"
#include <Arduino.h>



void lorawan_runloop_once(void);
void setup_lorawan(unsigned int packet_interval); //seconds

#endif
