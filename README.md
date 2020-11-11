# Flood Sense Sensor Library
This repository contains the source code for the Floodsense sensor which uses ultrasonic sensor technology to detect floods and send the data over LoRa using LoRaWAN protocol. [Here](https://github.com/floodsense/sensor_experiments) is another repo containing sensor experiments, analysis and additional support related to this library.

**Table of Contents:**

   * [Flood Sense Sensor Library](#flood-sense-sensor-library)
      * [About this Project](#about-this-project)
      * [Using this Library](#using-this-library)
        * [Sensor Config File](#sensor-config-file)
      * [LoRa](#lora)
        * [Uplink Packet Format](#uplink-packet-format)
        * [Downlink Packet Format](#downlink-packet-format)
        * [Changing parameters via Downlink](#changing-parameters-via-downlink)
          * [Changing dutycycle](#changing-dutycycle)
          * [Changing sensor mode](#changing-sensor-mode)
          * [Changing sensor sampling rate](#changing-sensor-sampling-rate)
          * [Changing number of sensor readings per measurement](#changing-number-of-sensor-readings-per-measurement)
        * [TTN Payload Decoder](#ttn-payload-decoder)
        * [TTN Credentials](#ttn-credentials)
      * [Ultrasonic Sensor](#ultrasonic-sensor)
        * [Ultrasonic Sensor Configuration](#ultrasonic-sensor-configuration)
      * [SD Card logging and RTC](#sd-card-logging-and-rtc)


## About this project
One of the goals of the Flood Sense project is to develop a flood sensor that overcomes common sensor challenges, as well as the digital infrastructure necessary to log, process, and present the data in combination with other publicly available information, such as rainfall data, 311 flooding complaints, and social media feeds. More about this project can be found [here](https://wp.nyu.edu/urbanflooding/flood-sense/).

## Using this Library

### Sensor Config file
The Sensor Config file, `sensorcfg.h` contains important pin definitions, flags and parameters that can be changed during runtime. However some parameters are fixed for a given architecture and shouldn't be modified during runtime such as Pins, Card and Chip Selects etc.

Here below is a list of parameters that can be changed during the run-time of the sensor:
- Duty Cycle
- Ultrasonic Sensing Modes

## LoRa
This library uses the [Arduino-lmic](https://github.com/mcci-catena/arduino-lmic) library to handle the LoRa communication. The Things Network has a LoRaWAN [compliance](https://www.thethingsnetwork.org/docs/lorawan/duty-cycle.html). This means every radio device must be compliant with the regulated duty cycle limits. To program the node to stay within the limits, an [air-time calculator](https://avbentem.github.io/airtime-calculator/ttn/us915) can be used.

### Uplink Packet Format

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


### Downlink Packet Format

|Duty Cycle in seconds | Sensor Mode  |  Sampling Rate| Number of readings per measurement |
|----------------------|--------------|---------------|  --------------------------------- |
| 2 bytes              |   1 byte     |    2 bytes     |          1 byte                    |

### Changing parameters via Downlink
To change the duty cycle and sensor parameters via downlink the above downlink packet format must be used. For this use case the maximum packet size is 6 bytes and can be varied. The parameters to be changed should be a non-zero entries and the rest can be left as zeroes if no changes are needed. The following sections describe changing duty cycle and sensor configurations with some examples.

#### Changing dutycycle
Uplink frequency at the end node is determined by the TX_INTERVAL. This TX_INTERVAL is used as a variable in this library to add the functionality of varying duty cycle. Few examples of changing duty cycle by updating `TX_INTERVAL` are mentioned below:

| Downlink Packet format| Explanation|
|---|---|
| `00 3C`| duty cycle = 60 seconds and 2 bytes are used for the downlink|
| `3C` |duty cycle = 60 seconds and lowest payload size |
| `01 68`| duty cycle = 360 seconds and 2 bytes are used for the downlink|
|`00 3C 00 00 00 00` | Only duty cycle is changed and rest are unchanged. 6 bytes used for the downlink |


#### Changing sensor mode
The sensor mode can be changed by adding non-zero values to the sensor mode byte in the downlink packet. Any invalid modes entered are discarded. Examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 00 01` | changing sensor mode to 1. Consumes 3 bytes: minimal mode change|
| `00 00 01 00 00 00` |changing sensor mode to 1 and the rest are unchanged. Consumes 6 bytes |
| `00 00 06 00 00 00` |Discarded and no change since 6 is an invalid mode. Consumes 6 bytes |
| `01`| Valid packet format but doesn't change sensor mode, instead duty cycle is changed to 1 sec|

#### Changing sampling rate
The sensor sampling rate can be changed by adding non-zero values to the corresponding bytes in the downlink packet. Examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 00 00 00 FA` | changing sensor sampling rate to 250ms. Consumes 5 bytes: minimal sampling rate change|
| `00 00 00 04 E2 00` |changing sensor sampling rate to 1250ms and the rest are unchanged. Consumes 6 bytes |
| `04 E2`| Valid packet format but doesn't change sensor sampling rate, instead duty cycle is changed to 1250 sec|

#### Changing number of sensor readings per measurement
Similarly, the number of sensor readings per measurement can be changed by adding non-zero values to the corresponding bytes in the downlink packet. A maximum of 20 is allowed. Examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 00 00 00 00 05` | 5 readings per measurement. Consumes 5 bytes: minimal sampling rate change|
| `00 00 00 00 00 22` | invalid, maximum of 20 readings are allowed and the configuration is unchanged.|

#### Changing multiple parameters via single downlink:
Multiple sensor parameters can be changed via downlink and below are such examples:

| Downlink Packet format| Explanation|
|---|---|
| `00 78 02 00 00 05` | Change duty cycle to 120 seconds, sensor mode to 2 (Median) and 5 readings per measurement|
| `00 78 03 00 FA 12` | Change duty cycle to 120 seconds, sensor mode to 1(Mode), 250ms sampling rate and 18 readings per measurement|

**Note:** the above downlink payload formats must be implemented with caution, else there is a danger of sleeping the MCU for unwanted periods of time or indefinitely or even a possible crash!  

### TTN Payload Decoder
TTN Payload decoder format can be found in the `ttnPayloadDecoder.js` file
### TTN Credentials
The TTN Credentials should be entered into the `ttncredentials.h` file from the TTN console.
## Ultrasonic Sensor
Ultrasonic sensors measure the distance to an object by measuring the time difference between sending a signal and receiving an echo from the object. The Ultrasonic sensor used for this project is [Maxbotix's MB-7389](https://www.maxbotix.com/Ultrasonic_Sensors/MB7389.htm).

### Ultrasonic sensor Configuration:
The Ultrasonic sensor has an internal averaging methods to avoid small obstacles, irregularities and reporting the distance to the largest acoustic return while ignoring smaller targets. However, some anomalies are observed due to corner reflections and metal surfaces. These anomalies can be avoided using statistical methods such as averaging or calculating median of multiple measurements.

#### Sensor modes:
These statistical methods are implemented as `sensorModes`. For example, `sensorMode = 2` uses a median of all the readings. The following are the available sensor modes:

|Sensor mode| Statistical method|
|---|---|
|  1 | Mean  |
|  2 | Median  |
|  3 | Mode  |

#### Number of Readings:
The number of readings for these averaging methods can be changed via variable `sensor_numberOfReadings`.

#### Sensor Sampling Frequency:
Single read from the sensor is as fast as 150 to 250 milliseconds and five of such measurements would be sensed over a period of 0.75 - 1.5 seconds. These measurements can be spread out over a larger interval by changing the variable `sensor_sampling_rate`, which enables to use averaging methods with varying rates. Each measurement corresponding to a sensor mode is followed by an interval of `sensor_sampling_rate` milliseconds.

## SD Card logging and RTC
Every lora event, sensor states and measurements are locally logged onto the SD Card using the [SDFat](https://github.com/greiman/SdFat) library. The sensor sets the SD Card error flag high if the local logging is failed. A Real-time Clock is used to create timestamps for these local logs.
