cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

project(stm32-blinky-ll C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS COMPONENTS STM32L0 STM32F1 STM32F4 REQUIRED)
find_package(HAL COMPONENTS STM32L0 STM32F1 STM32F4 LL_RCC LL_GPIO REQUIRED)

# STM32F4-Discovery
add_executable(stm32-blinky-f4 blinky.c)
target_link_libraries(stm32-blinky-f4
    HAL::STM32::F4::LL_RCC
    HAL::STM32::F4::LL_GPIO
    CMSIS::STM32::F407VG
    STM32::NoSys 
)
stm32_print_size_of_target(stm32-blinky-f4)

# STM32VL-Discovery
add_executable(stm32-blinky-f1 blinky.c)
target_link_libraries(stm32-blinky-f1
    HAL::STM32::F1::LL_RCC
    HAL::STM32::F1::LL_GPIO
    CMSIS::STM32::F100RB
    STM32::NoSys
)
stm32_print_size_of_target(stm32-blinky-f1)

# STM32L0538-Discovery
add_executable(stm32-blinky-l0 blinky.c)
target_link_libraries(stm32-blinky-l0
    HAL::STM32::L0::LL_RCC
    HAL::STM32::L0::LL_GPIO
    CMSIS::STM32::L053C8
    STM32::NoSys
)
stm32_print_size_of_target(stm32-blinky-l0)
