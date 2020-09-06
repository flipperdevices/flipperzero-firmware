FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive
ENV LANG C.UTF-8

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        make \
        gcc-arm-none-eabi \
        binutils-arm-none-eabi \
        libnewlib-arm-none-eabi \
        autoconf \
        automake \
        libtool \
        curl \
        wget \
        g++ \
        g++-multilib \
        unzip \
        build-essential \
        python \
        python-pip \
        libstdc++-arm-none-eabi-newlib \
        libclang-10-dev \
        git \
        ninja-build \
        gperf \
        ccache \
        dfu-util \
        device-tree-compiler \
        python3-dev \
        python3-pip \
        python3-setuptools \
        python3-tk \
        python3-wheel \
        xz-utils \
        file \
        gpg-agent \
        software-properties-common && \
    curl -fsSL https://apt.kitware.com/keys/kitware-archive-latest.asc | apt-key add - && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt-get update && \
    apt-get install -y cmake && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN curl https://sh.rustup.rs -sSf | sh -s -- -y --profile=minimal --target thumbv7em-none-eabi thumbv7em-none-eabihf

RUN curl -fsSLO https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/v2.3.0/scripts/requirements-base.txt && \
    pip3 install -r requirements-base.txt && \
    rm -f requirements-base.txt

ENV ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
ENV GNUARMEMB_TOOLCHAIN_PATH=/usr

COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
