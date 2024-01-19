set(STM32_WB_TYPES 
    WB55xx WB55xx WB35xx WB15xx WB50xx WB30xx WB10xx WB5Mxx
)
set(STM32_WB_TYPE_MATCH 
   "WB55.C" "WB55.[EGY]" "WB35.." "WB15.." "WB50.." "WB30.." "WB10.." "WB5M.."
)

# this is not full RAM of the chip but only the part allocated to M4 core (SRAM1 in datasheet)
set(STM32_WB_RAM_SIZES 
     64K 192K  32K  12K 64K  32K  12K 192K
)

# WB series need special area for SRAM2 shared with core M0PLUS
set(STM32_WB_RAM_SHARE_SIZES 
     10K  10K  10K  10K  10K  10K  10K  10K
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

function(stm32wb_get_memory_info DEVICE TYPE CORE RAM RAM_ORIGIN TWO_FLASH_BANKS)
    set(${TWO_FLASH_BANKS} TRUE PARENT_SCOPE)
    list(FIND STM32_WB_TYPES ${TYPE} TYPE_INDEX)
    list(GET STM32_WB_RAM_SIZES ${TYPE_INDEX} RAM_VALUE)
    set(${RAM} "${RAM_VALUE}-4" PARENT_SCOPE)
    set(${RAM_ORIGIN} 0x20000004 PARENT_SCOPE)
endfunction()

