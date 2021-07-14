1. waht are the checks a builder needs to check while building
2. QC checks - Functionality(data pushing, working?), accuracy(fine grained -> desired accuracy -> +- half an inch) 

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

### :white_check_mark: Waterproofing

Wire insulation is quite resistant to water. However, electrical connection is not. Every electrical connection that is outside the sensor housing shall be water proofed.


### :white_check_mark: No Loose Wirings

### :white_check_mark: No Short-circuits

### :white_check_mark: Good Soldering Practices

#### Soldering basics

:warning:_Caution:_ Never touch the tip of the solder iron, or the iron stand!! (often referred to as a cradle)

:heavy_exclamation_mark:_Potential Hazard:_ Leaving the solder iron unattended is a potential hazard: it could burn you or even burn your desk and start a fire. Always dock the solder iron in its stand and turn off after use.

First, plug and turn on the soldering iron to warm up. While the solder is warming up wet the sponge.

Apply a small amount of solder to the tip and then clean the tip using the wet sponge. You should have a shiny clean tip with a layer of molten solder on the tip of your iron.

Solder is a "joint" and to make a good electrical connection make sure that the through holes are clean. Oxidation and dirt can decrease the quality of the joints. Use a little isopropyl alcohol with a tissue/paper towel to clean the board.

Heat the through hole by placing the tip so that it is in contact with both the metal contact and the header pin.

:warning:_Caution:_ Be careful not to apply lot of heat to the board as it can damage the board.

Carefully put the solder on the joint either on the pin or the metal through hole, and it should melt into the through hole making a connection.

A good quality connection looks like the picture below:

#### Common soldering problems

The following are some common problems encountered while soldering that makes an electric connection low quality. These shall be identified and rectified through out the sensor's hardware build procedure.

##### :x: Too much solder!
Applying too much solder onto a pin causes excessive buildup that looks like a rounded ball or a raised shape.

##### :x: Not enough solder/ Insufficient wetting (Pad)
Sometimes the solder wets the leads nicely but does not form a good connection with the pad. This can be caused by failing to apply heat to the pad as well as the pin while making the connection.

##### :x: Solder balling / Solder splashes
If not careful, excess solder while soldering

##### :x: Cold joint
Cold joints are the ones where the solder did not melt completely. These joints often are lumpy and look like they are falling off. These type of joints are unreliable over time.

##### :x: Overheated joint
Applying excessive heat at the joint may lift the pad or burn the board.

##### :x: Solder bridge
Excess solder on the pins or bent leads can cause unintended connections like shown below

### :white_check_mark: Good Wire connection Practices

## Quality Assurance Procedure - with sensor testing and debugging
The instructions below are listed in order and shall be followed in the same order to maintain QC pipeline and enable efficient testing and easier debugging. These steps when followed are aimed to help inspectors/researchers/community members attain a good build-quality.

### A. Initial steps and preparation

#### 1. Components check
All the components needed to build a sensor are listed below.

| Component name                 | Count                |
|--------------------------------|----------------------|
| MCU - Adafruit Feather M0 LoRa | 1                    |
| Maxbotix Ultrasonic sensor     | 1                    |
| Adafruit SD Feather Wing       | 1                    |
| 3V Lithium cell battery        | 1                    |
| SD Card                        | 1                    |
| Male Headers Equal Length Long Centered 6.25mm on Both Sides, 15mm in total| 2|
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

#### 2. Tools check
The following are lists of tools categorized into must-haves and nice-to-haves.

| Must-haves               |
|--------------------------|
| Solder iron              |
| Solder               |   
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

### B. MCU preparation

#### 1. Setting up the MCU

The microcontroller unit(MCU) is the Adafruit Feather M0 RFM95 LoRa Radio - 900 MHz. Out of the box, the MCU comes with "through holes" or "PTH" or simply referred as pins. The left and right sides of the MCU has 16 pins and 12 pins respectively and cut the long male headers accordingly. Insert these long male headers into a breadboard as shown in the figure.  

Now mount the MCU onto these stacking header pins and make sure the board sits comfortably without any gap. When done correctly, the MCU looks perfectly horizontal to the breadboard from the side. Note that the MCU is upside down since the SD Feather Wing goes on top of MCU.

##### 1.1 Soldering MCU

