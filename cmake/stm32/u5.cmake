set(STM32_U5_TYPES 
    U575xx U585xx
)
set(STM32_U5_TYPE_MATCH 
   "U575.." "U585.."
)

set(STM32_U5_RAM_SIZES 
    768K 768K
)

stm32_util_create_family_targets(U5)

target_compile_options(STM32::U5 INTERFACE 
    -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::U5 INTERFACE 
    -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)

list(APPEND STM32_ALL_DEVICES
    U575CG
    U575CI
    U585CI
    U575RG
    U575RI
    U585RI
    U5750G
    U5750I
    U5850I
    U575VG
    U575VI
    U585VI
    U575QG
    U575QI
    U585QI
    U575ZG
    U575ZI
    U585ZI
    U575AG
    U575AI
    U585AI
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32U5
)

list(APPEND STM32_FETCH_FAMILIES U5)

set(CUBE_U5_VERSION  v1.5.0)
set(CMSIS_U5_VERSION v1.4.0)
set(HAL_U5_VERSION   v1.5.0)
