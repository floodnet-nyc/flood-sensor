#----------- RUN-ON-BOOT QC-script -----------------
#
# 
# setup instructions: https://raspberrypi-guide.github.io/programming/run-script-on-boot
#
#---------------------------------------------------


run_qc_test() {
	echo "Running QC test..."

	#check serial ports /dev/tty* for a USB device
	#check lsusb for STMicroelectronics device connected

	#if found -> extract DevEUI
	if sudo openocd -f usr/share/openocd/scripts/interface/stlink.cfg -f usr/share/openocd/scripts/target/stm32wlx.cfg | grep -q "<insert sensor found message>"; then
		echo "Found the sensor... Attempting to start a debug session"
		#start telnet session and run mdb command inside it to get the DevEUI 
		spawn telnet localhost 4444
		expect -re "Connected" 
		send "stm32wlx.cpu mdb 0x1FFF7580 8"
		expect -re "00"
		send "exit\r"	
	fi

	#update backend that device found
	#if this DevEUI does NOT exist in backend -> create a device and assign <timestamp, dev_eui, host_pi_name>  
	#program the device with QC firmware
	#update backend when completed to display unplug instruction on dashboard

	#else
	#sleep for 1 sec and check serial ports again
}	
