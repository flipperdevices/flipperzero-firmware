#!/bin/bash

rm -rf .flipper-firmware

git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git ./.flipper-firmware

cd .flipper-firmware

./fbt -h