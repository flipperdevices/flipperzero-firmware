set(STM32_SUPPORTED_FAMILIES L0 L1 L4 F0 F1 F2 F3 F4 F7 H7)

if(NOT STM32_TOOLCHAIN_PATH)
     set(STM32_TOOLCHAIN_PATH "/usr")
     message(STATUS "No STM32_TOOLCHAIN_PATH specified, using default: " ${STM32_TOOLCHAIN_PATH})
else()
     file(TO_CMAKE_PATH "${STM32_TOOLCHAIN_PATH}" STM32_TOOLCHAIN_PATH)
endif()

if(NOT STM32_TARGET_TRIPLET)
    set(STM32_TARGET_TRIPLET "arm-none-eabi")
    message(STATUS "No STM32_TARGET_TRIPLET specified, using default: " ${STM32_TARGET_TRIPLET})
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_SYSROOT  "${STM32_TOOLCHAIN_PATH}/${STM32_TARGET_TRIPLET}")
set(TOOLCHAIN_BIN_PATH "${STM32_TOOLCHAIN_PATH}/bin")
set(TOOLCHAIN_INC_PATH "${STM32_TOOLCHAIN_PATH}/${STM32_TARGET_TRIPLET}/include")
set(TOOLCHAIN_LIB_PATH "${STM32_TOOLCHAIN_PATH}/${STM32_TARGET_TRIPLET}/lib")

find_program(CMAKE_OBJCOPY NAMES ${STM32_TARGET_TRIPLET}-objcopy PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${STM32_TARGET_TRIPLET}-objdump PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_SIZE NAMES ${STM32_TARGET_TRIPLET}-size PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_DEBUGGER NAMES ${STM32_TARGET_TRIPLET}-gdb PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CPPFILT NAMES ${STM32_TARGET_TRIPLET}-c++filt PATHS ${TOOLCHAIN_BIN_PATH})

function(stm32_get_chip_type FAMILY DEVICE TYPE)
    if(${FAMILY} STREQUAL "F4")
        stm32f4_get_type(${DEVICE} T)
    endif()
    set(${TYPE} ${T} PARENT_SCOPE)
endfunction()

function(stm32_get_chip_info CHIP FAMILY TYPE DEVICE)
    string(TOUPPER ${CHIP} CHIP)
        
    string(REGEX MATCH "^STM32([A-Z][0-9])([0-9][0-9][A-Z][0-9A-Z]).*$" CHIP ${CHIP})
    
    if((NOT CMAKE_MATCH_1) OR (NOT CMAKE_MATCH_2))
        message(FATAL_ERROR "Unknown chip ${CHIP}")
    endif()
    
    set(STM32_FAMILY ${CMAKE_MATCH_1})
    set(STM32_DEVICE "${CMAKE_MATCH_1}${CMAKE_MATCH_2}")
    
    list(FIND STM32_SUPPORTED_FAMILIES ${STM32_FAMILY} STM32_FAMILY_INDEX)
    if (STM32_FAMILY_INDEX EQUAL -1)
        message(FATAL_ERROR "Unsupported family ${STM32_FAMILY} for device ${CHIP}")
    endif()

    stm32_get_chip_type(${STM32_FAMILY} ${STM32_DEVICE} STM32_TYPE)
    
    set(${FAMILY} ${STM32_FAMILY} PARENT_SCOPE)
    set(${DEVICE} ${STM32_DEVICE} PARENT_SCOPE)
    set(${TYPE} ${STM32_TYPE} PARENT_SCOPE)
endfunction()

foreach(FAMILY ${STM32_SUPPORTED_FAMILIES})
    if(NOT (TARGET STM32::${FAMILY}))
        add_library(STM32::${FAMILY} INTERFACE IMPORTED)
        target_compile_options(STM32::${FAMILY} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -mabi=aapcs -Wall -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -ffast-math
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Release>:-Os>
        )
        target_link_options(STM32::${FAMILY} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -mabi=aapcs -Wl,--gc-sections
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Release>:-Os -s>
        )
    endif()
endforeach()

if(NOT (TARGET STM32::NoSys))
    add_library(STM32::NoSys INTERFACE IMPORTED)
    target_compile_options(STM32::NoSys INTERFACE $<$<C_COMPILER_ID:GNU>:--specs=nosys.specs>)
    target_link_options(STM32::NoSys INTERFACE $<$<C_COMPILER_ID:GNU>:--specs=nosys.specs>)
endif()

include(stm32/f4)

