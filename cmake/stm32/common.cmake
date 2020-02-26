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

SET(CMAKE_OBJCOPY "${TOOLCHAIN_BIN_PATH}/${STM32_TARGET_TRIPLET}-objcopy" CACHE INTERNAL "objcopy tool")
SET(CMAKE_OBJDUMP "${TOOLCHAIN_BIN_PATH}/${STM32_TARGET_TRIPLET}-objdump" CACHE INTERNAL "objdump tool")
SET(CMAKE_SIZE "${TOOLCHAIN_BIN_PATH}/${STM32_TARGET_TRIPLET}-size" CACHE INTERNAL "size tool tool")
SET(CMAKE_DEBUGGER "${TOOLCHAIN_BIN_PATH}/${STM32_TARGET_TRIPLET}-gdb" CACHE INTERNAL "debugger tool")
SET(CMAKE_CPPFILT "${TOOLCHAIN_BIN_PATH}/${STM32_TARGET_TRIPLET}-c++filt" CACHE INTERNAL "C++filt tool")

include(stm32/f4)
include(stm32/linker)
include(stm32/cmsis)

function(stm32_get_chip TARGET FAMILY DEVICE)
    get_target_property(CHIP ${TARGET} STM32_CHIP)
    string(TOUPPER ${CHIP} CHIP)
    
    if(NOT CHIP)
        message(FATAL_ERROR "Please specify STM32 chip for target ${TARGET} using STM32_CHIP property")
    endif()
    
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

    set(${FAMILY} ${STM32_FAMILY} PARENT_SCOPE)
    set(${DEVICE} ${STM32_DEVICE} PARENT_SCOPE)
endfunction()

function(stm32_get_type FAMILY DEVICE TYPE)
    if(${FAMILY} STREQUAL "F4")
        stm32f4_get_type(${DEVICE} T)
    endif()
    set(${TYPE} ${T} PARENT_SCOPE)
endfunction()

function(stm32_configure_compiler TARGET)
    stm32_get_chip(${TARGET} STM32_FAMILY STM32_DEVICE) 
    
    target_compile_options(${TARGET} PRIVATE 
        --sysroot="${TOOLCHAIN_SYSROOT}"
        -mthumb -mabi=aapcs -Wall -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -ffast-math
        $<$<CONFIG:Debug>:-Og>
        $<$<CONFIG:Release>:-Os>
    )
    target_link_options(${TARGET} PRIVATE 
        --sysroot="${TOOLCHAIN_SYSROOT}"
        -mthumb -mabi=aapcs -Wl,--gc-sections
        $<$<CONFIG:Debug>:-Og>
        $<$<CONFIG:Release>:-Os -s>
    )

    get_target_property(SPECS ${TARGET} STM32_GCC_SPECS)
    if(SPECS)
        target_compile_options(${TARGET} PRIVATE $<$<C_COMPILER_ID:GNU>:--specs=${SPECS}.specs>)
        target_link_options(${TARGET} PRIVATE $<$<C_COMPILER_ID:GNU>:--specs=${SPECS}.specs>)
    endif()
    
    if(${STM32_FAMILY} STREQUAL "F4")
        stm32f4_configure_compiler(${TARGET})
    endif()
endfunction()

function(stm32_target TARGET)
    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    stm32_configure_compiler(${TARGET})
    if(TARGET_TYPE STREQUAL EXECUTABLE)
        get_target_property(NO_LINKER_SCRIPT ${TARGET} STM32_NO_LINKER_SCRIPT)
        if(NOT NO_LINKER_SCRIPT)
            stm32_generate_linker_script(${TARGET})
        endif()
        get_target_property(NO_CMSIS ${TARGET} STM32_NO_CMSIS)
        if(NOT NO_CMSIS)
            stm32_add_cmsis(${TARGET})
        endif()
    endif()
endfunction()

