set(STM32_L4_TYPES 
    L412xx L422xx L431xx L432xx L433xx L442xx 
    L443xx L451xx L452xx L462xx L471xx L475xx 
    L476xx L485xx L486xx L496xx L4A6xx L4P5xx 
    L4Q5xx L4R5xx L4R7xx L4R9xx L4S5xx L4S7xx 
    L4S9xx
)
set(STM32_L4_TYPE_MATCH 
   "L412.." "L422.." "L431.." "L432.." "L433.." "L442.." 
   "L443.." "L451.." "L452.." "L462.." "L471.." "L475.." 
   "L476.." "L485.." "L486.." "L496.." "L4A6.." "L4P5.." 
   "L4Q5.." "L4R5.." "L4R7.." "L4R9.." "L4S5.." "L4S7.." 
   "L4S9.."
)

set(STM32_L4_RAM_SIZES 
     40K  40K  64K  64K  64K  64K
     64K 160K 160K 160K  96K  96K
     96K  96K  96K 320K 320K 320K 
    320K 640K 640K 640K 640K 640K
    640K
)
# on devices where CCRAM is remapped to be contiguous with RAM it is included into RAM section
# If you want to have dedicated section then you will need to use custom linker script
set(STM32_L4_CCRAM_SIZES 
      0K   0K   0K   0K   0K   0K
      0K   0K   0K   0K  32K  32K
     32K  32K  32K   0K   0K   0K
      0K   0K   0K   0K   0K   0K
      0K
)

stm32_util_create_family_targets(L4)

target_compile_options(STM32::L4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::L4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)

list(APPEND STM32_ALL_DEVICES
    L412C8
    L412CB
    L412K8
    L412KB
    L412R8
    L412RB
    L412T8
    L412TB
    L422CB
    L422KB
    L422RB
    L422TB
    L431CB
    L431CC
    L431KB
    L431KC
    L431RB
    L431RC
    L431VC
    L432KB
    L432KC
    L433CB
    L433CC
    L433RB
    L433RC
    L433VC
    L442KC
    L443CC
    L443RC
    L443VC
    L451CC
    L451CE
    L451RC
    L451RE
    L451VC
    L451VE
    L452CC
    L452CE
    L452RC
    L452RE
    L452VC
    L452VE
    L462CE
    L462RE
    L462VE
    L471QE
    L471QG
    L471RE
    L471RG
    L471VE
    L471VG
    L471ZE
    L471ZG
    L475RC
    L475RE
    L475RG
    L475VC
    L475VE
    L475VG
    L476JE
    L476JG
    L476ME
    L476MG
    L476QE
    L476QG
    L476RC
    L476RE
    L476RG
    L476VC
    L476VE
    L476VG
    L476ZE
    L476ZG
    L486JG
    L486QG
    L486RG
    L486VG
    L486ZG
    L496AE
    L496AG
    L496QE
    L496QG
    L496RE
    L496RG
    L496VE
    L496VG
    L496ZE
    L496ZG
    L4A6AG
    L4A6QG
    L4A6RG
    L4A6VG
    L4A6ZG
    L4P5AE
    L4P5AG
    L4P5CE
    L4P5CG
    L4P5QE
    L4P5QG
    L4P5RE
    L4P5RG
    L4P5VE
    L4P5VG
    L4P5ZE
    L4P5ZG
    L4Q5AG
    L4Q5CG
    L4Q5QG
    L4Q5RG
    L4Q5VG
    L4Q5ZG
    L4R5AG
    L4R5AI
    L4R5QG
    L4R5QI
    L4R5VG
    L4R5VI
    L4R5ZG
    L4R5ZI
    L4R7AI
    L4R7VI
    L4R7ZI
    L4R9AG
    L4R9AI
    L4R9VG
    L4R9VI
    L4R9ZG
    L4R9ZI
    L4S5AI
    L4S5QI
    L4S5VI
    L4S5ZI
    L4S7AI
    L4S7VI
    L4S7ZI
    L4S9AI
    L4S9VI
    L4S9ZI
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32L4
)

list(APPEND STM32_FETCH_FAMILIES L4)

set(CUBE_L4_VERSION  v1.18.0)
set(CMSIS_L4_VERSION v1.7.3)
set(HAL_L4_VERSION   v1.13.4)
