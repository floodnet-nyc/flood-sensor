import os
import shlex
import serial
import subprocess
from telnetlib import Telnet
import requests


NODERED_URL = os.getenv('NODERED_URL')
assert NODERED_URL, 'NODERED_URL environment variable required.'

def bash(cmd, env=None):
    return subprocess.run(['bash', '-c', cmd], env={**os.environ.copy(), **(env or {})}, capture_output=True)

def initial_openocd():
    p = bash(r'''
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
        -f /usr/share/openocd/scripts/target/stm32wlx.cfg
    ''')

def get_dev_eui_from_telnet(host='host.docker.internal', port=4444):
    with Telnet(host, port) as tn:
        i, match, prev = tn.expect([b'> '])
        print(f"log until match {match} at {i}:", prev)
        tn.write(b'stm32wlx.cpu mdb 0x1FFF7580 8\n')


def register_device(dev_eui):
    r = requests.post(f'{NODERED_URL}/register-end-device', json={'dev_eui': dev_eui})
    r.raise_for_status()
    data = r.json()
    print(data)
    return data['app_key'], data['join_eui']



def build_firmware(app_key, dev_eui, join_eui):
    p = bash(r'''
# compiling firmware
cd flood-sensor/lora-e5
# reset identity file and replace with variables.
git checkout LoRaWAN/App/se-identity.h
sed s/(\#define LORAWAN_DEVICE_EUI)\s+.*/\1 $LORAWAN_DEVICE_EUI/ LoRaWAN/App/se-identity.h
sed s/(\#define LORAWAN_JOIN_EUI)\s+.*/\1 $LORAWAN_JOIN_EUI/ LoRaWAN/App/se-identity.h
sed s/(\#define LORAWAN_APP_KEY)\s+.*/\1 $LORAWAN_APP_KEY/ LoRaWAN/App/se-identity.h

# compile
mkdir build && cd build && cmake .. && make

openocd -f usr/share/openocd/scripts/interface/stlink.cfg \
        -f usr/share/openocd/scripts/target/stm32wlx.cfg \
        -c init -c "reset halt"  					    \
        -c "flash write_image erase lora-e5.bin 0x08000000" \
        -c "verify reset exit"
    ''', env={'LORAWAN_DEVICE_EUI': dev_eui, 'LORAWAN_JOIN_EUI': join_eui, 'LORAWAN_APP_KEY': app_key})

def main():
    initial_openocd()
    dev_eui = get_dev_eui_from_telnet()
    app_key, join_eui = register_device(dev_eui)
    build_firmware(app_key, dev_eui, join_eui)



if __name__ == '__main__':
    import fire
    fire.Fire()