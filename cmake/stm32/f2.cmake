set(STM32_F2_TYPES 
    F205xx F215xx F207xx F217xx
)
set(STM32_F2_TYPE_MATCH 
    "F205.." "F215.." "F207.." "F217.."
)
set(STM32_F2_RAM_SIZES 
      0K 128K 128K 128K 
)
set(STM32_F2_CCRAM_SIZES 
      0K   0K   0K   0K
)

stm32_util_create_family_targets(F2)

target_compile_options(STM32::F2 INTERFACE 
    -mcpu=cortex-m3
)
target_link_options(STM32::F2 INTERFACE 
    -mcpu=cortex-m3
)

function(stm32f2_get_memory_info DEVICE TYPE FLASH_SIZE RAM_SIZE)
    string(REGEX REPLACE "F2[0-9][0-9].([468BCDEFGHI])" "\\1" SIZE_CODE ${DEVICE})
    
    if(TYPE STREQUAL "F205xx")
        if(SIZE_CODE STREQUAL "B")
            set(RAM "64K")
        elseif(SIZE_CODE STREQUAL "C")
            set(RAM "96K")
        else()
            set(RAM "128K")
        endif()
    endif()
    
    if(RAM)
        set(${RAM_SIZE} ${RAM} PARENT_SCOPE)
    endif()
endfunction()

list(APPEND STM32_ALL_DEVICES
    F205RB
    F205RC
    F205RE
    F205RF
    F205RG
    F205VB
    F205VC
    F205VE
    F205VF
    F205VG
    F205ZC
    F205ZE
    F205ZF
    F205ZG
    F207IC
    F207IE
    F207IF
    F207IG
    F207VC
    F207VE
    F207VF
    F207VG
    F207ZC
    F207ZE
    F207ZF
    F207ZG
    F215RE
    F215RG
    F215VE
    F215VG
    F215ZE
    F215ZG
    F217IE
    F217IG
    F217VE
    F217VG
    F217ZE
    F217ZG
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32F2
)

list(APPEND STM32_FETCH_FAMILIES F2)

set(CUBE_F2_VERSION  v1.9.3)
set(CMSIS_F2_VERSION v2.2.5)
set(HAL_F2_VERSION   v1.2.7)
