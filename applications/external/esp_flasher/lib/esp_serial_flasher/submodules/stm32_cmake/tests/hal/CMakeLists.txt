cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

if(NOT TEST_FAMILIES)
    set(TEST_FAMILIES C0 F0 F1 F2 F3 F4 F7 G0 G4 H7 L0 L1 L4 L5 MP1 U5 WB WL)
endif()

# Generate the family long names list by prepending STM32 to elements in TEST_FAMILIES
list(TRANSFORM TEST_FAMILIES PREPEND STM32 OUTPUT_VARIABLE TEST_FAMILIES_LONG_NAMES)

project(hal-test C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

if(FETCH_ST_SOURCES)
    stm32_fetch_cmsis(${TEST_FAMILIES})
    stm32_fetch_hal(${TEST_FAMILIES})
endif()

find_package(CMSIS REQUIRED ${TEST_FAMILIES_LONG_NAMES})
find_package(HAL REQUIRED ${TEST_FAMILIES_LONG_NAMES})

set(SOURCES main.c)

foreach(FAMILY ${TEST_FAMILIES})
    stm32_get_devices_by_family(STM_DEVICES FAMILY ${FAMILY})
    list(GET STM_DEVICES 0 DEVICE)
    stm32_get_cores(CORES FAMILY ${FAMILY} DEVICE ${DEVICE})
    
    if(CORES)
        list(GET CORES 0 CORE)
        set(CORE "::${CORE}")
    else()
        unset(CORE)
    endif()
    
    add_executable(hal-test-${FAMILY} ${SOURCES})
    target_compile_definitions(hal-test-${FAMILY} PRIVATE USE_FULL_LL_DRIVER)
    
    foreach(DRIVER ${HAL_DRIVERS_${FAMILY}})
        string(TOUPPER ${DRIVER} DRIVER)
        target_link_libraries(hal-test-${FAMILY} HAL::STM32::${FAMILY}${CORE}::${DRIVER})
    endforeach()
    foreach(DRIVER ${HAL_EX_DRIVERS_${FAMILY}})
        string(TOUPPER ${DRIVER} DRIVER)
        target_link_libraries(hal-test-${FAMILY} HAL::STM32::${FAMILY}${CORE}::${DRIVER}Ex)
    endforeach()
    foreach(DRIVER ${HAL_LL_DRIVERS_${FAMILY}})
        string(TOUPPER ${DRIVER} DRIVER)
        target_link_libraries(hal-test-${FAMILY} HAL::STM32::${FAMILY}${CORE}::LL_${DRIVER})
    endforeach()
    target_link_libraries(hal-test-${FAMILY} STM32::NoSys CMSIS::STM32::${DEVICE}${CORE})
endforeach()
