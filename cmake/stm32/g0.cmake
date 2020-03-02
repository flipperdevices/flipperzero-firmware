set(STM32_G0_TYPES 
    G030xx G031xx G041xx G070xx G071xx G081xx   
)
set(STM32_G0_TYPE_MATCH 
    "G030.." "G031.." "G041.." "G070.." "G071.." "G081.." 
)
set(STM32_G0_RAM_SIZES 
     8K  8K  8K 36K 36K 36K
)
set(STM32_G0_CCRAM_SIZES 
     0K  0K  0K  0K  0K  0K
)

stm32_util_create_family_targets(G0)

target_compile_options(STM32::G0 INTERFACE 
    -mcpu=cortex-m0plus
)
target_link_options(STM32::G0 INTERFACE 
    -mcpu=cortex-m0plus
)
