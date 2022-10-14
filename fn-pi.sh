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

echo "Starting CI/CT process for FloodSensor...."
echo "Enter <pi_name>@<address> to begin:"
read pi

ssh $pi <<-EOF
	sudo apt-get update
	sudo apt-get upgrade
	sudo docker start fn_dev_env
	cat <<-PIPEIN | sudo docker exec -i fn_dev_env /bin/bash
		cd ~/../flood-sensor/ 
		git checkout Develop
		git pull
		cd firmware/lora-e5
		if [ -d build ]; then 
			rm -rf build && echo "deleting previous build files..."
		fi 
		mkdir build && cd build 
		cmake ..
		make
	PIPEIN
EOF
