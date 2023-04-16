#!/bin/zsh

cp ../../../build/f7-firmware-D/.extapps/seader.fap .
arm-none-eabi-readelf seader.fap -x 10
zip seader.fap.zip seader.fap
rm seader.fap