# function(nrf52_add_sdk_startup TARGET)
#     get_target_property(TARGET_NO_SDK ${TARGET} STM32_NO_SDK)
#     if(TARGET_NO_SDK)
#         return()
#     endif()
# 
#     target_include_directories(${TARGET} PRIVATE "${NRF5_SDK_PATH}/components/toolchain/cmsis/include")
#     target_include_directories(${TARGET} PRIVATE "${NRF5_SDK_PATH}/modules/nrfx/mdk")
#     
#     nrf52_get_chip(${TARGET} STM32_CHIP STM32_CHIP_VARIANT)
#     
#     unset(STM32_STARTUP_FILE CACHE)
#     find_file(STM32_STARTUP_FILE
#         NAMES gcc_startup_nrf52${STM32_CHIP}.S gcc_startup_nrf52.S
#         PATHS "${NRF5_SDK_PATH}/modules/nrfx/mdk"
#         NO_DEFAULT_PATH
#     )
#     
#     unset(STM32_SYSTEM_FILE CACHE)
#     find_file(STM32_SYSTEM_FILE
#         NAMES system_nrf52${STM32_CHIP}.c system_nrf52.c
#         PATHS "${NRF5_SDK_PATH}/modules/nrfx/mdk"
#         NO_DEFAULT_PATH
#     )
#     
#     if((NOT STM32_STARTUP_FILE) OR (NOT STM32_SYSTEM_FILE))
#         message(WARNING "Cannot find startup sources for target ${TARGET}, check NRF5_SDK_PATH variable")
#     else()
#         target_sources(${TARGET} PRIVATE "${STM32_STARTUP_FILE}" "${STM32_SYSTEM_FILE}")
#     endif()
# endfunction()
# 
# function(nrf52_add_linker_script TARGET SCRIPT)
#     target_link_options(${TARGET} PRIVATE -T "${SCRIPT}")
#     target_link_options(${TARGET} PRIVATE -L "${NRF5_SDK_PATH}/modules/nrfx/mdk")
# endfunction()
# 
# function(nrf52_generate_linker_script TARGET)
#     get_target_property(TARGET_NO_LINKER_SCRIPT ${TARGET} STM32_NO_LINKER_SCRIPT)
#     if(TARGET_NO_LINKER_SCRIPT)
#         return()
#     endif()
#     set(STM32_LINKER_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.ld)
#     nrf52_linker_generate_script(${TARGET} "${STM32_LINKER_FILE}")
#     nrf52_add_linker_script(${TARGET} "${STM32_LINKER_FILE}")
# endfunction()
# 
# function(nrf52_configure_compiler TARGET)
#     nrf52_get_chip(${TARGET} STM32_CHIP STM32_CHIP_VARIANT)
#     
#     if(STM32_CHIP EQUAL 840)
#         target_compile_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16)
#         target_compile_definitions(${TARGET} PRIVATE -DFLOAT_ABI_HARD)
#         target_link_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16)
#     elseif(STM32_CHIP EQUAL 832)
#         target_compile_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16)
#         target_link_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16)
#         target_compile_definitions(${TARGET} PRIVATE -DFLOAT_ABI_HARD)
#     elseif(STM32_CHIP EQUAL 811)
#         target_compile_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=soft)
#         target_compile_definitions(${TARGET} PRIVATE -DFLOAT_ABI_SOFT)
#         target_link_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=soft)
#     elseif(STM32_CHIP EQUAL 810)
#         target_compile_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=soft)
#         target_compile_definitions(${TARGET} PRIVATE -DFLOAT_ABI_SOFT)
#         target_link_options(${TARGET} PRIVATE -mcpu=cortex-m4 -mfloat-abi=soft)
#     endif()
#     
#     target_compile_options(${TARGET} PRIVATE -mthumb -mabi=aapcs -Wall -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums $<$<CONFIG:Release>:-Os>)
#     target_compile_definitions(${TARGET} PRIVATE -DSTM32${STM32_CHIP}_XX${STM32_CHIP_VARIANT})
#     target_link_options(${TARGET} PRIVATE -mthumb -mabi=aapcs -Wl,--gc-sections --specs=nano.specs $<$<CONFIG:Release>:-Os>)
# endfunction()
# 
# function(nrf52_target TARGET)
#     get_target_property(TARGET_TYPE ${TARGET} TYPE)    
#     nrf52_configure_compiler(${TARGET})
#     if(TARGET_TYPE STREQUAL EXECUTABLE)
#         nrf52_add_sdk_startup(${TARGET})
#         nrf52_generate_linker_script(${TARGET})
#         target_link_libraries(${TARGET} PRIVATE -lc -lnosys -lm)
#     endif()
# endfunction()
