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
set(STM32_H7_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
)

stm32_util_create_family_targets(H7)

target_compile_options(STM32::H7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::H7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_compile_definitions(STM32::H7 INTERFACE 
    -DCORE_CM7
)
