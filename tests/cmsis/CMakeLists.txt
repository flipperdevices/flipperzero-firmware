set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)
cmake_minimum_required(VERSION 3.8)

project(cmsis-test C ASM)

set(SOURCES main.c)

include(${CMAKE_SOURCE_DIR}/../stm32_devices.cmake)
stm32_get_devices_by_family(F4 DEVICES)

foreach(DEVICE ${DEVICES})
    string(TOLOWER ${DEVICE} DEVICE_L)
    set(TARGET cmsis-test-${DEVICE_L})
    
    message(STATUS "Testing ${DEVICE}...")
    add_executable(${TARGET} ${SOURCES})
    set_target_properties(${TARGET} PROPERTIES 
        STM32_CHIP ${DEVICE}
        STM32_GCC_SPECS nosys
    )
    stm32_target(${TARGET})
endforeach()
