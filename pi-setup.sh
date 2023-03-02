#!/usr/bin/env bash
#================================================================
# HEADER
#================================================================
#% 
#% todo
#%
#================================================================
# END_OF_HEADER
#================================================================


# Highly adviced to stop if any error occurs along the way 
set -Eeuo pipefail


FN_DOCKER_IMG=""
PI_USERNAMES=("", "")
PI_IPS=("","")
PI_PASSWORDS=("","")
FLOOD_SENSOR_SRC_PATH=""
TOOLCHAIN_PATH=""

FIRST_TIME_DOCKER_SETUP="apt update -y
apt upgrade
apt-get install -y sudo curl openocd git autoconf libtool make cmake pkg-config libusb-1.0-0 libusb-1.0-0-dev tar bzip2 tree snapd usbutils
sudo apt-get install -y gcc-arm-none-eabi
mkdir $FLOOD_SENSOR_SRC_PATH && cd $FLOOD_SENSOR_SRC_PATH
git clone https://github.com/floodnet-nyc/flood-sensor.git
mkdir $TOOLCHAIN_PATH && cd $TOOLCHAIN_PATH
curl -Lo gcc-arm-none-eabi.tar.xz 'https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-aarch64-arm-none-eabi.tar.xz?rev=82c9a3730e454ab6b8101952cd700cda&hash=9827748B56CB4E80301BB0937274C2814C22D57F'
mkdir /opt/gcc-arm-none-eabi
cd ~/.. && tar -xf $TOOLCHAIN_PATH/gcc-arm-none-eabi.tar.xz --strip-components=1 -C ~/../opt/gcc-arm-none-eabi
export PATH=$PATH:/opt/gcc-arm-none-eabi/bin	
exit
"

FIRST_TIME_PI_SETUP_SCRIPT="
apt update -y
apt upgrade
apt-get install -y sudo curl openocd git
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo systemctl start docker && sudo docker pull $FN_DOCKER_IMG
cat <<-PIPEIN | sudo docker run -ti $FN_DOCKER_IMG
$FIRST_TIME_DOCKER_SETUP
PIPEIN
"


UPDATE_PI_SCRIPT="
dpkg -s 'sudo' >/dev/null 2>&1 && {
	apt-get install -y sudo
}
sudo apt-get update -y
sudo apt-get upgrade
"

pi_first_time_setup(){

#pick pi host name

#ssh into pi and assign picked pi hostname 
echo "Running First time setup scripts for pi...."
ssh $pi_ip /bin/bash << EOF
	$FIRST_TIME_PI_SETUP_SCRIPT	
EOF

#todo: check setup status

#add entry to backend
}

