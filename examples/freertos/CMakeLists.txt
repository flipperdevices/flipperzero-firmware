cmake_minimum_required(VERSION 3.13)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-freertos C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS COMPONENTS STM32F407VG REQUIRED)
find_package(HAL COMPONENTS STM32F407VG REQUIRED)
find_package(FreeRTOS COMPONENTS ARM_CM4F REQUIRED)

set(PROJECT_SOURCES
    main.c
    FreeRTOSConfig.h
)

add_executable(stm32-freertos ${PROJECT_SOURCES} stm32f4xx_hal_conf.h)
target_link_libraries(stm32-freertos PRIVATE
    FreeRTOS::Timers
    FreeRTOS::Heap::1
    FreeRTOS::ARM_CM4F 
    HAL::STM32::F4::RCC
    HAL::STM32::F4::GPIO
    HAL::STM32::F4::CORTEX
    CMSIS::STM32::F407VG 
    STM32::NoSys
)
stm32_print_size_of_target(stm32-freertos)
