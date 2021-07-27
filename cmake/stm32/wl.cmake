set(STM32_WL_TYPES 
    WL54xx WL55xx WLE4xx WLE5xx WLE4xx WLE5xx WLE4xx WLE5xx 
)
set(STM32_WL_TYPE_MATCH 
   "WL54.." "WL55.." "WLE4.8" "WLE5.8" "WLE4.B" "WLE5.B" "WLE4.C" "WLE5.C" 
)

# this is RAM size allocated to M4 core
# Note devices with 20 and 48K RAM can use only half of available RAM because 
# there are 2 split sections of RAM and our default linker script only manages 
# one section.
set(STM32_WL_RAM_SIZES 
     32K  32K  10K  10K  24K  24K  64K  64K
)

# this is RAM size allocated to M0PLUS core
set(STM32_WL_M0PLUS_RAM_SIZES 
     32K  32K   0K   0K   0K   0K   0K   0K
)

set(STM32_WL_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K   0K
)

set(STM32_WL_DUAL_CORE
      WL54xx WL55xx
)

stm32_util_create_family_targets(WL M4)

target_compile_options(STM32::WL::M4 INTERFACE 
    -mcpu=cortex-m4 -mfloat-abi=soft
)
target_link_options(STM32::WL::M4 INTERFACE 
    -mcpu=cortex-m4 -mfloat-abi=soft
)

stm32_util_create_family_targets(WL M0PLUS)

target_compile_options(STM32::WL::M0PLUS INTERFACE 
    -mcpu=cortex-m0plus -mfloat-abi=soft
)
target_link_options(STM32::WL::M0PLUS INTERFACE 
    -mcpu=cortex-m0plus -mfloat-abi=soft
)

function(stm32wl_get_memory_info DEVICE TYPE CORE RAM FLASH_ORIGIN RAM_ORIGIN TWO_FLASH_BANKS)
    if(${TYPE} IN_LIST STM32_WL_DUAL_CORE)
        set(${TWO_FLASH_BANKS} TRUE PARENT_SCOPE)  
    else()
        set(${TWO_FLASH_BANKS} FALSE PARENT_SCOPE)
    endif()
    list(FIND STM32_WL_TYPES ${TYPE} TYPE_INDEX)
    if(CORE STREQUAL "M4")
        list(GET STM32_WL_RAM_SIZES ${TYPE_INDEX} RAM_VALUE)
        set(${RAM} ${RAM_VALUE} PARENT_SCOPE)
        set(${FLASH_ORIGIN} 0x8000000 PARENT_SCOPE)
        set(${RAM_ORIGIN} 0x20000000 PARENT_SCOPE)
    elseif((${TYPE} IN_LIST STM32_WL_DUAL_CORE) AND (CORE STREQUAL "M0PLUS"))
        list(GET STM32_WL_M0PLUS_RAM_SIZES ${TYPE_INDEX} RAM_VALUE)
        set(${RAM} ${RAM_VALUE} PARENT_SCOPE)
        set(${FLASH_ORIGIN} 0x8020000 PARENT_SCOPE)
        set(${RAM_ORIGIN} 0x20008000 PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown core ${CORE}")
    endif()
endfunction()



function(stm32wl_get_device_cores DEVICE TYPE CORES)
    if(${TYPE} IN_LIST STM32_WL_DUAL_CORE)
        set(${CORES} M4 M0PLUS PARENT_SCOPE)
    else()
        set(${CORES} M4 PARENT_SCOPE)
    endif()
endfunction()