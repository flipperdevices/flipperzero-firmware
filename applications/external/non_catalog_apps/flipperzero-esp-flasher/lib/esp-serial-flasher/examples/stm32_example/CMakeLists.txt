cmake_minimum_required(VERSION 3.16)

set(FLASHER_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
set(CMAKE_TOOLCHAIN_FILE ${FLASHER_DIR}/submodules/stm32-cmake/cmake/stm32_gcc.cmake)

project(stm32_flasher C ASM)

find_package(CMSIS COMPONENTS STM32H7_M7 REQUIRED)
find_package(HAL COMPONENTS STM32H7_M7 PWR RCC GPIO UART CORTEX REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} Core/Inc)

include(${CMAKE_CURRENT_LIST_DIR}/../common/bin2array.cmake)
create_resources(${CMAKE_CURRENT_LIST_DIR}/../binaries/Hello-world Core/Src/binaries.c)

add_executable(${CMAKE_PROJECT_NAME} 
    ../common/example_common.c
    Core/Src/main.c
    Core/Src/stm32h7xx_hal_msp.c
    Core/Src/stm32h7xx_it.c
    Core/Src/libc_compat.c
    Core/Src/binaries.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ../common)

set(PORT STM32)
set(STM32_CHIP STM32H743VI)
set(CORE_USED M7)
add_subdirectory(${FLASHER_DIR} ${CMAKE_BINARY_DIR}/flasher)

target_link_libraries(${CMAKE_PROJECT_NAME}
PRIVATE
    HAL::STM32::H7::M7::PWREx
    HAL::STM32::H7::M7::RCCEx
    HAL::STM32::H7::M7::GPIO
    HAL::STM32::H7::M7::UARTEx
    HAL::STM32::H7::M7::CORTEX
    CMSIS::STM32::H743VI::M7
    STM32::Nano
    flasher
)

stm32_print_size_of_target(${CMAKE_PROJECT_NAME})
stm32_generate_binary_file(${CMAKE_PROJECT_NAME})
stm32_generate_hex_file(${CMAKE_PROJECT_NAME})
