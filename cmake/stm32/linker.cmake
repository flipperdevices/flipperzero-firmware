function(stm32_set_linker_script TARGET SCRIPT)
    target_link_options(${TARGET} PRIVATE -T "${SCRIPT}")
endfunction()

function(stm32_generate_linker_script TARGET)
    stm32_get_chip(${TARGET} FAMILY DEVICE)
    
    get_target_property(FLASH_ORIGIN ${TARGET} STM32_FLASH_ORIGIN)
    get_target_property(RAM_ORIGIN ${TARGET} STM32_RAM_ORIGIN)
    get_target_property(CCRAM_ORIGIN ${TARGET} STM32_CCRAM_ORIGIN)
    get_target_property(FLASH_SIZE ${TARGET} STM32_FLASH_SIZE)
    get_target_property(RAM_SIZE ${TARGET} STM32_RAM_SIZE)
    get_target_property(CCRAM_SIZE ${TARGET} STM32_CCRAM_SIZE)
    get_target_property(STACK_SIZE ${TARGET} STM32_STACK_SIZE)
    get_target_property(HEAP_SIZE ${TARGET} STM32_HEAP_SIZE)
    
    if(NOT FLASH_ORIGIN)
        set(FLASH_ORIGIN 0x8000000)
    endif()
    if(NOT RAM_ORIGIN)
        set(RAM_ORIGIN 0x20000000)
    endif()
    if(NOT CCRAM_ORIGIN)
        set(CCRAM_ORIGIN 0x10000000)
    endif()
    
    if(${FAMILY} STREQUAL "F4")
        stm32f4_memory_size(${DEVICE} DEF_FLASH DEF_RAM DEF_CCRAM)
    endif()
    
    if(NOT FLASH_SIZE)
        set(FLASH_SIZE ${DEF_FLASH})
    endif()
    if(NOT RAM_SIZE)
        set(RAM_SIZE ${DEF_RAM})
    endif()
    if(NOT CCRAM_SIZE)
        set(CCRAM_SIZE ${DEF_CCRAM})
    endif()
    if(NOT STACK_SIZE)
        set(STACK_SIZE 0x400)
    endif()
    if(NOT HEAP_SIZE)
        set(HEAP_SIZE 0x200)
    endif()
    
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld"
        COMMAND ${CMAKE_COMMAND} 
            -DFLASH_ORIGIN="${FLASH_ORIGIN}" 
            -DRAM_ORIGIN="${RAM_ORIGIN}" 
            -DCCRAM_ORIGIN="${CCRAM_ORIGIN}" 
            -DFLASH_SIZE="${FLASH_SIZE}" 
            -DRAM_SIZE="${RAM_SIZE}" 
            -DCCRAM_SIZE="${CCRAM_SIZE}" 
            -DSTACK_SIZE="${STACK_SIZE}" 
            -DHEAP_SIZE="${HEAP_SIZE}" 
            -DLINKER_SCRIPT="${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld"
            -P "${STM32_CMAKE_DIR}/stm32/linker_ld.cmake"
    )
    add_custom_target(${TARGET}_linker_script DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld)
    add_dependencies(${TARGET} ${TARGET}_linker_script)
    stm32_set_linker_script(${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld)
endfunction() 
