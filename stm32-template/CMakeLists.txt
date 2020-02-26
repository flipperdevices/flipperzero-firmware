set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/stm32_gcc.cmake)
cmake_minimum_required(VERSION 3.8)

project(stm32-template C ASM)

set(PROJECT_SOURCES
    main.c
)

add_executable(stm32-template.elf ${PROJECT_SOURCES})
set_target_properties(stm32-template.elf PROPERTIES 
    STM32_CHIP STM32F407IG
    STM32_GCC_SPECS nosys
)
stm32_target(stm32-template.elf)
