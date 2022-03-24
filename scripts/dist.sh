#!/usr/bin/env bash

set -e

suffix="${DIST_SUFFIX:=local}"
targetdir="dist/${TARGET}"
updatedir="${targetdir}/update"

rm -rf "${targetdir}"
mkdir -p "${targetdir}"
mkdir -p "${updatedir}"

for PROJECT in firmware updater
do
    # copy build outputs
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.elf \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.elf
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.bin \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.bin
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.dfu \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.dfu
    cp firmware/.obj/${TARGET}-${PROJECT}/${PROJECT}.json \
        dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.json
done

# generate full.json
#./scripts/meta.py merge \
#    -i dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.json \
#    >dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.json

./scripts/update.py generate \
    -d "${updatedir}" \
    -t "${TARGET}" \
    -dfu "dist/${TARGET}/flipper-z-${TARGET}-firmware-${suffix}.dfu" \
    -stage "dist/${TARGET}/flipper-z-${TARGET}-updater-${suffix}.bin"


echo "Firmware binaries can be found at:"
echo -e "\t$(pwd)/dist/${TARGET}"
echo "Use this file to flash your Flipper:"
echo -e "\tflipper-z-${TARGET}-full-${suffix}.dfu"
echo "Use this directory to self-update your Flipper:"
echo -e "\t${updatedir}"
