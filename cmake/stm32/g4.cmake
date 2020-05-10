set(STM32_G4_TYPES 
    G431xx G441xx G471xx G473xx G483xx G474xx G484xx
)
set(STM32_G4_TYPE_MATCH 
    "G431.." "G441.." "G471.." "G473.." "G483.." "G474.." "G484.."
)
set(STM32_G4_RAM_SIZES 
     22K  22K  96K  96K  96K  96K  96K
)
set(STM32_G4_CCRAM_SIZES 
     10K  10K  32K  32K  32K  32K  32K
)

stm32_util_create_family_targets(G4)

target_compile_options(STM32::G4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::G4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
