cmake_minimum_required(VERSION 3.8)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

if(NOT TEST_FAMILIES)
    set(TEST_FAMILIES F0)
endif()

set(TEST_F0_BOARDS 
    STM32F0xx_Nucleo_32 STM32F0xx_Nucleo STM32F072B_Discovery 
    STM32F0308_Discovery STM32072B_EVAL STM32091C_EVAL
)
set(TEST_F0_COMPONENTS
    HX8347D L3GD20 SPFD5408 ST7735 STLM75
)
set(DEVICE_STM32F0xx_Nucleo_32 F031K6)
set(DEVICE_STM32F0xx_Nucleo F030R8)

project(bsp-test C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS REQUIRED)
find_package(HAL REQUIRED)
find_package(BSP COMPONENTS STM32F0 REQUIRED)

set(SOURCES main.c)

foreach(FAMILY ${TEST_FAMILIES})    
    foreach(BOARD ${TEST_${FAMILY}_BOARDS})
        add_executable(bsp-test-${BOARD} ${SOURCES})
        target_link_libraries(bsp-test-${BOARD} 
            BSP::STM32::${BOARD} 
            HAL::STM32::${FAMILY}
            STM32::NoSys
        )
        if(DEVICE_${BOARD})
            target_link_libraries(bsp-test-${BOARD} CMSIS::STM32::${DEVICE_${BOARD}})
        endif()        
        foreach(COMP ${TEST_${FAMILY}_COMPONENTS})
            target_link_libraries(bsp-test-${BOARD} BSP::STM32::${FAMILY}::${COMP})
        endforeach()
    endforeach()
endforeach()
