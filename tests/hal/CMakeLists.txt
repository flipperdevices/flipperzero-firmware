cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

if(NOT TEST_FAMILIES)
    set(TEST_FAMILIES F0 F1 F2 F3 F4 F7 G0 G4 H7 L0 L1 L4)
endif()

project(hal-test C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS REQUIRED)
find_package(HAL REQUIRED)

set(SOURCES main.c)

foreach(FAMILY ${TEST_FAMILIES})
    stm32_get_devices_by_family(${FAMILY} DEVICES)
    list(GET DEVICES 0 DEVICE)
    
    add_executable(hal-test-${FAMILY} ${SOURCES})
    
    foreach(DRIVER ${HAL_DRIVERS_${FAMILY}})
        string(TOUPPER ${DRIVER} DRIVER)
        target_link_libraries(hal-test-${FAMILY} HAL::STM32::${FAMILY}::${DRIVER})
    endforeach()
    foreach(DRIVER ${HAL_EX_DRIVERS_${FAMILY}})
        string(TOUPPER ${DRIVER} DRIVER)
        target_link_libraries(hal-test-${FAMILY} HAL::STM32::${FAMILY}::${DRIVER}Ex)
    endforeach()
    foreach(DRIVER ${HAL_LL_DRIVERS_${FAMILY}})
        string(TOUPPER ${DRIVER} DRIVER)
        target_link_libraries(hal-test-${FAMILY} HAL::STM32::${FAMILY}::LL_${DRIVER})
    endforeach()
    target_link_libraries(hal-test-${FAMILY} STM32::NoSys CMSIS::STM32::${DEVICE})
endforeach()
