#!/bin/sh
BINPATH="/Users/antirez/hack/flipper/official/build/f7-firmware-D/.extapps/asteroids.fap"
cp $BINPATH .
git commit -a -m 'Binary file updated.'
