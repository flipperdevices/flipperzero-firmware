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
