set(STM32_F0_TYPES 
    F030x6 F030x8 F031x6 F038xx F042x6 F048xx F051x8 F058xx 
    F070x6 F070xB F071xB F072xB F078xx F091xC F098xx F030xC
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

list(APPEND STM32_ALL_DEVICES
    F030C6
    F030C8
    F030CC
    F030F4
    F030K6
    F030R8
    F030RC
    F031C4
    F031C6
    F031E6
    F031F4
    F031F6
    F031G4
    F031G6
    F031K4
    F031K6
    F038C6
    F038E6
    F038F6
    F038G6
    F038K6
    F042C4
    F042C6
    F042F4
    F042F6
    F042G4
    F042G6
    F042K4
    F042K6
    F042T6
    F048C6
    F048G6
    F048T6
    F051C4
    F051C6
    F051C8
    F051K4
    F051K6
    F051K8
    F051R4
    F051R6
    F051R8
    F051T8
    F058C8
    F058R8
    F058T8
    F070C6
    F070CB
    F070F6
    F070RB
    F071C8
    F071CB
    F071RB
    F071V8
    F071VB
    F072C8
    F072CB
    F072R8
    F072RB
    F072V8
    F072VB
    F078CB
    F078RB
    F078VB
    F091CB
    F091CC
    F091RB
    F091RC
    F091VB
    F091VC
    F098CC
    F098RC
    F098VC
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32F0
)

list(APPEND STM32_FETCH_FAMILIES F0)

set(CUBE_F0_VERSION  v1.11.5)
set(CMSIS_F0_VERSION v2.3.7)
set(HAL_F0_VERSION   v1.7.8)
