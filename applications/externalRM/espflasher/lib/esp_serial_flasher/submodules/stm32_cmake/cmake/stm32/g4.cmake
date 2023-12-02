set(STM32_G4_TYPES 
    G431xx G441xx G471xx G473xx G483xx G474xx G484xx
    G491xx G4A1xx
)
set(STM32_G4_TYPE_MATCH 
    "G431.." "G441.." "G471.." "G473.." "G483.." "G474.." "G484.."
    "G491.." "G4A1.."
)
set(STM32_G4_RAM_SIZES 
     32K  32K 128K 128K 128K 128K 128K
    112K 112K
)
set(STM32_G4_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K
      0K   0K
)

stm32_util_create_family_targets(G4)

target_compile_options(STM32::G4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::G4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
