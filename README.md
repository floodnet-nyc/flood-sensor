# Flood Sense Sensor Library
This repository contains the source code for the Floodsense sensor which uses ultrasonic sensor technology to detect floods and send the data over LoRa using LoRaWAN protocol.

**Table of Contents:**

   * [Flood Sense Sensor Library](#flood-sense-sensor-library)
      * [About this Project](#about-this-project)
   * [Using this Library](#using-this-library)
      * [Sensor Config File](#sensor-config-file)
      * [LoRa](#lora)
        * [LoRa Configuration](#lora-configuration)
          * [Duty Cycle](#duty-cycle)
        * [TTN Payload Decoder](#ttn-payload-decoder)
        * [TTN Credentials](#ttn-credentials)
      * [Ultrasonic Sensor Modes](#ultrasonic-sensor-modes)
      * [SD Card logging and RTC](#sd-card-logging-and-rtc)


## About this project
One of the goals of the Flood Sense project is to develop a flood sensor that overcomes common sensor challenges, as well as the digital infrastructure necessary to log, process, and present the data in combination with other publicly available information, such as rainfall data, 311 flooding complaints, and social media feeds. More about this project can be found [here](https://wp.nyu.edu/urbanflooding/flood-sense/).

## Using this Library

### Sensor Config file
The Sensor Config file, `sensorcfg.h` contains important pin definitions, flags and parameters that can be changed during runtime. However some parameters are fixed for a given architecture and shouldn't be modified during runtime such as Pins, Card and Chip Selects etc.

Here below is a list of parameters that can be changed during the run-time of the sensor:
- Duty Cycle
- Ultrasonic Sensing Modes

### LoRa
This library uses the [Arduino-lmic](https://github.com/mcci-catena/arduino-lmic) library to handle the LoRa communication. The Things Network has a LoRaWAN [compliance](https://www.thethingsnetwork.org/docs/lorawan/duty-cycle.html). This means every radio device must be compliant with the regulated duty cycle limits. To program the node to stay within the limits, an [air-time calculator](https://avbentem.github.io/airtime-calculator/ttn/us915) can be used.

#### LoRa Configuration
Uplink frequency at the end node is determined by the TX_INTERVAL. This TX_INTERVAL is used as a variable in this library to add the functionality of varying duty cycle.

#### TTN Payload Decoder

```
// decoder variable contains battery and distance
function Decoder(b, port) {

  var decoded = {};

  // distance
  var distance = b[1]<<8|b[0];
  decoded.distance = distance;

  // battery
  var battery = b[3]<<8|b[2];       // battery in centi Volts
  battery = battery/1000;    // Convert to Volts

  decoded.battery = battery;

  // Sensor mode
  var mode = b[4];
  decoded.mode = mode;

  // SD Card Error flag
  var sdError = b[5];
  decoded.sdError = sdError;

  return decoded;
}
```

### Ultrasonic Sensor
Ultrasonic sensors measure the distance to an object by measuring the time difference between sending a signal and receiving an echo from the object. The Ultrasonic sensor used for this project is [Maxbotix's MB-7389](https://www.maxbotix.com/Ultrasonic_Sensors/MB7389.htm).

##### Ultrasonic sensor Configuration:
The Ultrasonic sensor has an internal averaging methods to avoid small obstacles, irregularities and reporting the distance to the largest acoustic return while ignoring smaller targets. However, some anomalies are observed due to corner reflections and metal surfaces. These anomalies can be avoided using statistical methods such as averaging or calculating median of multiple measurements.

##### Sensor modes:
These statistical methods are implemented as `sensorModes`. For example, `sensorMode 5` is a median of 5 measurements.

##### Number of Readings:
The number of readings for these averaging methods can be changed via variable `sensor_numberOfReadings`.

##### Sensor Sampling Frequency:
Single read from the sensor is as fast as 150 to 250 milliseconds and five of such measurements would be sensed over a period of 0.75 - 1.5 seconds. These measurements can be spread out to average over a larger interval using the variable `sensor_sampling_rate`. Each measurement corresponding to a sensor mode is followed by an interval of `sensor_sampling_rate`.

### SD Card logging and RTC
Every lora event, sensor states and measurements are locally logged onto the SD Card using the [SDFat](https://github.com/greiman/SdFat) library. The sensor sets the SD Card error flag high if the local logging is failed. A Real-time Clock is used to create timestamps for these local logs. 
