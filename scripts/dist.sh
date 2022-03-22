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

# generate full.bin
#cp dist/${TARGET}/flipper-z-${TARGET}-bootloader-${suffix}.bin \
#    dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.bin
#dd if=/dev/null of=dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.bin bs=1 count=0 seek=32768 2> /dev/null
#cat dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.bin \
#    >>dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.bin \
#    2> /dev/null

# generate full.dfu
#./scripts/bin2dfu.py \
#    -i dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.bin \
#    -o dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.dfu \
#    -a 0x08000000 \
#    -l "Flipper Zero $(echo ${TARGET} | tr a-z A-Z)"

# generate full.json
#./scripts/meta.py merge \
#    -i dist/${TARGET}/flipper-z-${TARGET}-${PROJECT}-${suffix}.json \
#    >dist/${TARGET}/flipper-z-${TARGET}-full-${suffix}.json

#cp dist/${TARGET}/flipper-z-${TARGET}-firmware-${suffix}.dfu ${updatedir}/
#cp dist/${TARGET}/flipper-z-${TARGET}-updater-${suffix}.bin ${updatedir}/updater.bin

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
