#!/usr/bin/env bash

set -e

suffix="${DIST_SUFFIX:=local}"
targetdir="dist/${TARGET}"
updatedir="${targetdir}/update-${TARGET}-${suffix}"

rm -rf "${targetdir}"
mkdir -p "${targetdir}"
mkdir -p "${updatedir}"

function copy_project_files() {
    local PROJECT=$1
    # copy build outputs
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.elf \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.elf
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.bin \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.bin
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.dfu \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.dfu
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.json \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.json
}

copy_project_files firmware

echo "Firmware binaries can be found at:"
echo -e "\t$(pwd)/dist/${TARGET}"
echo "Use this file to flash your Flipper:"
echo -e "\tflipper-z-${TARGET}-full-${suffix}.dfu"

