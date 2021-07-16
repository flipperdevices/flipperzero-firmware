set(STM32_WB_TYPES 
    WB55xx WB55xx WB35xx WB15xx WB50xx WB30xx WB10xx WB5Mxx
)
set(STM32_WB_TYPE_MATCH 
   "WB55.C" "WB55.[EGY]" "WB35.." "WB15.." "WB50.." "WB30.." "WB10.." "WB5M.."
)

set(STM32_WB_RAM_SIZES 
    128K 256K  96K  48K 128K  96K  48K 256K
)
set(STM32_WB_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K   0K
)

stm32_util_create_family_targets(WB M4)

target_compile_options(STM32::WB::M4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::WB::M4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
