set(STM32_F7_TYPES 
    F756xx F746xx F745xx F765xx F767xx F769xx F777xx F779xx
    F722xx F723xx F732xx F733xx F730xx F750xx
)
set(STM32_F7_TYPE_MATCH 
    "F756.." "F746.." "F745.." "F765.." "F767.." "F769.." "F777.." "F77[89].."
    "F722.." "F723.." "F732.." "F733.." "F730.." "F750.."
)
set(STM32_F7_RAM_SIZES
    320K 320K 320K 512K 512K 512K 512K 512K
    256K 256K 256K 256K 256K 320K 
)
set(STM32_F7_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
)

stm32_util_create_family_targets(F7)

target_compile_options(STM32::F7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
