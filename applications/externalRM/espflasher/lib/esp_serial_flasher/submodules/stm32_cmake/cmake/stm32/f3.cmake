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
