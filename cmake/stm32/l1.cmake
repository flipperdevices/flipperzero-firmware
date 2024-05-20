set(STM32_L1_TYPES 
    L100xB  L100xBA L100xC  L151xB  L151xBA L151xC  L151xCA L151xD 
    L151xDX L151xE  L152xB  L152xBA L152xC  L152xCA L152xD  L152xDX
    L152xE  L162xC  L162xCA L162xD  L162xDX L162xE
)
set(STM32_L1_TYPE_MATCH 
    "L100.[68B]" "L100.[68B]A" "L100.C" "L151.[68B]" "L151.[68B]A" "L151.C" "L151.CA" "L151.D"
    "L151.DX" "L151.E" "L152.[68B]" "L152.[68B]A" "L152.C" "L152.CA" "L152.D" "L152.DX"
    "L152.E" "L162.C" "L162.CA" "L162.D" "L162.DX" "L162.E"
)
set(STM32_L1_RAM_SIZES 
     0K  0K 16K  0K  0K 32K 32K 48K
    80K 80K  0K  0K 32K 32K 48K 80K
    80K 32K 32K 48K 80K 80K
)
set(STM32_L1_CCRAM_SIZES 
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  0K  0K  0K  0K
     0K  0K  0K  0K  0K  0K
)

stm32_util_create_family_targets(L1)

target_compile_options(STM32::L1 INTERFACE 
    -mcpu=cortex-m3
)
target_link_options(STM32::L1 INTERFACE 
    -mcpu=cortex-m3
)

function(stm32l1_get_memory_info DEVICE TYPE FLASH_SIZE RAM_SIZE)
    string(REGEX REPLACE "L1[0-9][0-9].([68BCDE])" "\\1" SIZE_CODE ${DEVICE})
    
    unset(RAM)
    
    if((TYPE STREQUAL "L100xB"))
        if(SIZE_CODE STREQUAL "6")
            set(RAM "4K")
        elseif(SIZE_CODE STREQUAL "8")
            set(RAM "8K")
        elseif(SIZE_CODE STREQUAL "B")
            set(RAM "10K")
        endif()
    elseif((TYPE STREQUAL "L100xBA"))
        if(SIZE_CODE STREQUAL "6")
            set(RAM "4K")
        elseif(SIZE_CODE STREQUAL "8")
            set(RAM "8K")
        elseif(SIZE_CODE STREQUAL "B")
            set(RAM "16K")
        endif()
    elseif((TYPE STREQUAL "L151xB") OR (TYPE STREQUAL "L152xB"))
        if(SIZE_CODE STREQUAL "6")
            set(RAM "10K")
        elseif(SIZE_CODE STREQUAL "8")
            set(RAM "10K")
        elseif(SIZE_CODE STREQUAL "B")
            set(RAM "16K")
        endif()
    elseif((TYPE STREQUAL "L151xBA") OR (TYPE STREQUAL "L152xBA"))
        if(SIZE_CODE STREQUAL "6")
            set(RAM "16K")
        elseif(SIZE_CODE STREQUAL "8")
            set(RAM "32K")
        elseif(SIZE_CODE STREQUAL "B")
            set(RAM "32K")
        endif()
    endif()
    
    if(RAM)
        set(${RAM_SIZE} ${RAM} PARENT_SCOPE)
    endif()
endfunction()

list(APPEND STM32_ALL_DEVICES
    L100C6
    L100R8
    L100RB
    L100RC
    L151C6
    L151C8
    L151CB
    L151CC
    L151QC
    L151QD
    L151QE
    L151R6
    L151R8
    L151RB
    L151RC
    L151RD
    L151RE
    L151UC
    L151V8
    L151VB
    L151VC
    L151VD
    L151VE
    L151ZC
    L151ZD
    L151ZE
    L152C6
    L152C8
    L152CB
    L152CC
    L152QC
    L152QD
    L152QE
    L152R6
    L152R8
    L152RB
    L152RC
    L152RD
    L152RE
    L152UC
    L152V8
    L152VB
    L152VC
    L152VD
    L152VE
    L152ZC
    L152ZD
    L152ZE
    L162QC
    L162QD
    L162RC
    L162RD
    L162RE
    L162VC
    L162VD
    L162VE
    L162ZC
    L162ZD
    L162ZE
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32L1
)

list(APPEND STM32_FETCH_FAMILIES L1)

set(CUBE_L1_VERSION  v1.10.3)
set(CMSIS_L1_VERSION v2.3.2)
set(HAL_L1_VERSION   v1.4.4)
