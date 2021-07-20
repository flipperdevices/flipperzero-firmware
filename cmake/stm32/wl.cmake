set(STM32_WL_TYPES 
    WL54xx WL55xx WLE4xx WLE5xx WLE4xx WLE5xx WLE4xx WLE5xx 
)
set(STM32_WL_TYPE_MATCH 
   "WL54.." "WL55.." "WLE4.8" "WLE5.8" "WLE4.B" "WLE5.B" "WLE4.C" "WLE5.C" 
)

set(STM32_WL_RAM_SIZES 
     64K  64K  20K  20K  48K  48K  64K  64K
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

function(stm32wl_get_device_cores DEVICE TYPE CORES)
    if(${TYPE} IN_LIST STM32_WL_DUAL_CORE)
        set(${CORES} M4 M0PLUS PARENT_SCOPE)
    else()
        set(${CORES} M4 PARENT_SCOPE)
    endif()
endfunction()