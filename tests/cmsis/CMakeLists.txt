cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

if(NOT TEST_FAMILIES)
    set(TEST_FAMILIES F4)
endif()

project(cmsis-test C ASM)

find_package(CMSIS REQUIRED)

set(SOURCES main.c)

include(stm32/devices)

foreach(FAMILY ${TEST_FAMILIES})
    stm32_get_devices_by_family(${FAMILY} DEVICES)
    foreach(DEVICE ${DEVICES})    
        add_executable(cmsis-test-${DEVICE} ${SOURCES})
        target_link_libraries(cmsis-test-${DEVICE} CMSIS::STM32::${DEVICE} STM32::NoSys)
    endforeach()
endforeach()
