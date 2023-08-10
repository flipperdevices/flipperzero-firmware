#!/bin/sh

WORKSPACE=$(eval "pwd")
PROJ=wolfssl.xcodeproj
CONFIG=Release
SCHEME=wolfssl_ios
ARCH=i386
SDK=watchsimulator5.1
CONF_BUILD_DIR=${WORKSPACE}/simulator

xcodebuild clean build -project ${PROJ} -configuration ${CONFIG} \
           -scheme ${SCHEME} -arch ${ARCH} -sdk ${SDK} \
           BITCODE_GENERATION_MODE=bitcode \
           OTHER_CFLAGS="-fembed-bitcode -O3 -fomit-frame-pointer" \
           CONFIGURATION_BUILD_DIR=${CONF_BUILD_DIR}  \
           -quiet
