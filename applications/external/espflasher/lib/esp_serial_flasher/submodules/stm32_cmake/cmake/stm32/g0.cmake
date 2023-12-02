set(STM32_G0_TYPES 
    G030xx G031xx G041xx G050xx G051xx G061xx
    G070xx G071xx G081xx G0B0xx G0B1xx G0C1xx
)
set(STM32_G0_TYPE_MATCH 
    "G030.." "G031.." "G041.." "G050.." "G051.." "G061.."
    "G070.." "G071.." "G081.." "G0B0.." "G0B1.." "G0C1.."
)
set(STM32_G0_RAM_SIZES 
      8K   8K   8K  18K  18K  18K
     36K  36K  36K 144K 144K 144K
)
set(STM32_G0_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
)

stm32_util_create_family_targets(G0)

target_compile_options(STM32::G0 INTERFACE 
    -mcpu=cortex-m0plus
)
target_link_options(STM32::G0 INTERFACE 
    -mcpu=cortex-m0plus
)
