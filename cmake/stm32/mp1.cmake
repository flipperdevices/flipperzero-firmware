set(STM32_MP1_TYPES 
    MP151Axx MP151Cxx MP151Dxx MP151Fxx
    MP153Axx MP153Cxx MP153Dxx MP153Fxx 
    MP157Axx MP157Cxx MP157Dxx MP157Fxx)

set(STM32_MP1_TYPE_MATCH
    "MP151AA.?" "MP151CA.?" "MP151DA.?" "MP151FA.?"  
    "MP153AA.?" "MP153CA.?" "MP153DA.?" "MP153FA.?"
    "MP157AA.?" "MP157CA.?" "MP157DA.?" "MP157FA.?")

set(STM32_MP1_RAM_SIZES
    384K 384K 384K 384K
    384K 384K 384K 384K
    384K 384K 384K 384K)

set(STM32_MP1_CCRAM_SIZES
    0K 0K 0K 0K
    0K 0K 0K 0K
    0K 0K 0K 0K)

stm32_util_create_family_targets(MP1 M4)

target_compile_options(STM32::MP1::M4 INTERFACE -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
target_link_options(STM32::MP1::M4 INTERFACE -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
target_compile_definitions(STM32::MP1::M4 INTERFACE -DCORE_CM4)

