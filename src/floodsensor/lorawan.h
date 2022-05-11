#ifndef LORAWAN_H
#define LORAWAN_H

#pragma once

#include "LoRaWan_APP.h"
#include <Arduino.h>


#ifndef INNERWDT_H
#ifdef __asr650x__
#include "innerWdt.h"
#endif
#endif



void lorawan_runloop_once(void);
void setup_lorawan(unsigned int packet_interval); //seconds

#endif
