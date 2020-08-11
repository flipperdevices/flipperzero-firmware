# Only CM7 core is supported for now...

set(STM32_H7_TYPES 
    H743xx H753xx H750xx H742xx H745xx H755xx H747xx H757xx
    H7A3xx H7A3xxQ H7B3xx H7B3xxQ H7B0xx H7B0xxQ
)
set(STM32_H7_TYPE_MATCH 
   "H743.." "H753.." "H750.." "H742.." "H745.." "H755.." "H747.." "H757.."
   "H7A3.." "H7A3..Q" "H7B3.." "H7B3..Q" "H7B0.." "H7B0..Q"
)
set(STM32_H7_RAM_SIZES
    128K 128K 128K 128K 128K 128K 128K 128K
    128K 128K 128K 128K 128K 128K 
)
set(STM32_H7_M4_RAM_SIZES
    288K 288K 288K 288K 288K 288K 288K 288K
    288K 288K 288K 288K 288K 288K 
)

set(STM32_H7_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
)

stm32_util_create_family_targets(H7 M7)

target_compile_options(STM32::H7::M7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::H7::M7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_compile_definitions(STM32::H7::M7 INTERFACE 
    -DCORE_CM7
)

stm32_util_create_family_targets(H7 M4)

target_compile_options(STM32::H7::M4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::H7::M4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_compile_definitions(STM32::H7::M4 INTERFACE 
    -DCORE_CM4
)

function(stm32h7_get_memory_info DEVICE TYPE CORE RAM FLASH_ORIGIN RAM_ORIGIN)
    list(FIND STM32_H7_TYPES ${TYPE} TYPE_INDEX)
    if(CORE STREQUAL "_M7")
        list(GET STM32_H7_RAM_SIZES ${TYPE_INDEX} RAM_VALUE)
        set(${RAM} ${RAM_VALUE} PARENT_SCOPE)
        set(${FLASH_ORIGIN} 0x8000000 PARENT_SCOPE)
        set(${RAM_ORIGIN} 0x20000000 PARENT_SCOPE)
    elseif(CORE STREQUAL "_M4")
        list(GET STM32_H7_M4_RAM_SIZES ${TYPE_INDEX} RAM_VALUE)
        set(${RAM} ${RAM_VALUE} PARENT_SCOPE)
        set(${FLASH_ORIGIN} 0x8100000 PARENT_SCOPE)
        set(${RAM_ORIGIN} 0x10000000 PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown core ${CORE}")
    endif()
endfunction()
