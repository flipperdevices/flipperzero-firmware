cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr)

project(flipper-zero-firmware)

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/include/cmsis_rtos_v2)

include_directories(include ../../core ../../applications)

add_definitions(-DEXAMPLE_BLINK -DEXAMPLE_IPC)

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
