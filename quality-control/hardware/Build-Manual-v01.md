# Sensor Build Manual

The sensor build instructions are listed below in order and shall be followed in the same order to maintain QC pipeline, enable efficient testing, and easier debugging. These steps when followed are aimed to help inspectors/researchers/community members attain a good build-quality.

## A. Initial steps and preparation

### 1. Components check
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
| Solid core wire for antenna    | 1 (atleast 4 inches) |
| Mounting headers and screws    | 4 pairs              |
| Solar Charger board            | 1                    |
| Capacitor - 10V rating         | 1                    |
| Solar Panel                    | 1                    |
| Lithium ion Battery - 2200mAh  | 1                    |
| Alarm Cable                    | 1 (atleast 2ft)      |
| Heat shrinks                   | na                   |
| loose wires                    | na                   |

### 2. Tools check
The following are lists of tools categorized into must-haves and nice-to-haves.

| Must-haves               |
|--------------------------|
| Solder iron              |
| Solder                   |   
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

## B. MCU preparation

### 1. Setting up the MCU

The microcontroller unit(MCU) is the Adafruit Feather M0 RFM95 LoRa Radio - 900 MHz. Out of the box, the MCU comes with "through holes" or "PTH" or simply referred as pins and with no headers attached. The left and right sides of the MCU has 16 pins and 12 pins respectively. 

Prepare the header pins by cutting the long male headers so that they can accommodate all the pins like shown below.

*Note:* Use Headers that are Long Centered 6.25mm on Both Sides (15mm in total), not the 3.5mm pin headers.

Now mount the MCU ***upsidedown*** onto these stacking header pins and make sure the board sits comfortably without any gap. 

When done correctly, the MCU looks perfectly horizontal to the breadboard from the side like shown below. Note that the MCU is upside down since the SD Feather Wing goes on top of MCU.

#### 1.1 Soldering MCU

