# Hardware Quality Control Manual

**Table of Contents:**
* [Introduction](#introduction)
* [General Laboratory Safety](#general-laboratory-safety)
* [Components, equipment and other tools](#components,-equipment-and-other-tools)

## Introduction
This manual details the sensor quality control at FloodSense. It is meant to be a teaching tool and source of information to build a high quality sensor.

One of the hardware goals of FloodSense is to devise a community-buildable sensor design that meets industry standards - IP67 Waterproof, Weatherproof and is robust.

The objectives of this manual are:
- to provide a reference QC manual to quality inspectors
- to provide a reference QC manual to lab personnel/researchers/community members
- to reduce human error and improve the quality of the sensor by
  - providing detailed steps in order which are easy to follow
  - proper building instructions
  - notes on areas where things might go wrong

## General Laboratory Safety

1. Always use safety goggles while performing drilling or sawing or any other activities where there is a potential danger to the eyes
2. Use care when waterproofing electrical components using liquid electrical tape. Thoroughly wash hands after use.
3. Use care when handling a solder iron. Never touch the element of soldering iron! Please use a cleaning wet sponge during use. Always put the soldering iron on the stand when not in use and never put it on the workbench. Turn it off and unplug when not in use.
4. Do not use a NiMH/NiCad/lead-acid charger while charging LiPo batteries
5. Always inspect batteries and surrounding circuitry for any damage, loose wiring, or possibility of short circuits

## Build Quality
The following practices shall be followed while building the sensor to ensure a good build quality.
1. Waterproofing
2. Loose Wiring
3. Short-circuits
4. Soldering
5. Wire connection practices

## Sensor Build Instructions with Quality control and Testing
The instructions below are listed in order and shall be followed in the same order to maintain QC pipeline and enable efficient testing and easier debugging. These steps when followed are aimed to help inspectors/researchers/community members attain a good build-quality.

**A. Initial steps and preparation**

1. <em>Components check:</em> All the components needed to build a sensor are listed below.

| Component name                 | Count                |
|--------------------------------|----------------------|
| MCU - Adafruit Feather M0 LoRa | 1                    |
| Maxbotix Ultrasonic sensor     | 1                    |
| Adafruit SD Feather Wing       | 1                    |
| 3V Lithium cell battery        | 1                    |
| SD Card                        | 1                    |
| Connector board                | 1                    |
| Solid wire for antenna         | 1 (atleast 4 inches) |
| Mounting headers and screws    | 4 pairs              |
| Solar Charger board            | 1                    |
| Capacitor - 10V rating         | 1                    |
| Solar Panel                    | 1                    |
| Lithium ion Battery - 2200mAh  | 1                    |
| Alarm Cable                    | 1 (atleast 2ft)      |
| Heat shrinks                   | na                   |
| loose wires                    | na                   |

2. <em>Tools check:</em> The following are lists of tools categorized into must-haves and nice-to-haves.

| Must-haves               |
|--------------------------|
| Solder iron              |   
| Drill and step drill bit |   
| Screw driver             |  
| Pliers                   |   


| Nice-to-haves  |
|----------------|
| Heat Gun       |
| Wire strippers |
| Scissors       |
| Razor files    |
| Tweezers       |

**B. MCU preparation**

1. Setting up the MCU

2. Downloading latest firmware
3. Installing firmware on the MCU

**C. Maxbotix depth sensor preparation**

1. Preparing the Maxbotix depth sensor

2. Drilling a hole in the housing to accommodate Maxbotix depth sensor

**D. Solar Charger board preparation**

1. Capacitor

2. Wiring

**E. Solar Panel preparation**

1. Wiring

2. Waterproofing
3. Drilling a hole to accommodate the Solar Panel's cable

**F. Wiring**
1. Mounting the MCU

2. Connecting pin io1 to pin 6
3. Mounting the depth sensor
4. Connecting depth sensor to the MCU
5. Connecting Solar board with the Solar Charger board
6. Connecting the battery to the Solar Charger board
7. Connecting Solar Charger board with the MCU

**G. Battery**


**H. Final Steps**

**I. Debugging**

**J. Mounting**
