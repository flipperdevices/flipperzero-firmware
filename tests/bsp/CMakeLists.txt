cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

if(NOT TEST_FAMILIES)
    set(TEST_FAMILIES F0 F1 F2 F3 F4 F7)
endif()

set(DEVICE_STM32F0xx_Nucleo_32 F031K6)
set(DEVICE_STM32F0xx_Nucleo F030R8)
set(DEVICE_STM32F3xx_Nucleo_32 F303K8)
set(DEVICE_STM32F3xx_Nucleo F334R8)
set(DEVICE_STM32F4xx_Nucleo_144 F429ZI)
set(DEVICE_STM32F4xx_Nucleo F446RE)
set(DEVICE_STM32F7xx_Nucleo_144 F746ZG)
set(DEFINES_STM32469I_EVAL USE_IOEXPANDER)
set(DEFINES_STM32F769I_EVAL USE_IOEXPANDER) 

# Ban some boards because their BSP need non-free components
set(BANNED_BOARDS STM32756G_EVAL)

project(bsp-test C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS REQUIRED)
find_package(HAL REQUIRED)
find_package(BSP REQUIRED)

set(SOURCES main.c)

foreach(FAMILY ${TEST_FAMILIES})    
    foreach(BOARD ${BSP_${FAMILY}_BOARDS})
        string(REPLACE "-" "_" BOARD ${BOARD})
        
        if(BOARD IN_LIST BANNED_BOARDS)
            continue()
        endif()
        
        add_executable(bsp-test-${BOARD} ${SOURCES})
        target_link_libraries(bsp-test-${BOARD} 
            BSP::STM32::${BOARD} 
            HAL::STM32::${FAMILY}
            STM32::NoSys
        )
        if(DEVICE_${BOARD})
            target_link_libraries(bsp-test-${BOARD} CMSIS::STM32::${DEVICE_${BOARD}})
        endif()
        if(DEFINES_${BOARD})
            target_compile_definitions(bsp-test-${BOARD} PRIVATE ${DEFINES_${BOARD}})
        endif()
        foreach(COMP ${BSP_${FAMILY}_COMPONENTS})
            string(TOUPPER ${COMP} COMP)
            # Workaround - F3 has both CS43L22 and CS43L52 that conflicts
            if((FAMILY STREQUAL F3) AND (COMP STREQUAL CS43L22))
                continue()
            endif()
            target_link_libraries(bsp-test-${BOARD} BSP::STM32::${FAMILY}::${COMP})
        endforeach()
    endforeach()
endforeach()
