#!/bin/bash


# Remove old builds and sources
rm -rf .flipperzero-firmware/applications_user/flipp_pomodoro
rm -rf dist

# Place app sources to the dir for build to perform
cp -r flipp_pomodoro/. .flipperzero-firmware/applications_user/flipp_pomodoro

# Build an app
cd .flipperzero-firmware
./fbt fap_flipp_pomodoro

# Collect build artifacts
cp -r build/f7-firmware-D/.extapps/. ../dist