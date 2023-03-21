FROM python:3.10

# build

# apt installs
RUN apt update && \
    pip install cmake && \
    apt-get install -y \
        sudo curl openocd git autoconf libtool make pkg-config \
        libusb-1.0-0 libusb-1.0-0-dev tar bzip2 tree snapd usbutils telnet && \
    apt-get install -y gcc-arm-none-eabi && \
    apt-get install -y --only-upgrade cmake && \
    rm -rf /var/lib/apt/lists/*

# install toolchain
RUN mkdir toolchain && cd toolchain; \
    ARM_TOOLCHAIN_VERSION=$(curl -s https://developer.arm.com/downloads/-/gnu-rm | grep -Po '<h3>Version \K.+(?= <span)');\
    echo ARM_TOOLCHAIN_VERSION $ARM_TOOLCHAIN_VERSION; \
    curl -Lo gcc-arm-none-eabi.tar.bz2 \
    "https://developer.arm.com/-/media/Files/downloads/gnu-rm/${ARM_TOOLCHAIN_VERSION}/gcc-arm-none-eabi-${ARM_TOOLCHAIN_VERSION}-aarch64-linux.tar.bz2" && \
    mkdir /opt/gcc-arm-none-eabi && \
    tar -xvjf /toolchain/gcc-arm-none-eabi.tar.bz2 --strip-components=1 -C /opt/gcc-arm-none-eabi

# update path
ENV PATH=$PATH:/opt/gcc-arm-none-eabi/bin

RUN git clone https://github.com/floodnet-nyc/flood-sensor.git && \
    cd flood-sensor && \
    git checkout Develop && \
    cd firmware/lora-e5 && \
    mkdir build && cd build && cmake .. && make

ADD firmware/provision-scripts .

# ENTRYPOINT [ "./usb_watch.sh" ]