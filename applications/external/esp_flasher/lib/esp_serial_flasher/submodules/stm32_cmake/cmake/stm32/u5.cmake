set(STM32_U5_TYPES 
    U575xx U585xx
)
set(STM32_U5_TYPE_MATCH 
   "U575.." "U585.."
)

set(STM32_U5_RAM_SIZES 
    768K 768K
)

stm32_util_create_family_targets(U5)

target_compile_options(STM32::U5 INTERFACE 
    -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::U5 INTERFACE 
    -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
