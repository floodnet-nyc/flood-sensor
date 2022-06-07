
# Installation and Usage



- [Installation and Usage](#installation-and-usage)
  - [Installing necessary softwares and libraries](#installing-necessary-softwares-and-libraries)
    - [Arduino IDE](#arduino-ide)
    - [Flood Sensor Library](#flood-sensor-library)
    - [Installing CubeCell Library](#installing-cubecell-library)
  - [Registering the device to The Things Network](#registering-the-device-to-the-things-network)
    - [Creating an end-device](#creating-an-end-device)
    - [Installing the device secrets](#installing-the-device-secrets)
  - [Compiling and Uploading the firmware to the sensor](#compiling-and-uploading-the-firmware-to-the-sensor)
- [Usage](#usage)
  - [Modifying Sensor Configuration using Downlinks](#modifying-sensor-configuration-using-downlinks)
    - [Changing dutycycle](#changing-dutycycle)
    - [Changing sensor mode](#changing-sensor-mode)
    - [Changing sampling rate](#changing-sampling-rate)
    - [Changing number of sensor readings per measurement](#changing-number-of-sensor-readings-per-measurement)
    - [Changing multiple parameters via single downlink](#changing-multiple-parameters-via-single-downlink)
    - [Changing sensor state](#changing-sensor-state)
  - [Understanding the uplink message payload](#understanding-the-uplink-message-payload)
    - [Uplink Packet Format](#uplink-packet-format)


## Installing necessary softwares and libraries

### Arduino IDE
For simplicity, Arduino IDE has been used in the documentation but other IDEs can also be used. Arduino IDE, can be downloaded from [Arduino's website](https://www.arduino.cc/en/software). The Arduino IDE is used for uploading the firmware to the microcontroller.

### Flood Sensor Library
Download the Flood Sensor Library from [here](https://github.com/floodnet-nyc/flood-sensor). Following are the steps to install this library:

1. Find Arduino libraries folder
2. Clone the library and copy the `/firmware` folder into the Arduino>libraries

### Installing CubeCell Library
This repo is built to run on CubeCell's example library. To install the CubeCell's library, follow the instructions on their repo [here](https://github.com/HelTecAutomation/CubeCell-Arduino/tree/master/InstallGuide)

## Registering the device to The Things Network

### Creating an end-device

### Installing the device secrets

## Compiling and Uploading the firmware to the sensor


# Usage

## Modifying Sensor Configuration using Downlinks

Three types of downlinks are available, categorized based on functionality.

**Sensor downlink format:**

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

## Understanding the uplink message payload

### Uplink Packet Format

| Error flags  | Battery Level | Ultrasonic reading  |
|--------------|---------------|---------------------|
|   1 byte     |    2 bytes    |        2 bytes      |

**Cfg packet format:**

| Error flags  | Sensor Mode | Sensor Sampling Rate | Sensor Number of Readings |
|--------------|---------------|---------------------|---------------------|
|   1 byte : 255 or 0xFF     |    1 byte   |      2 bytes         |        1 bytes            |

**Error Flags:**

|     bit 7    |     bit 6   |     bit 5    |     bit 4    |     bit 3    |     bit 2    |     bit 1    |     bit 0    |
|   ----       |   ----      |     ----     |     ---      |      ---     |      ---     |       ---    |      ---     |
|      Used only for CFG update (all other bits are high)         |             |              |              |              |              |              | SD error flag|

^empty flags above - bits 1-6 are for future use.
