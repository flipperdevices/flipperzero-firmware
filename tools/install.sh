#!/bin/bash

rm -rf .flipperzero-firmware

git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git ./.flipperzero-firmware

cd .flipperzero-firmware

./fbt -h