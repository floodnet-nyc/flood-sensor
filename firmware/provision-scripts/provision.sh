# 
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
        -f /usr/share/openocd/scripts/target/stm32wlx.cfg

# 1. get Dev EUI
# telnet 4444 > stm32wlx.cpu mdb 0x1FFF7580 8

# generate app key

# 2. ttn



# compiling firmware
cd flood-sensor/lora-e5
# reset identity file and replace with variables.
git checkout LoRaWAN/App/se-identity.h
sed s/(\#define LORAWAN_DEVICE_EUI)\s+.*/\1 $LORAWAN_DEVICE_EUI/ LoRaWAN/App/se-identity.h
sed s/(\#define LORAWAN_JOIN_EUI)\s+.*/\1 $LORAWAN_JOIN_EUI/ LoRaWAN/App/se-identity.h
sed s/(\#define LORAWAN_APP_KEY)\s+.*/\1 $LORAWAN_APP_KEY/ LoRaWAN/App/se-identity.h

# compile
mkdir build && cd build && cmake .. && make

# flash
openocd -f usr/share/openocd/scripts/interface/stlink.cfg \
        -f usr/share/openocd/scripts/target/stm32wlx.cfg \
        -c init -c "reset halt"  					    \
        -c "flash write_image erase lora-e5.bin 0x08000000" \
        -c "verify reset exit”
