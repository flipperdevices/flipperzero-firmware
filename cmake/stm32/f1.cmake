set(STM32_F1_TYPES 
    F100xB F100xE F101x6 F101xB F101xE F101xG F102x6 F102xB 
    F103x6 F103xB F103xE F103xG F105xC F107xC
)
set(STM32_F1_TYPE_MATCH 
    "F100.[468B]" "F100.[CDE]" "F101.[46]" "F101.[8B]" "F101.[CDE]" "F101.[FG]" "F102.[46]" "F102.[8B]" 
    "F103.[46]" "F103.[8B]" "F103.[CDE]" "F103.[FG]" "F105.[8BC]" "F107.[BC]"
)
set(STM32_F1_RAM_SIZES 
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  0K  0K
)
set(STM32_F1_CCRAM_SIZES 
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  0K  0K
)

stm32_util_create_family_targets(F1)

target_compile_options(STM32::F1 INTERFACE 
    -mcpu=cortex-m3
)
target_link_options(STM32::F1 INTERFACE 
    -mcpu=cortex-m3
)

function(stm32f1_get_memory_info DEVICE TYPE FLASH_SIZE RAM_SIZE)
    string(REGEX REPLACE "F1[0-9][0-9].([468BCDEFGHI])" "\\1" SIZE_CODE ${DEVICE})
    
    if((TYPE STREQUAL "F100xB") OR (TYPE STREQUAL "F100xE"))
        if((SIZE_CODE STREQUAL "4") OR (SIZE_CODE STREQUAL "6"))
            set(RAM "4K")
        elseif((SIZE_CODE STREQUAL "8") OR (SIZE_CODE STREQUAL "B"))
            set(RAM "8K")
        elseif(SIZE_CODE STREQUAL "C")
            set(RAM "24K")
        elseif((SIZE_CODE STREQUAL "D") OR (SIZE_CODE STREQUAL "E"))
            set(RAM "32K")
        endif()
    elseif((TYPE STREQUAL "F101x6") OR (TYPE STREQUAL "F101xB") OR 
           (TYPE STREQUAL "F101xE") OR (TYPE STREQUAL "F101xG") OR
           (TYPE STREQUAL "F102x6") OR (TYPE STREQUAL "F102xB"))
        if(SIZE_CODE STREQUAL "4")
            set(RAM "4K")
        elseif(SIZE_CODE STREQUAL "6")
            set(RAM "6K")
        elseif(SIZE_CODE STREQUAL "8")
            set(RAM "10K")
        elseif(SIZE_CODE STREQUAL "B")
            set(RAM "16K")
        elseif(SIZE_CODE STREQUAL "C")
            set(RAM "32K")
        elseif((SIZE_CODE STREQUAL "D") OR (SIZE_CODE STREQUAL "E"))
            set(RAM "48K")
        elseif((SIZE_CODE STREQUAL "F") OR (SIZE_CODE STREQUAL "G"))
            set(RAM "80K")
        endif()
    elseif((TYPE STREQUAL "F103x6") OR (TYPE STREQUAL "F103xB") OR
           (TYPE STREQUAL "F103xE") OR (TYPE STREQUAL "F103xG"))
        if(SIZE_CODE STREQUAL "4")
            set(RAM "6K")
        elseif(SIZE_CODE STREQUAL "6")
            set(RAM "10K")
        elseif((SIZE_CODE STREQUAL "8") OR (SIZE_CODE STREQUAL "B"))
            set(RAM "20K")
        elseif(SIZE_CODE STREQUAL "C")
            set(RAM "48K")
        elseif((SIZE_CODE STREQUAL "D") OR (SIZE_CODE STREQUAL "E"))
            set(RAM "64K")
        elseif((SIZE_CODE STREQUAL "F") OR (SIZE_CODE STREQUAL "G"))
            set(RAM "96K")
        endif()
    elseif((TYPE STREQUAL "F105xC") OR (TYPE STREQUAL "F107xC"))
        set(RAM "64K")
    endif()
    
    set(${RAM_SIZE} ${RAM} PARENT_SCOPE)
endfunction()

list(APPEND STM32_ALL_DEVICES
    F100C4
    F100C6
    F100C8
    F100CB
    F100R4
    F100R6
    F100R8
    F100RB
    F100RC
    F100RD
    F100RE
    F100V8
    F100VB
    F100VC
    F100VD
    F100VE
    F100ZC
    F100ZD
    F100ZE
    F101C4
    F101C6
    F101C8
    F101CB
    F101R4
    F101R6
    F101R8
    F101RB
    F101RC
    F101RD
    F101RE
    F101RF
    F101RG
    F101T4
    F101T6
    F101T8
    F101TB
    F101V8
    F101VB
    F101VC
    F101VD
    F101VE
    F101VF
    F101VG
    F101ZC
    F101ZD
    F101ZE
    F101ZF
    F101ZG
    F102C4
    F102C6
    F102C8
    F102CB
    F102R4
    F102R6
    F102R8
    F102RB
    F103C4
    F103C6
    F103C8
    F103CB
    F103R4
    F103R6
    F103R8
    F103RB
    F103RC
    F103RD
    F103RE
    F103RF
    F103RG
    F103T4
    F103T6
    F103T8
    F103TB
    F103V8
    F103VB
    F103VC
    F103VD
    F103VE
    F103VF
    F103VG
    F103ZC
    F103ZD
    F103ZE
    F103ZF
    F103ZG
    F105R8
    F105RB
    F105RC
    F105V8
    F105VB
    F105VC
    F107RB
    F107RC
    F107VB
    F107VC
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32F1
)

list(APPEND STM32_FETCH_FAMILIES F1)

set(CUBE_F1_VERSION  v1.8.4)
set(CMSIS_F1_VERSION v4.3.3)
set(HAL_F1_VERSION   v1.1.8)
