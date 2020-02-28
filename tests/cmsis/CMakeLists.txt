set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)
cmake_minimum_required(VERSION 3.8)

project(cmsis-test C ASM)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)

set(SOURCES main.c)

include(stm32/devices)

stm32_get_devices_by_family(F4 DEVICES)
foreach(DEVICE ${DEVICES})    
    add_executable(cmsis-test-${DEVICE} ${SOURCES})
    target_link_libraries(cmsis-test-${DEVICE} CMSIS::STM32::${DEVICE} STM32::NoSys)
endforeach()
