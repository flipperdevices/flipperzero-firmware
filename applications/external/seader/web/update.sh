#!/bin/zsh

cp ../dist/seader.fap .
arm-none-eabi-readelf seader.fap -x 10
zip seader.fap.zip seader.fap
rm seader.fap
