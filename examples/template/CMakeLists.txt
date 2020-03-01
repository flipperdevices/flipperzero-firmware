cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-template C ASM)

find_package(CMSIS COMPONENTS STM32F407VG REQUIRED)

set(PROJECT_SOURCES
    main.c
)

add_executable(stm32-template.elf ${PROJECT_SOURCES})
target_link_libraries(stm32-template.elf CMSIS::STM32::F407VG STM32::NoSys)
