set(STM32_H5_TYPES 
    H503xx H562xx H563xx H573xx
)

set(STM32_H5_TYPE_MATCH 
    "H503.." "H562.." "H563.." "H573.."
)
set(STM32_H5_RAM_SIZES 
    32K 640K 640K 640K
)
set(STM32_H5_CCRAM_SIZES 
    0K  0K  0K  0K
)

stm32_util_create_family_targets(H5)

target_compile_options(STM32::H5 INTERFACE 
    -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb
)

target_link_options(STM32::H5 INTERFACE 
    -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb
)

list(APPEND STM32_ALL_DEVICES
    H503CB
    H503EB
    H503KB
    H503RB
    H562AG
    H562AI
    H562IG
    H562II
    H562RG
    H562RI
    H562VG
    H562VI
    H562ZG
    H562ZI
    H563AG
    H563AI
    H563IG
    H563II
    H563MI
    H563RG
    H563RI
    H563VG
    H563VI
    H563ZG
    H563ZI
    H573AI
    H573II
    H573MI
    H573RI
    H573VI
    H573ZI
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32H5
)

list(APPEND STM32_FETCH_FAMILIES H5)

set(CUBE_H5_VERSION  v1.1.0)
set(CMSIS_H5_VERSION v1.1.0)
set(HAL_H5_VERSION   v1.1.0)
