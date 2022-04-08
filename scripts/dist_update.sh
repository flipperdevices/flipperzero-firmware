#!/usr/bin/env bash

set -e

source $(dirname -- "$0")/dist.sh

copy_project_files updater

./scripts/update.py generate \
    -d "${updatedir}" \
    -v "${VERSION_STRING}" \
    -t "${TARGET}" \
    -dfu "dist/${TARGET}/flipper-z-${TARGET}-firmware-${suffix}.dfu" \
    -stage "dist/${TARGET}/flipper-z-${TARGET}-updater-${suffix}.bin"

echo "Use this directory to self-update your Flipper:"
echo -e "\t${updatedir}"
