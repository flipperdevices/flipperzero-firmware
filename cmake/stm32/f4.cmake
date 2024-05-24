set(STM32_F4_TYPES 
    F401xC F401xE F405xx F407xx F410Cx F410Rx F410Tx F411xE 
    F412Cx F412Rx F412Vx F412Zx F413xx F415xx F417xx F423xx
    F427xx F429xx F437xx F439xx F446xx F469xx F479xx
)
set(STM32_F4_TYPE_MATCH 
    "F401.[CB]" "F401.[ED]" "F405.." "F407.." "F410C." "F410R." "F410T." "F411.[CE]"
    "F412C." "F412R." "F412V." "F412Z." "F413.." "F415.." "F417.." "F423.."
    "F427.." "F429.." "F437.." "F439.." "F446.." "F469.." "F479.."
)
set(STM32_F4_RAM_SIZES 
     64K  96K 128K 128K  32K  32K  32K 128K
    256K 256K 256K 256K 320K 128K 128K 320K
    192K 192K 192K 192K 128K 320K 320K
)
set(STM32_F4_CCRAM_SIZES 
     0K  0K 64K 64K  0K  0K  0K  0K
     0K  0K  0K  0K  0K 64K 64K  0K
    64K 64K 64K 64K  0K 64K 64K
)

stm32_util_create_family_targets(F4)

target_compile_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)

list(APPEND STM32_ALL_DEVICES
    F401CB
    F401CC
    F401CD
    F401CE
    F401RB
    F401RC
    F401RD
    F401RE
    F401VB
    F401VC
    F401VD
    F401VE
    F405OE
    F405OG
    F405RG
    F405VG
    F405ZG
    F407IE
    F407IG
    F407VE
    F407VG
    F407ZE
    F407ZG
    F410C8
    F410CB
    F410R8
    F410RB
    F410T8
    F410TB
    F411CC
    F411CE
    F411RC
    F411RE
    F411VC
    F411VE
    F412CE
    F412CG
    F412RE
    F412RG
    F412VE
    F412VG
    F412ZE
    F412ZG
    F413CG
    F413CH
    F413MG
    F413MH
    F413RG
    F413RH
    F413VG
    F413VH
    F413ZG
    F413ZH
    F415OG
    F415RG
    F415VG
    F415ZG
    F417IE
    F417IG
    F417VE
    F417VG
    F417ZE
    F417ZG
    F423CH
    F423MH
    F423RH
    F423VH
    F423ZH
    F427AG
    F427AI
    F427IG
    F427II
    F427VG
    F427VI
    F427ZG
    F427ZI
    F429AG
    F429AI
    F429BE
    F429BG
    F429BI
    F429IE
    F429IG
    F429II
    F429NE
    F429NG
    F429NI
    F429VE
    F429VG
    F429VI
    F429ZE
    F429ZG
    F429ZI
    F437AI
    F437IG
    F437II
    F437VG
    F437VI
    F437ZG
    F437ZI
    F439AI
    F439BG
    F439BI
    F439IG
    F439II
    F439NG
    F439NI
    F439VG
    F439VI
    F439ZG
    F439ZI
    F446MC
    F446ME
    F446RC
    F446RE
    F446VC
    F446VE
    F446ZC
    F446ZE
    F469AE
    F469AG
    F469AI
    F469BE
    F469BG
    F469BI
    F469IE
    F469IG
    F469II
    F469NE
    F469NG
    F469NI
    F469VE
    F469VG
    F469VI
    F469ZE
    F469ZG
    F469ZI
    F479AG
    F479AI
    F479BG
    F479BI
    F479IG
    F479II
    F479NG
    F479NI
    F479VG
    F479VI
    F479ZG
    F479ZI
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32F4
)

list(APPEND STM32_FETCH_FAMILIES F4)

set(CUBE_F4_VERSION  v1.28.0)
set(CMSIS_F4_VERSION v2.6.9)
set(HAL_F4_VERSION   v1.8.2)
