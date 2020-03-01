set(STM32_F0_TYPES 
    030x6 030x8 031x6 038xx 042x6 048xx 051x8 058xx 
    070x6 070xB 071xB 072xB 078xx 091xC 098xx 030xC
)
set(STM32_F0_TYPE_MATCH 
    "F030.[46]" "F030.8" "F031.[46]" "F038.." "F042.[46]" "F048.." "F051.[468]" "F058.."
    "F070.6" "F070.B" "F071.[8B]" "F072.[8B]" "F078.." "F091.[BC]" "F098.." "F030.C"
)
set(STM32_F0_RAM_SIZES 
     4K  8K  4K  4K  6K  6K  8K  8K
     6K 16K 16K 16K 16K 32K 32K 32K
)
set(STM32_F0_CCRAM_SIZES 
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  0K  0K  0K  0K
)

stm32_util_create_family_targets(F0)

target_compile_options(STM32::F0 INTERFACE 
    -mcpu=cortex-m0
)
target_link_options(STM32::F0 INTERFACE 
    -mcpu=cortex-m0
)
