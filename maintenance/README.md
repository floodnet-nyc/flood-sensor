
- [Maintenance playbook](#maintenance-playbook)
- [Common issues and their fixes](#common-issues-and-their-fixes)
	- [Trending down battery profile](#trending-down-battery-profile)
	- [Brownouts](#brownouts)
	- [Antenna Issues](#antenna-issues)
	- [Manual Firmware updates](#manual-firmware-updates)
	- [Operational Issues:](#operational-issues)
	- [Sensor swapout in the case of a failure](#sensor-swapout-in-the-case-of-a-failure)

# Maintenance playbook

This playbook details the steps for in-person maintanance visits to perform quick, reliable firmware updates and quality sensor maintenance.



1. **Detaching sensor from the mount**

	In order to detach the sensor, first solar panel, followed by the housing shall be uninstalled. 

    - **Uninstalling Solar Panel:**

		The solar panel uses a struct nut with M6 hex screw to fasten the panel mount to the struct channel. Unfasten the M6 screw completely detach the panel from the struct channel.

		**Note:** _If the panel is installed somewhere else other than the struct channel, this has been done to harvest maximum solar power by being in direct sunlight. Keep a note of this mounting location for re-installing after the maintenance._ 

	- **Detaching the housing:**

		Unfasten the M6 screws on the either side of the housing almost halfway or when the sensor is able slide out and is completely detached from the mount.


2. **Updating the device firmware to the latest version**

	- **Obtaining the latest firmware:**

		The latest branch can be downloaded from the [main branch](https://github.com/floodnet-nyc/flood-sensor) of FloodNet's flood-sensor repository. 

    - **End-device network secret keys:**
  
		For every device, update the keys in `ttn_credentials.h` file with the `floodnet-lab` keys with same Device ID.

    - **Clear EEPROM:**

    - **Compile and Upload the new Firmware Image**


3. **Identifying and Replacing damaged Hardware**

   - **Damaged solar panel:**
   - **Damaged ultrasonic sensor:**
   - **Damaged Antenna:**
   - **Loosely connected Antenna:**



4. **Upgrading Antennas for sensors with weak signal strength**

	Upgrade the previous antenna with a larger gain antenna to imporve signal strength in devices which previously weaker signal or significant packet-loss has been observed.



5. **Solar Panel Maintenance** 

	Clean the panel to clear and debris.



6. **Re-installing the sensor**

	_Before_ climbing up the ladder read the below steps:

	- On the ground -> Verification

    	- Verify that the sensor is in CFG mode on `floodnet-lab`

        - Verify firmware version and sensor settings from the CFG uplinks

    - Up the ladder -> Sensor Installtion
        - Re-installing the housing

        - Re-installing the solar panel

    - Switching Sensor to operational state

        - Change Application from `floodnet-lab` to `floodnet-live`

        - Start sensing

        - Check whether the sensor is pushing correct distance and battery data


7. **Adding Sensor ID stickers**

	Use latest deviceEUI from the `floodnet-live` application



8. **Installing signage if missing or damaged**

	Install FloodNet signage if missing or damaged.




# Common issues and their fixes

### Trending down battery profile
  - Not enough sunlight 
  - Battery does not charge
### Brownouts

### Antenna Issues

### Manual Firmware updates

### Operational Issues:
	- Inaccurate or unexpected sensor measurements
	- No sensor measurements
### Sensor swapout in the case of a failure


