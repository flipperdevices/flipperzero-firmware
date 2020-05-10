cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

if(NOT TEST_FAMILIES)
    set(TEST_FAMILIES F0 G0 L0 F1 L1 F2 F3 F4 G4 L4 F7 H7)
endif()

project(cmsis-test C ASM)

find_package(CMSIS REQUIRED)

set(SOURCES main.c)

include(stm32/devices)

foreach(FAMILY ${TEST_FAMILIES})
    stm32_get_devices_by_family(${FAMILY} DEVICES)
    foreach(DEVICE ${DEVICES})    
        stm32_get_chip_type(${FAMILY} ${DEVICE} TYPE)
        stm32_get_memory_info(${FAMILY} ${DEVICE} 
            FLASH_SIZE RAM_SIZE CCRAM_SIZE STACK_SIZE HEAP_SIZE 
            FLASH_ORIGIN RAM_ORIGIN CCRAM_ORIGIN
        )
        message(STATUS "STM32${DEVICE}: ${FAMILY} family, type ${TYPE}, ${FLASH_SIZE} flash, ${RAM_SIZE} RAM")
        add_executable(cmsis-test-${DEVICE} ${SOURCES})
        target_link_libraries(cmsis-test-${DEVICE} CMSIS::STM32::${DEVICE} STM32::NoSys)
    endforeach()
endforeach()
