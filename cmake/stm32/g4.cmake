set(STM32_G4_TYPES 
    G431xx G441xx G471xx G473xx G483xx G474xx G484xx
    G491xx G4A1xx
)
set(STM32_G4_TYPE_MATCH 
    "G431.." "G441.." "G471.." "G473.." "G483.." "G474.." "G484.."
    "G491.." "G4A1.."
)
set(STM32_G4_RAM_SIZES 
     32K  32K 128K 128K 128K 128K 128K
    112K 112K
)
set(STM32_G4_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K   0K
      0K   0K
)

stm32_util_create_family_targets(G4)

target_compile_options(STM32::G4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::G4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)

list(APPEND STM32_ALL_DEVICES
    G431C6
    G431C8
    G431CB
    G431K6
    G431K8
    G431KB
    G431M6
    G431M8
    G431MB
    G431R6
    G431R8
    G431RB
    G431V6
    G431V8
    G431VB
    G441CB
    G441KB
    G441MB
    G441RB
    G441VB
    G471CC
    G471CE
    G471MC
    G471ME
    G471QC
    G471QE
    G471RC
    G471RE
    G471VC
    G471VE
    G473CB
    G473CC
    G473CE
    G473MB
    G473MC
    G473ME
    G473PB
    G473PC
    G473PE
    G473QB
    G473QC
    G473QE
    G473RB
    G473RC
    G473RE
    G473VB
    G473VC
    G473VE
    G474CB
    G474CC
    G474CE
    G474MB
    G474MC
    G474ME
    G474PB
    G474PC
    G474PE
    G474QB
    G474QC
    G474QE
    G474RB
    G474RC
    G474RE
    G474VB
    G474VC
    G474VE
    G483CE
    G483ME
    G483PE
    G483QE
    G483RE
    G483VE
    G484CE
    G484ME
    G484PE
    G484QE
    G484RE
    G484VE
    G491CC
    G491KC
    G491MC
    G491RC
    G491VC
    G491CE
    G491KE
    G491ME
    G491RE
    G491VE
    G4A1CE
    G4A1KE
    G4A1ME
    G4A1RE
    G4A1VE
    GBK1CB
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32G4
)

list(APPEND STM32_FETCH_FAMILIES G4)

set(CUBE_G4_VERSION  v1.4.0)
set(CMSIS_G4_VERSION v1.2.1)
set(HAL_G4_VERSION   v1.2.1)
