#!/usr/bin/env bash

set -e

suffix="${DIST_SUFFIX:=local}"

# detect targets
targets=$(comm -12 <(ls bootloader/.obj) <(ls firmware/.obj))

for target in $targets; do
    rm -rf "dist/$target"
    mkdir -p "dist/$target"

    # copy build outputs
    cp bootloader/.obj/${target}/bootloader.elf \
        dist/${target}/flipper-z-${target}-bootloader-${suffix}.elf
    cp bootloader/.obj/${target}/bootloader.bin \
        dist/${target}/flipper-z-${target}-bootloader-${suffix}.bin
    cp bootloader/.obj/${target}/bootloader.dfu \
        dist/${target}/flipper-z-${target}-bootloader-${suffix}.dfu
    cp bootloader/.obj/${target}/bootloader.json \
        dist/${target}/flipper-z-${target}-bootloader-${suffix}.json
    cp firmware/.obj/${target}/firmware.elf \
        dist/${target}/flipper-z-${target}-firmware-${suffix}.elf
    cp firmware/.obj/${target}/firmware.bin \
        dist/${target}/flipper-z-${target}-firmware-${suffix}.bin
    cp firmware/.obj/${target}/firmware.dfu \
        dist/${target}/flipper-z-${target}-firmware-${suffix}.dfu
    cp firmware/.obj/${target}/firmware.json \
        dist/${target}/flipper-z-${target}-firmware-${suffix}.json

    # generate full.bin
    cp dist/${target}/flipper-z-${target}-bootloader-${suffix}.bin \
        dist/${target}/flipper-z-${target}-full-${suffix}.bin
    truncate -s 32768 dist/${target}/flipper-z-${target}-full-${suffix}.bin
    cat dist/${target}/flipper-z-${target}-firmware-${suffix}.bin \
        >>dist/${target}/flipper-z-${target}-full-${suffix}.bin

    # generate full.dfu
    ./scripts/bin2dfu.py \
        -i dist/${target}/flipper-z-${target}-full-${suffix}.bin \
        -o dist/${target}/flipper-z-${target}-full-${suffix}.dfu \
        -a 0x08000000 \
        -l "Flipper Zero $(echo $target | tr a-z A-Z)"

    # generate full.json
    ./scripts/meta.py merge \
        -i dist/${target}/flipper-z-${target}-bootloader-${suffix}.json \
        dist/${target}/flipper-z-${target}-firmware-${suffix}.json \
        >dist/${target}/flipper-z-${target}-full-${suffix}.json
    
    echo "Firmware binaries can be found at:"
    echo -e "\t$(pwd)/dist/${target}"
    echo "Use this file to flash your Flipper:"
    echo -e "\tflipper-z-${target}-full-${suffix}.dfu"
done
