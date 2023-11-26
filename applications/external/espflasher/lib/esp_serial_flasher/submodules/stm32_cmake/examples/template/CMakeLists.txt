cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-template C ASM)

find_package(CMSIS COMPONENTS STM32F407VG REQUIRED)
# Find CMSIS for all F4 devices: 
#find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
# Find CMSIS for all devices: 
#find_package(CMSIS REQUIRED)

set(PROJECT_SOURCES
    main.c
)

add_executable(stm32-template ${PROJECT_SOURCES})
target_link_libraries(stm32-template CMSIS::STM32::F407VG STM32::NoSys)
stm32_print_size_of_target(stm32-template)
