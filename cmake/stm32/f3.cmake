set(STM32_F3_TYPES 
    F301x8 F302x8 F302xC F302xE F303x8 F303xC 
    F303xE F318xx F328xx F334x8 F358xx F373xC 
    F378xx F398xx 
)
set(STM32_F3_TYPE_MATCH 
    "301.[68]" "302.[68]" "302.[BC]"  "302.[DE]" "303.[68]"  "303.[BC]" 
    "303.[DE]"    "318.."    "328.." "334.[468]"    "358.." "373.[8BC]"
    "378.." "398.."    
)
set(STM32_F3_RAM_SIZES 
     16K  16K   0K  64K  12K   0K  
     64K  16K  12K  12K  40K   0K
     32K  64K
)
set(STM32_F3_CCRAM_SIZES 
      0K   0K   0K   0K   4K   8K  
     16K   0K   4K   4K   8K   0K
      0K  16K
)

stm32_util_create_family_targets(F3)

target_compile_options(STM32::F3 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F3 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)

function(stm32f3_get_memory_info DEVICE TYPE FLASH_SIZE RAM_SIZE)
    string(REGEX REPLACE "F3[0-9][0-9].([468BCDEFGHI])" "\\1" SIZE_CODE ${DEVICE})
    
    if(TYPE STREQUAL "F302xC")
        if(SIZE_CODE STREQUAL "C")
            set(RAM "40K")
        else()
            set(RAM "32K")
        endif()
    elseif(TYPE STREQUAL "F303xC")
        if(SIZE_CODE STREQUAL "C")
            set(RAM "40K")
        else()
            set(RAM "32K")
        endif()
    elseif(TYPE STREQUAL "F373xC")
        if(SIZE_CODE STREQUAL "B")
            set(RAM "24K")
        elseif(SIZE_CODE STREQUAL "C")
            set(RAM "32K")
        else()
            set(RAM "16K")
        endif()
    endif()
    
    if(RAM)
        set(${RAM_SIZE} ${RAM} PARENT_SCOPE)
    endif()
endfunction()

list(APPEND STM32_ALL_DEVICES
    F301C6
    F301C8
    F301K6
    F301K8
    F301R6
    F301R8
    F302C6
    F302C8
    F302CB
    F302CC
    F302K6
    F302K8
    F302R6
    F302R8
    F302RB
    F302RC
    F302RD
    F302RE
    F302VB
    F302VC
    F302VD
    F302VE
    F302ZD
    F302ZE
    F303C6
    F303C8
    F303CB
    F303CC
    F303K6
    F303K8
    F303R6
    F303R8
    F303RB
    F303RC
    F303RD
    F303RE
    F303VB
    F303VC
    F303VD
    F303VE
    F303ZD
    F303ZE
    F318C8
    F318K8
    F328C8
    F334C4
    F334C6
    F334C8
    F334K4
    F334K6
    F334K8
    F334R6
    F334R8
    F358CC
    F358RC
    F358VC
    F373C8
    F373CB
    F373CC
    F373R8
    F373RB
    F373RC
    F373V8
    F373VB
    F373VC
    F378CC
    F378RC
    F378VC
    F398VE
)

list(APPEND STM32_SUPPORTED_FAMILIES_LONG_NAME
    STM32F3
)

list(APPEND STM32_FETCH_FAMILIES F3)

set(CUBE_F3_VERSION  v1.11.5)
set(CMSIS_F3_VERSION v2.3.8)
set(HAL_F3_VERSION   v1.5.8)