Now solder the header pins onto the MCU while following [good soldering practices](https://github.com/floodsense/floodsense_sensor/tree/main/quality-control/hardware/QC-build-quality.md).

The MCU after this step should look like the following figure, where the pins are longer on the top to accommodate the SD Feather wing. Be sure to solder all the pins for a reliable electrical contact. 

#### 1.2 Soldering SD Feather Wing

Now carefully mount the SD feather wing onto these pins as shown in the figure. Just to note, the Feather M0 LoRa board and SD Feather Wing have same number of through holes and there must be no through holes left unmounted/soldered after this step.

Now solder from the top as shown in the figure below.

:warning: Using a multimeter, any potential short circuits must be checked.

Potential short circuits at this step are due to improper soldering, refer to [QC-build-quality](https://github.com/floodsense/floodsense_sensor/tree/main/quality-control/hardware/QC-build-quality.md) document for some examples.

#### 1.3 Installing SD card and Coin cell battery onto SD Feather Wing
After the completion of above steps, carefully insert the coin cell with positive side up into the feather wing as shown in the figure.

Now insert SD card into the SD card slot and must hear a click when safely secured.

#### 1.4 MCU Antenna
The Feather m0 LoRa module ***does not*** come with a built-in antenna and needs one for LoRaWAN communication.

A simple single core standard wire can be used as an antenna when cut down to right lengths according to the desired frequencies in the below table. This project is located in Brooklyn, United States, the frequency used is 915 MHz.

| Frequency     | Length (inches) | Length (cm) |
|---------------|-----------------|-------------|
| 433 MHz       | 6.5             | 16.5        |
| 868 MHz       | 3.25            | 8.2         |
| 915 MHz       | 3               | 7.8 cm      |

Cut the single core wire to 3 inches and strip a mm or two just on one end. Now, tin and solder into the **ANT** pad on the very right hand edge of the Feather like shown below.

#### 1.5 Mounting the MCU
The MCU shall now be mounted onto the terminal block breakout board like shown below.

:white_check_mark: The MCU must be secured and feel tightly held onto the breakout board.

:warning: ​If the MCU pins do not completely fit into the female headers, excess solder on few pins might cause this issue. To fix it, remove the excess solder and male sure solder on the pins are clean and consistent.

### 2. Installing the latest firmware on the MCU

#### Necessary Software
For simplicity, Arduino IDE has been used in the documentation but other IDEs can also be used. Arduino IDE, can be downloaded from [Arduino's website](https://www.arduino.cc/en/software). The Arduino IDE is used for uploading the firmware to the microcontroller.

#### FloodSense Sensor Library
Download the FloodSense Sensor Library from [here](https://github.com/floodsense/floodsense_sensor) by clicking on the green code button and selecting "Download Zip" option. The downloaded file's name should be similar to `floodsense_sensor-main.zip` or `floodsense_sensor.zip`. Now to install this library on the Arduino IDE follow either of the following methods. 

**Method 1:**

1. Find Arduino libraries folder
2. Clone the FloodSense library folder into the Arduino>libraries

[**Method 2:**](https://www.arduino.cc/en/guide/libraries)
1. In Arduino IDE, go to: Sketch> Include Library> Add .ZIP Library..

2. Select the downloaded FloodSense ZIP folder

#### Additional Libraries
The following libraries are also required to be installed:
1. Arduino-Lmic library: https://github.com/mcci-catena/arduino-lmic
2. SDFat library: https://github.com/Praneethsvch/SdFat
3. Adafruit's sleepydog library: https://github.com/adafruit/Adafruit_SleepyDog
4. Adafruit's RTClib: https://github.com/adafruit/RTClib

**Restart the Arduino IDE** for the library to appear on the installed libraries.

#### Arduino IDE setup

After you have installed the libraries, navigate to the **Preferences** menu. You can access it from the **File** menu in *Windows* or *Linux*, or the **Arduino** menu on *macOS*.

In the pop-up dialogue, add the following URL to the **Additional Boards Manager URLs**. The list of URLs is comma separated, and *you will only have to add each URL once.*

```html
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
```

After the URL is added, Adafruit's packages will be available in the Board Manager. 

To install the boards with the **Board Manager**, navigate to Tools>Board>Boards Manager...

In the Boards Manager window, choose **Type: All**. 

By typing in the search bar, install the following packages:

- **Arduino SAMD**
- **Adafruit SAMD**

**Restart the Arduino IDE** to ensure that all of the boards are properly installed.

#### Flashing the MCU

**Enter the TTN secret keys** into the file `ttncredentials.h` file that can be found in the path Documents/Arduino/libraries/floodsense_sensor/src

In the Arduino IDE, navigate to File>Examples>Floodsense library and choose basic_sensormodes.

Choose the board **Adafruit Feather M0** by navigating to Tools>Board>Adafruit SAMD>Adafruit Feather M0.

Connect the MCU via micro-usb to your computer. Now, **double press the reset button** on the MCU to enter the **Bootloader Mode**.

On the Arduino IDE, navigate to Sketch>Upload to upload the firmware.


## C. Maxbotix Ultrasonic Sensor Preparation

### 1. Preparing the Maxbotix depth sensor

The GND, VCC, Pin 4 and Pin 5 are used for this application. Now choose 4 wires and prepare them for soldering. Tin all the wires on both sides like shown below

Tinning ensures all the fine wires are held together and making an electrical connection. If the wire is not tinned, the individual fine wires may cause short circuits/wrong connections.

Solder them carefully onto the required pins while following [good soldering practices](https://github.com/floodsense/floodsense_sensor/tree/main/quality-control/hardware/QC-build-quality.md).
Carefully holding the wires together with one hand, twist the wires using the other hand. Check for any loose connections or broken wire ends.

### 2. Drilling a hole in the housing to accommodate Maxbotix depth sensor

To accommodate the Maxbotix sensor, a hole shall be drilled on the bottom side of the housing as shown below, so that the sensor is facing the ground when mounted. This hole is centered on the side where the sensor would be mounted.

Carefully secure the housing before using the drill to prevent it from moving. Please be safe and make use of safety glasses to protect your eyes. 

Use a step drill bit and drill a hole just enough for the Maxbotix sensor's threading on it's top. For the model MB-7389, this threading diameter is 1.17”. A 1 $\frac{1}{4}$" hole should be enough to accommodate the sensor.    


## D. Solar Charger board preparation

### 1. Soldering the Capacitor
The negative pin of the capacitor is indicated by a "-" and/or a colored strip along the can. They might also have a longer positive leg.

Now place the capacitor legs into the corresponding through holes on the solar charger board and make sure there is enough room for it to bend sideways to save space as shown in the figure.

Now solder the capacitor and cut off the excess legs. Make sure these are discarded and not left out inside the sensor as they could short other components.

### 2. Wiring
Tin two wires and solder them onto the pins **B+** and GND as shown below. If the solar charger board is [Adafruit's Universal USB/DC/Solar Lithium Ion/Polymer Charger](https://www.adafruit.com/product/4755), the pins are **LIPO** and **GND** instead.

## E. Solarpanel preparation

### 1. Waterproofing the panel connection
The backside of the solar panel has positive and negative terminals. Use a high quality alarm cable and cut open the ends like shown below.

Strip the edges of the wire and tin them

Now heat shrink both the small wires as shown

Solder them onto the corresponding pins and do not apply heat for too long. This might damage and lift the pad on the solar panel.

Now use a bigger heat shrink to hold the both wires like shown below

Waterproof the electrical contact by using liquid electrical tape. Make sure it covers all around making a good seal.

Let it dry for a few minutes and a good quality seal and connection would like below

The following are the examples of a poor quality seal and connection

### 2. Drilling a hole to accommodate the Solar Panel's cable

The solar panel cable lies in the range of 3-6.5 mm, so drill a hole of diameter **12.6 mm** or simply just enough to thread the **PG-7 cable gland**. 

## F. Wiring

1. Connecting pin io1 to pin 6
2. Mounting the depth sensor
3. Connecting depth sensor to the MCU
4. Connecting Solar board with the Solar Charger board
5. Connecting the battery to the Solar Charger board
6. Connecting Solar Charger board with the MCU

## G. Battery
Plug in the battery into the Batt connector on the solar board charger. Now you should see the MCU powering up and the on board LED turns on after battery plug-in if everything goes right. Else recheck the connections once more.

## H. Final Steps

Double check the following connections and observe the serial monitor data to make sure the sensor is operational and is reading valid distance measurements. 

## I. Troubleshooting

## J. Mounting
