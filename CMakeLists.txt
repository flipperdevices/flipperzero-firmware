# this file is only to satisfy CLion

cmake_minimum_required(VERSION 3.23)
project(trexrunner)

include_directories(.)

set(FLIPPER_FIRMWARE_PATH /home/gelin/work/github/flipperzero-firmware)

include_directories(SYSTEM
    ${FLIPPER_FIRMWARE_PATH}/furi/
    ${FLIPPER_FIRMWARE_PATH}/firmware/targets/f7/furi_hal/
    ${FLIPPER_FIRMWARE_PATH}/firmware/targets/furi_hal_include/
    ${FLIPPER_FIRMWARE_PATH}/applications/services/
)

# there's no need to build executable, but without it CLion don't load libraries
add_executable(trexrunner trexrunner.c)
