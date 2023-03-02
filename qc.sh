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
	#update backend that device found
	#if this DevEUI does NOT exist in backend -> create a device and assign <timestamp, dev_eui, host_pi_name>  
	#program the device with QC firmware
	#update backend when completed to display unplug instruction on dashboard

	#else
	#sleep for 1 sec and check serial ports again
}	
