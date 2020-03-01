set(STM32_F1_TYPES 
    100xB 100xE 101x6 101xB 101xE 101xG 102x6 102xB 
    103x6 103xB 103xE 103xG 105xC 107xC
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
    
    if((TYPE STREQUAL "100xB") OR (TYPE STREQUAL "100xE"))
        if((SIZE_CODE STREQUAL "4") OR (SIZE_CODE STREQUAL "6"))
            set(RAM "4K")
        elseif((SIZE_CODE STREQUAL "8") OR (SIZE_CODE STREQUAL "B"))
            set(RAM "8K")
        elseif(SIZE_CODE STREQUAL "C")
            set(RAM "24K")
        elseif((SIZE_CODE STREQUAL "D") OR (SIZE_CODE STREQUAL "E"))
            set(RAM "32K")
        endif()
    elseif((TYPE STREQUAL "101x6") OR (TYPE STREQUAL "101xB") OR 
           (TYPE STREQUAL "101xE") OR (TYPE STREQUAL "101xG") OR
           (TYPE STREQUAL "102x6") OR (TYPE STREQUAL "102xB"))
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
    elseif((TYPE STREQUAL "103x6") OR (TYPE STREQUAL "103xB") OR
           (TYPE STREQUAL "103xE") OR (TYPE STREQUAL "103xG"))
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
    elseif((TYPE STREQUAL "105xC") OR (TYPE STREQUAL "107xC"))
        set(RAM "64K")
    endif()
    
    set(${RAM_SIZE} ${RAM} PARENT_SCOPE)
endfunction()
