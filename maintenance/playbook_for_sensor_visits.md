# Maintenance Playbook for Sensor Visits

<!-- MarkdownTOC levels="2,3" autolink="true" -->

- [1. Detaching sensor from the mount](#1-detaching-sensor-from-the-mount)
	- [1.1 Uninstalling Solar Panel](#11-uninstalling-solar-panel)
	- [1.2 Detaching the housing](#12-detaching-the-housing)
- [2. Updating the device firmware to the latest version](#2-updating-the-device-firmware-to-the-latest-version)
	- [2.1 Obtaining the latest firmware](#21-obtaining-the-latest-firmware)
	- [2.2 End-device network secret keys](#22-end-device-network-secret-keys)
	- [2.3 Clear EEPROM](#23-clear-eeprom)
	- [2.4 Compile and Upload the new Firmware Image](#24-compile-and-upload-the-new-firmware-image)
- [3. Identifying and Replacing damaged Hardware](#3-identifying-and-replacing-damaged-hardware)
	- [3.1 Damaged solar panel](#31-damaged-solar-panel)
	- [3.2 Damaged ultrasonic sensor](#32-damaged-ultrasonic-sensor)
	- [3.3 Damaged Antenna](#33-damaged-antenna)
	- [3.4 Loosely connected Antenna](#34-loosely-connected-antenna)
- [4. Upgrading Antennas for sensors with weak signal strength](#4-upgrading-antennas-for-sensors-with-weak-signal-strength)
- [5. Solar Panel Maintenance](#5-solar-panel-maintenance)
- [6. Re-installing the sensor](#6-re-installing-the-sensor)
	- [6.1 On the ground -> Verification](#61-on-the-ground---verification)
	- [6.2 Up the ladder -> Re-installing the housing](#62-up-the-ladder---re-installing-the-housing)
	- [6.3 Re-installing the solar panel](#63-re-installing-the-solar-panel)
	- [6.4 Switching Sensor to operational state](#64-switching-sensor-to-operational-state)
- [7. Adding Sensor ID stickers](#7-adding-sensor-id-stickers)
- [8. Installing signage - Missing or Damaged](#8-installing-signage---missing-or-damaged)

<!-- /MarkdownTOC -->


This playbook details the steps for in-person maintanance visits to perform quick, reliable firmware updates and quality sensor maintenance.





## 1. Detaching sensor from the mount

In order to detach the sensor, first solar panel, followed by the housing shall be uninstalled. 

### 1.1 Uninstalling Solar Panel

The solar panel uses a struct nut with M6 hex screw to fasten the panel mount to the struct channel. Unfasten the M6 screw completely detach the panel from the struct channel.

**Note:** _If the panel is installed somewhere else other than the struct channel, this has been done to harvest maximum solar power by being in direct sunlight. Keep a note of this mounting location for re-installing after the maintenance._ 

### 1.2 Detaching the housing

Unfasten the M6 screws on the either side of the housing almost halfway or when the sensor is able slide out and is completely detached from the mount.






## 2. Updating the device firmware to the latest version

### 2.1 Obtaining the latest firmware

The latest branch can be downloaded from the [main branch of FloodNet's flood-sensor repository](https://github.com/floodnet-nyc/flood-sensor). 

### 2.2 End-device network secret keys

For every device, update the keys in `ttn_credentials.h` file with the `floodnet-lab` keys with same Device ID.

### 2.3 Clear EEPROM

### 2.4 Compile and Upload the new Firmware Image







## 3. Identifying and Replacing damaged Hardware

### 3.1 Damaged solar panel
### 3.2 Damaged ultrasonic sensor
### 3.3 Damaged Antenna
### 3.4 Loosely connected Antenna






## 4. Upgrading Antennas for sensors with weak signal strength

Upgrade the previous antenna with a larger gain antenna to imporve signal strength in devices which previously weaker signal or significant packet-loss has been observed.






## 5. Solar Panel Maintenance 

Clean the panel to clear and debris.







## 6. Re-installing the sensor 

_Before_ climbing up the ladder read the below steps:


### 6.1 On the ground -> Verification

#### 6.1.1 Verify that the sensor is in CFG mode on `floodnet-lab`

#### 6.2.2 Verify firmware version and sensor settings from the CFG uplinks

### 6.2 Up the ladder -> Re-installing the housing

### 6.3 Re-installing the solar panel

### 6.4 Switching Sensor to operational state

#### 6.4.1 Change Application from `floodnet-lab` to `floodnet-live`

#### 6.4.2 Start sensing

#### 6.4.3 Check whether the sensor is pushing correct distance and battery data






## 7. Adding Sensor ID stickers

Use latest deviceEUI from the `floodnet-live` application





## 8. Installing signage - Missing or Damaged

Install FloodNet signage if missing or damaged.





