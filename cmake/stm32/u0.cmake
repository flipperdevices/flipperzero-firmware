set(STM32_U0_TYPES
    U031xx
    U073xx
    U083xx
)
set(STM32_U0_TYPE_MATCH 
    "U031.[468]"
    "U073.C"
    "U083.C"
)
set(STM32_U0_RAM_SIZES 
    12K
    40K
    40K
)
set(STM32_U0_CCRAM_SIZES 
     0K
     0K
     0K
)

stm32_util_create_family_targets(U0)

target_compile_options(STM32::U0 INTERFACE 
    -mcpu=cortex-m0plus
)
target_link_options(STM32::U0 INTERFACE 
    -mcpu=cortex-m0plus
)
