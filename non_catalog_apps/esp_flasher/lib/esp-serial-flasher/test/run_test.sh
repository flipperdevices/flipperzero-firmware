#!/bin/bash

set -e
mkdir -p build && cd build

if [ "$1" = "host" ]; then
    cmake -DQEMU_TEST=False .. && cmake --build . && ./serial_flasher_test
elif [ "$1" = "qemu" ]; then
    # QEMU_PATH environment variable has to be defined, pointing to qemu-system-xtensa
    # Example: export QEMU_PATH=/home/user/esp/qemu/xtensa-softmmu/qemu-system-xtensa
    if [ -z "${QEMU_PATH}" ]; then
        echo "QEMU_PATH environment variable needs to be set"
        exit 1
    fi

    # Generate empty file into which application will be flashed and compared against
    dd if=/dev/zero bs=1024 count=4096 of="empty_file.bin"

    # Run qemu in background (daemonized)
    ${QEMU_PATH} \
        -daemonize \
        -machine esp32 \
        -drive file=empty_file.bin,if=mtd,format=raw \
        -global driver=esp32.gpio,property=strap_mode,value=0x0f \
        -serial tcp::5555,server,nowait

    cmake -DQEMU_TEST=True .. && cmake --build . && ./serial_flasher_test

    # Kill qemu process running in background
    kill -9 $(pidof qemu-system-xtensa)
else
    echo "Please select which test to run: qemu or host"
fi