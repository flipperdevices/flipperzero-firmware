cmake_minimum_required(VERSION 3.13.1)

option(TEST "build tests" OFF)

find_package(Zephyr REQUIRED HINTS ../zephyr)

project(flipper-zero-firmware)

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/include/cmsis_rtos_v2)

include_directories(include ../../core ../../applications)

find_library(FLIPPER_CORE_LIBRARY flipper_core HINTS ../../core-rs/target/x86_64-unknown-linux-gnu PATH_SUFFIXES debug release)

target_link_libraries(app PRIVATE ${FLIPPER_CORE_LIBRARY})

if(TEST)
    add_definitions(-DTEST)
else()
    add_definitions(-DEXAMPLE_BLINK -DEXAMPLE_IPC)
endif(TEST)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/flipper_hal.c)

target_sources(app PRIVATE ../../core/app.cpp)
target_sources(app PRIVATE ../../core/log.c)
target_sources(app PRIVATE ../../core/tty_uart.c)
target_sources(app PRIVATE ../../core/furi.c)
target_sources(app PRIVATE ../../core/furi_ac.c)

target_sources(app PRIVATE ../../applications/examples/blink.c)
target_sources(app PRIVATE ../../applications/examples/uart_write.c)
target_sources(app PRIVATE ../../applications/examples/ipc.c)

target_sources(app PRIVATE ../../applications/tests/furiac_test.c)
target_sources(app PRIVATE ../../applications/tests/furi_record_test.c)
target_sources(app PRIVATE ../../applications/tests/test_index.c)
