# Use 64-bit Ubuntu as the base image
FROM ubuntu:20.04

# Set environment variable to make the installation non-interactive
ENV DEBIAN_FRONTEND=noninteractive

# Set the working directory inside the container
WORKDIR /flipperzero-firmware

# Update package lists and install dependencies
RUN apt-get update && \
    apt-get -y install \
        build-essential \
        git \
        cmake \
        python3 \
        python3-pip \
        libusb-1.0-0-dev \
        libnewlib-arm-none-eabi \
        gcc-arm-none-eabi \
        binutils-arm-none-eabi \
        libstdc++-arm-none-eabi-newlib \
        pkg-config \
        libssl-dev \
        libncurses5 \
        lsb-core \
        libncurses5-dev \
        libncursesw5-dev \
        libpng-dev \
        libsecret-1-dev \
        zlib1g-dev \
        libreadline-dev \
        libusb-1.0-0-dev \
        libudev-dev \
        qtbase5-dev \
        libhidapi-dev

# Clone the project repository
RUN git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git .

# Checkout a specific version of the project
RUN git checkout 0.98.3

# Install required Python packages
RUN pip3 install pyusb

# Compile the project
RUN ./fbt

# Set the default command to /bin/bash
CMD ["/bin/bash"]
