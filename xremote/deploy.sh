#!/bin/bash
# This source is part of "flipper-xremote" project
# 2023 - Sandro Kalatozishvili (s.kalatoz@gmail.com)

#FLIPPER_FIRMWARE="/opt/flipper/firmwares/unleashed-firmware"
#FLIPPER_FIRMWARE="/opt/flipper/firmwares/flipperzero-firmware"

XCLR_DIM="\x1B[2m"
XCLR_RED="\x1B[31m"
XCLR_RESET="\x1B[0m\n"

FBT_CMD="./fbt"
FBT_DBG="DEBUG=0"
FBT_ARGS="COMPACT=1 launch"

BUILD_PROJECT=0
LINK_PROJECT=0
RUN_QFLIPPER=0
BUILD_DONE=0

for arg in "$@"; do
    if [[ $arg == --firmware=* || $arg == --fw=* ]]; then
        FLIPPER_FIRMWARE="${arg#*=}"
    fi

    if [[ $arg == "--build" || $arg == "-b" ]]; then
        BUILD_PROJECT=1
    fi

    if [[ $arg == "--run" || $arg == "-r" ]]; then
        RUN_PROJECT=1
    fi

    if [[ $arg == "--link" || $arg == "-l" ]]; then
        LINK_PROJECT=1
    fi

    if [[ $arg == "--debug" || $arg == "-d" ]]; then
        FBT_DBG="DEBUG=1"
    fi

    if [[ $arg == "--sudo" || $arg == "-s" ]]; then
        FBT_CMD="sudo ./fbt"
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

link_project() {
    [ -s $FLIPPER_USER_APP ] && rm -f $FLIPPER_USER_APP
    ln -s $XREMOTE_PROJ_PATH $FLIPPER_FIRMWARE/applications_user
}

build_project() {
    cd $FLIPPER_FIRMWARE
    $FBT_CMD $FBT_ARGS $FBT_DBG APPSRC=$FLIPPER_APPSRC && BUILD_DONE=1
}

run_project() {
    if [[ $BUILD_PROJECT -eq 0 || $BUILD_DONE -eq 1  ]]; then
        qFlipper
    fi
}

[ $LINK_PROJECT -eq 1 ] && link_project
[ $BUILD_PROJECT -eq 1 ] && build_project
[ $RUN_PROJECT -eq 1 ] && run_project

# Return with success
exit 0
