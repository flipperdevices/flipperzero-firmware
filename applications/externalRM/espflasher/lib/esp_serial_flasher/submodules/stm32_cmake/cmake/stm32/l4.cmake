set(STM32_L4_TYPES 
    L412xx L422xx L431xx L432xx L433xx L442xx 
    L443xx L451xx L452xx L462xx L471xx L475xx 
    L476xx L485xx L486xx L496xx L4A6xx L4P5xx 
    L4Q5xx L4R5xx L4R7xx L4R9xx L4S5xx L4S7xx 
    L4S9xx
)
set(STM32_L4_TYPE_MATCH 
   "L412.." "L422.." "L431.." "L432.." "L433.." "L442.." 
   "L443.." "L451.." "L452.." "L462.." "L471.." "L475.." 
   "L476.." "L485.." "L486.." "L496.." "L4A6.." "L4P5.." 
   "L4Q5.." "L4R5.." "L4R7.." "L4R9.." "L4S5.." "L4S7.." 
   "L4S9.."
)

set(STM32_L4_RAM_SIZES 
     40K  40K  64K  64K  64K  64K
     64K 160K 160K 160K  96K  96K
     96K  96K  96K 320K 320K 320K 
    320K 640K 640K 640K 640K 640K
    640K
)
# on devices where CCRAM is remapped to be contiguous with RAM it is included into RAM section
# If you want to have dedicated section then you will need to use custom linker script
set(STM32_L4_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K  32K  32K
     32K  32K  32K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
      0K
)

stm32_util_create_family_targets(L4)

target_compile_options(STM32::L4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::L4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
