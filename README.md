# FloodSense Sensor Technical Documentation
The latest and current deployment version of flood sensor is maintained on the [main](https://github.com/floodsense/floodsense_sensor/tree/main) branch

[This repository](https://github.com/floodsense/floodsense_sensor) contains the source code for the Floodsense sensor which uses ultrasonic sensor technology to detect floods and send the data over LoRa using LoRaWAN protocol. [Here](https://github.com/floodsense/sensor_experiments) is the experiments repo containing technical documentation, analysis and additional support related to this library.

<u>Note while using this library with CubeCell boards:</u> *For every firmware upload, clear the EEPROM by running the EEPROM example to delete the old keys. EEPROM is unaffected by firmware change and shall be cleared manually every time.*


## Introduction
Of the myriad impacts that are predicted to accompany climate change, flooding is expected to have an out-sized influence on public health, infrastructure, and mobility in urban areas. In New York City, for example, sea level rise and an increase in the occurrence of high intensity rain storms (which convey large volumes of water to drains, leading to backups and overflows) have led to a dramatic increase in flood risk, particularly in low-lying and coastal neighborhoods. The physical presence of standing water on streets and sidewalks can impede mobility and restrict access to transportation. Additionally, urban flood water contains a diverse array of contaminants, including industrial and household chemicals, fuels, and sewage.  Access to real-time information on flooding can improve resiliency and efficiency by allowing residents to identify navigable transportation routes and make informed decisions to avoid exposure to floodwater contaminants.

One of the goals of the [FloodSense](https://www.floodnet.nyc/) project is to develop a flood sensor that overcomes common sensor challenges, as well as the digital infrastructure necessary to log, process, and present the data in combination with other publicly available information, such as rainfall data, 311 flooding complaints, and social media feeds. This document specifies the sensor design, building instructions, initial deployment and schematics for technology transfer of the sensor development for the FloodSense project at 370 Jay Street, 13th Floor.

<img src="img/mountedv3.jpeg" width="480" >



## Build Instructions

Step by step build instructions are documented with images on the [documentation](https://floodsense.github.io//quality-management/sensor-assembly-qap/) website.

## Installation
All the following three sub-sections are needed to be completed in order to satisfy the requirements to run the software for this sensor. Furthermore, in order to utilize the complete functionalities of the sensor, a back-end support is needed.

#### Necessary Software
For simplicity, Arduino IDE has been used in the documentation but other IDEs can also be used. Arduino IDE, can be downloaded from [Arduino's website](https://www.arduino.cc/en/software). The Arduino IDE is used for uploading the firmware to the microcontroller.

#### FloodSense Sensor Library
Download the FloodSense Sensor Library from [here](https://github.com/floodsense/floodsense_sensor). This library contains the source code "src" folder, which is the firmware and contains all the necessary files for this sensor to work.

**Method 1:**
1. Find Arduino libraries folder
2. Clone the library folder into the Arduino>libraries

[**Method 2:**](https://www.arduino.cc/en/guide/libraries)
1. In Arduino IDE, go to: Sketch> Include Library> Add .ZIP Library..

  <img src="img/add-library.png" width="640">
<br />
2. Select the downloaded ZIP folder of this library from your computer

#### Installing CubeCell Library

This repo is built to run on CubeCell's example library. To install the CubeCell's library, follow the instructions on their repo [here](https://github.com/HelTecAutomation/CubeCell-Arduino/tree/master/InstallGuide)

## Using FloodSense Library

The default sensor state after reset is `stop`, in which the sensor sends its current configuration via uplink every 10 seconds. To start sensing, change the sensor state to `start` by sending a command via downlink. When sensing, the duty cycle is `packet_interval`, default is 60 seconds.

The following is an example from the examples folder, which demonstrates how this library functionality can be used only with few lines of code:

```cpp

#include <Floodsense_sensor.h>

void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println("Starting");

  setup_maxbotix(2, 150, 7);  // sensor mode 2(Median), 150ms sampling rate (time between readings), 7 readings per measurement
  setup_lorawan(packet_interval = 60);             // uplink frequency 60 seconds - controls duty cycle during sensor operation
}

void loop() {
  lorawan_runloop_once();
}
```

The main loop contains a single function `lorawan_runloop_once()` which runs all the time handling LoRa, Low Power management, Deep Sleep, Sensors operation, and everything else.

The `void setup()` sets up the sensor's serial communication (used for debugging), Ultrasonic sensor (`setup_maxbotix()` function), and finally lmic library handles the LoRaWAN and is setup using `lmicsetup(usigned int packet_interval)` function. Default `packet_interval` is `60s` and can be changed according by modifying this number in seconds.

#### Sensor Configuration file
The Sensor Config file, `sensorcfg.h` contains important pin definitions, flags and parameters that can be changed during runtime. However some parameters are fixed for a given architecture and shouldn't be modified during runtime such as Pins, Card and Chip Selects etc.

Here below is a list of parameters that can be changed during the run-time of the sensor:
- Duty Cycle
- Ultrasonic Sensing Modes

### LoRa

#### Uplink Packet Format

| Error flags  | Battery Level | Ultrasonic reading  |
|--------------|---------------|---------------------|
|   1 byte     |    2 bytes    |        2 bytes      |

**Cfg packet format:**

| Error flags  | Sensor Mode | Sensor Sampling Rate | Sensor Number of Readings |
|--------------|---------------|---------------------|---------------------|
|   1 byte : 255 or 0xFF     |    1 byte   |      2 bytes         |        1 bytes            |

**Error Flags(empty flags are for future use):**

|     bit 7    |     bit 6   |     bit 5    |     bit 4    |     bit 3    |     bit 2    |     bit 1    |     bit 0    |
|   ----       |   ----      |     ----     |     ---      |      ---     |      ---     |       ---    |      ---     |
|      Used only for CFG update (all other bits are high)         |             |              |              |              |              |              | SD error flag|


#### Downlink Packet Format

|Duty Cycle in seconds | Sensor Mode  |  Sampling Rate| Number of readings per measurement |
|----------------------|--------------|---------------|  --------------------------------- |
| 2 bytes              |   1 byte     |    2 bytes     |          1 byte                    |



## Modifying Sensor Configuration using Downlinks

Three types of downlinks are available, categorized based on functionality. 

**Sensor downlink command format:**

```<header> <command>```

| Header | Command            | Functionality                                                |
| ------ | ------------------ | ------------------------------------------------------------ |
| `0x4D` | Sensor Mode Change | Changes sensor aggregate, duty cycle, measuring delta, number of readings for averaging. |
| `0x41` | Application Change | Changes LoRaWAN Application                                  |
| `0x4F` | Sensor State       | `Start/Stop/Reset`. On a Reset the default sensor state is `Stop`, and it sends CFG packets as uplink. Starts sensing when changed to `Start` state. `Reset` resets the sensor using a watchdog timer. |

Further sections explain the possible commands.

### Changing dutycycle

Uplink frequency at the end node is determined by the TX_INTERVAL. This TX_INTERVAL is used as a variable in this library to add the functionality of varying duty cycle. Few examples of changing duty cycle by updating `TX_INTERVAL` are mentioned below:

| Downlink Packet format| Explanation|
|---|---|
| `00 3C`| duty cycle = 60 seconds and 2 bytes are used for the downlink|
| `3C` |duty cycle = 60 seconds and lowest payload size |
| `01 68`| duty cycle = 360 seconds and 2 bytes are used for the downlink|
|`00 3C 00 00 00 00` | Only duty cycle is changed and rest are unchanged. 6 bytes used for the downlink |


### Changing sensor mode
The sensor mode can be changed by adding non-zero values to the sensor mode byte in the downlink packet. Any invalid modes entered are discarded. Examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 00 01` | changing sensor mode to 1. Consumes 3 bytes: minimal mode change|
| `00 00 01 00 00 00` |changing sensor mode to 1 and the rest are unchanged. Consumes 6 bytes |
| `00 00 06 00 00 00` |Discarded and no change since 6 is an invalid mode. Consumes 6 bytes |
| `01`| Valid packet format but doesn't change sensor mode, instead duty cycle is changed to 1 sec|

### Changing sampling rate
The sensor sampling rate can be changed by adding non-zero values to the corresponding bytes in the downlink packet. Examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 00 00 00 FA` | changing sensor sampling rate to 250ms. Consumes 5 bytes: minimal sampling rate change|
| `00 00 00 04 E2 00` |changing sensor sampling rate to 1250ms and the rest are unchanged. Consumes 6 bytes |
| `04 E2`| Valid packet format but doesn't change sensor sampling rate, instead duty cycle is changed to 1250 sec|

### Changing number of sensor readings per measurement
Similarly, the number of sensor readings per measurement can be changed by adding non-zero values to the corresponding bytes in the downlink packet. A maximum of 20 is allowed. Examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 00 00 00 00 05` | 5 readings per measurement. Consumes 5 bytes: minimal sampling rate change|
| `00 00 00 00 00 22` | invalid, maximum of 20 readings are allowed and the configuration is unchanged.|

### Changing multiple parameters via single downlink
Multiple sensor parameters can be changed via downlink and below are such examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 78 02 00 00 05` | Change duty cycle to 120 seconds, sensor mode to 2 (Median) and 5 readings per measurement|
| `00 78 03 00 FA 12` | Change duty cycle to 120 seconds, sensor mode to 1(Mode), 250ms sampling rate and 18 readings per measurement|

**Note:** the above downlink payload formats must be implemented with caution, else there is a danger of sleeping the MCU for unwanted periods of time or indefinitely or even a possible crash!  

### Changing sensor state

| Operation | Packet format       |
| --------- | ------------------- |
| Start     | `73 74 61 72 74 0a` |
| Stop      | `73 74 6f 70`       |
| Reset     | `72 65 73 65 74`    |



## The Things Network
### The Things Network Payload Decoder
TTN Payload decoder format can be found in the `ttnPayloadDecoder.js` file
### The Things Network Credentials
The TTN Credentials should be entered into the `ttncredentials.h` file from the TTN console.

## Ultrasonic sensor
Ultrasonic sensor measures the distance to an object by measuring the time difference between sending a signal and receiving an echo from the object. The Ultrasonic sensor used for this project is [Maxbotix's MB-7389](https://www.maxbotix.com/Ultrasonic_Sensors/MB7389.htm).

The MB-7389 Ultrasonic sensor has an internal averaging methods to avoid small obstacles, irregularities and reporting the distance to the largest acoustic return while ignoring smaller targets. However, some anomalies are observed due to corner reflections and metal surfaces. These anomalies can be avoided using statistical methods such as averaging or calculating median of multiple measurements.

### Ultrasonic Sensor modes
These statistical methods are implemented as `sensorModes`. For example, `sensorMode = 2` uses a median of all the readings. The following are the available sensor modes:

|Sensor mode| Statistical method|
|---|---|
|  1 | Mean  |
|  2 | Median  |
|  3 | Mode  |

The function `read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate, unsigned int sensor_numberOfReadings)` reads the ultrasonic sensor using these sensor modes and stores into an `uint_16` array called `readings_arr` of size 20. This means the maximum number of readings that can be read for averaging are 20. Changing this array size can accommodate more readings for averaging.

`maxbotix.h` file contains the ultrasonic sensor pin definitions.

### Number of Readings
The number of readings for these averaging methods can be changed via variable `sensor_numberOfReadings`.

### Sensor Sampling Frequency
Single read from the sensor is as fast as 150 to 250 milliseconds and five of such measurements would be sensed over a period of 0.75 - 1.5 seconds. These measurements can be spread out over a larger interval by changing the variable `sensor_sampling_rate`, which enables to use averaging methods with varying rates. Each measurement corresponding to a sensor mode is followed by an interval of `sensor_sampling_rate` milliseconds.


## Appendix

### Solar Influence on Ultrasonic Sensor

<u>Effect of Temperature on Ultrasonic Sensor measurements:</u> There is a notable drift of the ultrasonic sensor readings over time due to the change in temperature. The distance is measured by calculating the time taken by the signal from pulsing to the return of its echo. Since temperature has the most effect on the speed of the sound, the following figure shows that effect in play.

The following is the ultrasonic sensor readings vs temperature over a period of 24 hours.

<img src="img/UltrasonicvsTemp_aug22nd_.png" width="1080" >
<br />

### FloodSense Library Structure and Scope
This library has modules for each component, i.e. LoRa, Ultrasonic Sensor, etc. This enables to add future modules/functionalities without the need to modify the others. The sensor configuration is stored in the file `sensorcfg.h` which contains the variables that can be changed during run-time and are shared between all the modules.

Including the `floodsense_sensor.h` by using `#include <Floodsense_sensor.h>` setups the complete library. This header file has the reference to all the other header files and any further new header files must be linked to this file.
