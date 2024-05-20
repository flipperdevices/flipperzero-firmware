set(STM32_F7_TYPES 
    F756xx F746xx F745xx F765xx F767xx F769xx F777xx F779xx
    F722xx F723xx F732xx F733xx F730xx F750xx
)
set(STM32_F7_TYPE_MATCH 
    "F756.." "F746.." "F745.." "F765.." "F767.." "F769.." "F777.." "F77[89].."
    "F722.." "F723.." "F732.." "F733.." "F730.." "F750.."
)
set(STM32_F7_RAM_SIZES
    320K 320K 320K 512K 512K 512K 512K 512K
    256K 256K 256K 256K 256K 320K 
)
set(STM32_F7_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
)

stm32_util_create_family_targets(F7)

target_compile_options(STM32::F7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F7 INTERFACE 
    -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard
)

list(APPEND STM32_ALL_DEVICES
    F722IC
    F722IE
    F722RC
    F722RE
    F722VC
    F722VE
    F722ZC
    F722ZE
    F723IC
    F723IE
    F723VE
    F723ZC
    F723ZE
    F730I8
    F730R8
    F730V8
    F730Z8
    F732IE
    F732RE
    F732VE
    F732ZE
    F733IE
    F733VE
    F733ZE
    F745IE
    F745IG
    F745VE
    F745VG
    F745ZE
    F745ZG
    F746BE
    F746BG
    F746IE
    F746IG
    F746NE
    F746NG
    F746VE
    F746VG
    F746ZE
    F746ZG
    F750N8
    F750V8
    F750Z8
    F756BG
    F756IG
    F756NG
    F756VG
    F756ZG
    F765BG
    F765BI
    F765IG
    F765II
    F765NG
    F765NI
    F765VG
    F765VI
    F765ZG
    F765ZI
    F767BG
    F767BI
    F767IG
    F767II
    F767NG
    F767NI
    F767VG
    F767VI
    F767ZG
    F767ZI
    F769AI
    F769BG
    F769BI
    F769IG
    F769II
    F769NG
    F769NI
    F777BI
    F777II
    F777NI
    F777VI
    F777ZI
    F778AI
    F779AI
    F779BI
    F779II
    F779NI
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32F7
)

list(APPEND STM32_FETCH_FAMILIES F7)

set(CUBE_F7_VERSION  v1.16.1)
set(CMSIS_F7_VERSION v1.2.6)
set(HAL_F7_VERSION   v1.2.9)
