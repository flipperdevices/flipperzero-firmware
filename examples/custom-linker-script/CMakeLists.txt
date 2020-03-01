cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-custom-linker-script C ASM)

find_package(CMSIS COMPONENTS STM32F407VG REQUIRED)

set(PROJECT_SOURCES
    main.c
)

add_executable(stm32-custom-linker-script.elf ${PROJECT_SOURCES})
target_link_libraries(stm32-custom-linker-script.elf CMSIS::STM32::F4::407xx STM32::NoSys)
stm32_add_linker_script(stm32-custom-linker-script.elf PRIVATE F407VG.ld)
