#ifndef LORAWAN_H
#define LORAWAN_H

#pragma once

#include <Arduino.h>
#include "LoRaWan_APP.h"
#include "device_secrets.h"

void lorawan_runloop_once(void);
void setup_lorawan(unsigned int packet_interval); //seconds

/* Pre-processor derivatives to enable EASY_DEP_MODE or 
 * ADV_DEP_MODE.
 * 	
 * 	ADV_DEP_MODE
 * 		- Application change can be triggered over 
 * 		  the air from the TTN console
 * 		- By default the sensor is staged in the 
 * 		  testing/lab application and is moved over
 * 		  to the live application up on deployment. 
 * 		- Default mode is config mode when not deployed
 * 		- Default mode is sensing mode after deployed 
 * 		- To deploy switch to live app and start sensing.
 *
 * 	EASY_DEP_MODE
 * 		- All the above advanced features are disabled
 * 		- No app change or cfg modes
 * 		- Turning on the sensor starts the sensor in sensing
 * 		  mode, and goes live immediately
 * */
#define EASY_DEP_MODE 1
//#define ADV_DEP_MODE 1


#endif
