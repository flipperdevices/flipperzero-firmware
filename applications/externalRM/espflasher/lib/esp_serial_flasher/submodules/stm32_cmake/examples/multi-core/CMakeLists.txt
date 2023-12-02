cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-template C ASM)

#Find CMSIS for both cores
find_package(CMSIS COMPONENTS STM32H757VG_M4 STM32H757VG_M7 REQUIRED)
find_package(HAL COMPONENTS STM32H757VG_M4 STM32H757VG_M7 REQUIRED)

#Creating both binaries
add_executable(m7core main.c)
add_executable(m4core main.c)

target_include_directories(m7core PRIVATE Inc)
target_include_directories(m4core PRIVATE Inc)

target_link_libraries(m7core PRIVATE
    HAL::STM32::H7::M7::RCC
    HAL::STM32::H7::M7::GPIO
    HAL::STM32::H7::M7::CORTEX
    CMSIS::STM32::H757VG::M7
    STM32::NoSys
    )
stm32_print_size_of_target(m7core)

target_link_libraries(m4core PRIVATE
    HAL::STM32::H7::M4::RCC
    HAL::STM32::H7::M4::GPIO
    HAL::STM32::H7::M4::CORTEX
    CMSIS::STM32::H757VG::M4
    STM32::NoSys
    )
stm32_print_size_of_target(m4core)
