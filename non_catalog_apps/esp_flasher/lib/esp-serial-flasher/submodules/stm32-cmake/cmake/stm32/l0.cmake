set(STM32_L0_TYPES 
    L010x4 L010x6 L010x8 L010xB L011xx L021xx L031xx L041xx
    L051xx L052xx L053xx L061xx L062xx L063xx L071xx L072xx
    L073xx L081xx L082xx L083xx
)
set(STM32_L0_TYPE_MATCH 
    "L010.4" "L010.6" "L010.8" "L010.B" "L011.." "L021.." "L031.." "L041.."
    "L051.." "L052.." "L053.." "L061.." "L062.." "L063.." "L071.." "L072.."
    "L073.." "L081.." "L082.." "L083.."
)
set(STM32_L0_RAM_SIZES 
     2K  8K  8K 20K  2K  2K  8K  8K
     8K  8K  8K  8K  8K  8K 20K 20K
    20K 20K 20K 20K
)
set(STM32_L0_CCRAM_SIZES 
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  
)

stm32_util_create_family_targets(L0)

target_compile_options(STM32::L0 INTERFACE 
    -mcpu=cortex-m0plus
)
target_link_options(STM32::L0 INTERFACE 
    -mcpu=cortex-m0plus
)
