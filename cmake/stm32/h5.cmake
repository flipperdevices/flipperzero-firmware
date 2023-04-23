set(STM32_H5_TYPES 
    H503xx H562xx H563xx H573xx
)

set(STM32_H5_TYPE_MATCH 
    "H503.." "H562.." "H563.." "H573.."
)
set(STM32_H5_RAM_SIZES 
    32K 640K 640K 640K
)
set(STM32_H5_CCRAM_SIZES 
    0K  0K  0K  0K
)

stm32_util_create_family_targets(H5)

target_compile_options(STM32::H5 INTERFACE 
    -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb
)

target_link_options(STM32::H5 INTERFACE 
    -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb
)
