# define chip used in this project, this set must define before project definition
# for this project dont use cmake commandline option -DSTM32_CHIP=<chip>
set(STM32_CHIP STM32F407xG)

cmake_minimum_required(VERSION 3.4)
project(chibios-nil-f4-template)

ENABLE_LANGUAGE(ASM)

# test build all available ChibiOS COMPONENTS for F4 chip
#FIND_PACKAGE(ChibiOS COMPONENTS nil hal adc can dac ext gpt i2c i2s icu mac mmc_spi pal pwm rtc sdc serial serial_usb spi st uart usb memstreams nullstreams REQUIRED)

FIND_PACKAGE(ChibiOS COMPONENTS nil hal pal  REQUIRED)

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ChibiOS_INCLUDE_DIRS}
        config
        board
        work
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



ADD_DEFINITIONS(-DCORTEX_USE_FPU=TRUE)

SET(STM32_LINKER_SCRIPT ${ChibiOS_LINKER_SCRIPT})


set(SOURCE_FILES main.c board/board.c board/board.h config/mcuconf.h config/halconf.h config/nilconf.h work/test.c work/test.h )

add_executable(${CMAKE_PROJECT_NAME}.elf ${SOURCE_FILES}  ${ChibiOS_SOURCES})

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME}.elf)

STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME}.elf)
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME}.elf)
STM32_PRINT_SIZE_OF_TARGETS(${CMAKE_PROJECT_NAME}.elf)