Now solder the header pins onto the MCU while following [good soldering practices](#good-soldering-practices).

The MCU after this step should look like the following figure, where the pins are longer on the top to accommodate the SD Feather wing.

##### 1.2 Soldering SD Feather Wing

Now carefully mount the SD feather wing onto these pins as shown in the figure. Just to note, the Feather M0 LoRa board and SD Feather Wing have same number of through holes and there must be no through holes left unmounted/soldered after this step.

Now solder from the top as shown in the figure below.

:warning: Using a multimeter, any potential short circuits must be checked.

##### 1.3 Installing SD card and Coin cell battery onto SD Feather Wing
After the completion of above steps, carefully insert the coin cell with positive side up into the feather wing as shown in the figure.

Now insert SD card into the SD card slot and must hear a click when safely secured.

##### 1.4 MCU Antenna
The Feather m0 LoRa module does not come with a built-in antenna. A simple single core standard wire can be used as an antenna when cut down to right lengths according to the desired frequencies in the below table. This project is located in Brooklyn, United States, the frequency used is 915 MHz.

| Frequency     | Length (inches) | Length (cm) |
|---------------|-----------------|-------------|
| 433 MHz       | 6.5             | 16.5        |
| 868 MHz       | 3.25            | 8.2         |
| 915 MHz       | 3               | 7.8 cm      |

Cut the single core wire to 3 inches and strip a mm or two just on one end. Now, tin and solder into the ANT pad on the very right hand edge of the Feather like shown below.

##### 1.5 Mounting the MCU
The MCU shall be mounted onto the terminal block breakout board as shown below.

:white_check_mark:The MCU must be secured and feel tightly held onto the breakout board.

:warning:If the MCU pins do not completely fit into the female headers, excess solder on few pins might cause this issue. To fix it, remove the excess solder and male sure solder on the pins are clean and consistent.

#### 2. Installing the latest firmware on the MCU


### **C. Maxbotix Ultrasonic Sensor Preparation**

##### 1. Preparing the Maxbotix depth sensor

The GND, VCC, Pin 4 and Pin 5 are used for this application. Now choose 4 wires and prepare them for soldering. Tin all the wires on both sides like shown below

Tinning ensures all the fine wires are held together and making an electrical connection. If the wire is not tinned, the individual fine wires may cause short circuits/wrong connections.

Solder them carefully onto the required pins while following [good soldering practices](#good-soldering-practices).

Carefully holding the wires together with one hand, twist the wires using the other hand. Check for any loose connections or broken wire ends.

##### 2. Drilling a hole in the housing to accommodate Maxbotix depth sensor


### **D. Solar Charger board preparation**

##### 1. Soldering the Capacitor
The negative pin of the capacitor is indicated by a "-" and/or a colored strip along the can. They might also have a longer positive leg.

Now place the capacitor legs into the corresponding through holes on the solar charger board and make sure there is enough room for it to bend sideways to save space as shown in the figure.

Now solder the capacitor and cut off the excess legs. Make sure these are discarded and not left out inside the sensor as they could short other components.

##### 2. Wiring
Tin two wires and solder them onto the pins B+ and GND as shown below.

### **E. Solar Panel preparation**

##### 1. Waterproofing the panel connection
The backside of the solar panel has positive and negative terminals. Use a high quality alarm cable and cut open the ends like shown below.

Strip the edges of the wire and tin them

Now heat shrink both the small wires as shown

Solder them onto the corresponding pins and do not apply heat for too long. This might damage and lift the pad on the solar panel.

Now use a bigger heat shrink to hold the both wires like shown below

Waterproof the electrical contact by using liquid electrical tape. Make sure it covers all around making a good seal.

Let it dry for a few minutes and a good quality seal and connection would like below

The following are the examples of a poor quality seal and connection

##### 2. Drilling a hole to accommodate the Solar Panel's cable

### **F. Wiring**


1. Connecting pin io1 to pin 6
2. Mounting the depth sensor
3. Connecting depth sensor to the MCU
4. Connecting Solar board with the Solar Charger board
5. Connecting the battery to the Solar Charger board
6. Connecting Solar Charger board with the MCU

### **G. Battery**
Plug in the battery into the Batt connector on the solar board charger.

### **H. Final Steps**

### **I. Troubleshoots**

### **J. Mounting**
