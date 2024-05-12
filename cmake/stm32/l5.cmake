set(STM32_L5_TYPES 
    L552xx L562xx
)
set(STM32_L5_TYPE_MATCH 
   "L552.." "L562.."
)

set(STM32_L5_RAM_SIZES 
    256K 256K
)
set(STM32_L5_CCRAM_SIZES 
      0K   0K
)

stm32_util_create_family_targets(L5)

target_compile_options(STM32::L5 INTERFACE 
    -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::L5 INTERFACE 
    -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)

list(APPEND STM32_ALL_DEVICES
    L552CC
    L552CE
    L552ME
    L552QC
    L552QE
    L552RC
    L552RE
    L552VC
    L552VE
    L552ZC
    L552ZE
    L562CE
    L562ME
    L562QE
    L562RE
    L562VE
    L562ZE
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32L5
)

list(APPEND STM32_FETCH_FAMILIES L5)

set(CUBE_L5_VERSION  v1.5.1)
set(CMSIS_L5_VERSION v1.0.6)
set(HAL_L5_VERSION   v1.0.6)
