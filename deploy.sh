#!/bin/bash
# This source is part of "flipper-xremote" project
# 2023 - Sandro Kalatozishvili (s.kalatoz@gmail.com)

# Change it according to the root path of the used firmware
FLIPPER_FIRMWARE="/opt/flipper/firmwares/unleashed-firmware"
#FLIPPER_FIRMWARE="/opt/flipper/firmwares/flipperzero-firmware"

# Private variables
XREMOTE_PROJ_PATH=$(dirname $(readlink -f "$0"))
XREMOTE_PROJ_NAME=$(basename "$XREMOTE_PROJ_PATH")

FLIPPER_APPSRC="applications_user/$XREMOTE_PROJ_NAME"
FLIPPER_USER_APP="$FLIPPER_FIRMWARE/$FLIPPER_APPSRC"

# Unlink existing user application first
[ -s $FLIPPER_USER_APP ] && rm -f $FLIPPER_USER_APP
ln -s $XREMOTE_PROJ_PATH $FLIPPER_FIRMWARE/applications_user

# Build and deploy the project
cd $FLIPPER_FIRMWARE
DEPLOY_DONE=0
sudo ./fbt COMPACT=1 DEBUG=0 launch APPSRC=$FLIPPER_APPSRC && DEPLOY_DONE=1

# Run qflipper command if asked
for arg in "$@"; do
    if [[ $arg == "--run" || $arg == "-r" ]]; then
        [ $DEPLOY_DONE -eq 1 ] && sudo qflipper
    fi
done
