#!/bin/bash
# This source is part of "flipper-xremote" project
# 2023 - Sandro Kalatozishvili (s.kalatoz@gmail.com)

#FLIPPER_FIRMWARE="/opt/flipper/firmwares/unleashed-firmware"
#FLIPPER_FIRMWARE="/opt/flipper/firmwares/flipperzero-firmware"

XCLR_DIM="\x1B[2m"
XCLR_RED="\x1B[31m"
XCLR_RESET="\x1B[0m\n"

# Parse firmware path from arguments if present
for arg in "$@"; do
    if [[ $arg == --firmware=* || $arg == --fw=* ]]; then
        FLIPPER_FIRMWARE="${arg#*=}"
    fi
done

# Check if FLIPPER_FIRMWARE variable is set
if [ -z "$FLIPPER_FIRMWARE" ]; then
  echo -e "$XCLR_RED""FLIPPER_FIRMWARE variable is not set or is empty. $XCLR_RESET"
  echo "You can either export FLIPPER_FIRMWARE variable:"
  echo -e "$XCLR_DIM""export FLIPPER_FIRMWARE=/path/to/firmware $XCLR_RESET"
  echo "Or pass the firmware path as an argument:"
  echo -e "$XCLR_DIM""$0 --fw=/path/to/firmware $XCLR_RESET"
  exit 1
else
  echo "Using firmware path: $FLIPPER_FIRMWARE"
fi

# Check if the path exists and has a applications_user sub directory
if [[ ! -d "$FLIPPER_FIRMWARE" || ! -d "$FLIPPER_FIRMWARE/applications_user" || ! -f "$FLIPPER_FIRMWARE/fbt" ]]; then
    echo -e "$XCLR_RED""Firmware path does not exist or does not contain the required flipper context. $XCLR_RESET"
    exit 1
fi

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

# Return with success
exit 0