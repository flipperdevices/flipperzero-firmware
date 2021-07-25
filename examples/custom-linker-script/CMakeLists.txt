cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-custom-linker-script C ASM)

find_package(CMSIS COMPONENTS STM32F407VG REQUIRED)

set(PROJECT_SOURCES
    main.c
)

add_executable(stm32-custom-linker-script ${PROJECT_SOURCES})
target_link_libraries(stm32-custom-linker-script CMSIS::STM32::F407xx STM32::NoSys)
stm32_add_linker_script(stm32-custom-linker-script PRIVATE F407VG.ld)
stm32_print_size_of_target(stm32-custom-linker-script)
