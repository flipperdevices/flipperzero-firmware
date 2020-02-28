set(STM32F4_TYPES 
    401xC 401xE 405xx 407xx 410Cx 410Rx 410Tx 411xE 
    412Cx 412Rx 412Vx 412Zx 413xx 415xx 417xx 423xx
    427xx 429xx 437xx 439xx 446xx 469xx 479xx
)
set(STM32F4_TYPE_MATCH 
    "F401.[CB]" "F401.[ED]" "F405.." "F407.." "F410C." "F410R." "F410T." "F411.[CE]"
    "F412C." "F412R." "F412V." "F412Z." "F413.." "F415.." "F417.." "F423.."
    "F427.." "F429.." "F437.." "F439.." "F446.." "F469.." "F479.."
)
set(STM32F4_RAM_SIZES 
     64K  96K 128K 128K  32K  32K  32K 128K
    256K 256K 256K 256K 256K 128K 128K 320K
    192K 192K 192K 192K 128K 320K 320K
)

set(STM32F4_CCRAM_SIZES 
     0K  0K 64K 64K  0K  0K  0K  0K
     0K  0K  0K  0K 64K 64K 64K  0K
    64K 64K 64K 64K  0K 64K 64K
)

target_compile_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_compile_definitions(STM32::F4 INTERFACE 
    STM32F4
)

foreach(TYPE ${STM32F4_TYPES})
    if(NOT (TARGET STM32::F4::${TYPE}))
        add_library(STM32::F4::${TYPE} INTERFACE IMPORTED)
        target_link_libraries(STM32::F4::${TYPE} INTERFACE STM32::F4)
        target_compile_definitions(STM32::F4::${TYPE} INTERFACE 
            STM32F${TYPE}
        )
    endif()
endforeach()

function(stm32f4_get_type DEVICE TYPE)
    set(INDEX 0)
    foreach(C_TYPE ${STM32F4_TYPES})
        list(GET STM32F4_TYPE_MATCH ${INDEX} REGEXP)
        if(DEVICE MATCHES ${REGEXP})
            set(RESULT_TYPE ${C_TYPE})
        endif()
        math(EXPR INDEX "${INDEX}+1")
    endforeach()
    if(NOT RESULT_TYPE)
        message(FATAL_ERROR "Invalid/unsupported STM32F4 device: ${DEVICE}")
    endif()
    set(${TYPE} ${RESULT_TYPE} PARENT_SCOPE)
endfunction()

function(stm32f4_memory_info DEVICE 
        FLASH_SIZE RAM_SIZE CCRAM_SIZE STACK_SIZE HEAP_SIZE 
        FLASH_ORIGIN RAM_ORIGIN CCRAM_ORIGIN
)
    string(REGEX REPLACE "^F4[0-9][0-9].([8BCDEGHI])$" "\\1" SIZE_CODE ${DEVICE})
    
    if(SIZE_CODE STREQUAL "8")
        set(FLASH "64K")
    elseif(SIZE_CODE STREQUAL "B")
        set(FLASH "128K")
    elseif(SIZE_CODE STREQUAL "C")
        set(FLASH "256K")
    elseif(SIZE_CODE STREQUAL "D")
        set(FLASH "384K")
    elseif(SIZE_CODE STREQUAL "E")
        set(FLASH "512K")
    elseif(SIZE_CODE STREQUAL "G")
        set(FLASH "1024K")
    elseif(SIZE_CODE STREQUAL "H")
        set(FLASH "1536K")
    elseif(SIZE_CODE STREQUAL "I")
        set(FLASH "2048K")
    else()
        set(FLASH "64K")
        message(WARNING "Unknow flash size for device ${DEVICE}")
    endif()
        
    stm32f4_get_type(${DEVICE} TYPE)
    list(FIND STM32F4_TYPES ${TYPE} TYPE_INDEX)
    list(GET STM32F4_RAM_SIZES ${TYPE_INDEX} RAM)
    list(GET STM32F4_CCRAM_SIZES ${TYPE_INDEX} CCRAM)
    
    set(${FLASH_SIZE} ${FLASH} PARENT_SCOPE)
    set(${RAM_SIZE} ${RAM} PARENT_SCOPE)
    set(${CCRAM_SIZE} ${CCRAM} PARENT_SCOPE)
    set(${STACK_SIZE} 0x400 PARENT_SCOPE)
    set(${HEAP_SIZE} 0x200 PARENT_SCOPE)
    set(${FLASH_ORIGIN} 0x8000000 PARENT_SCOPE)
    set(${RAM_ORIGIN} 0x20000000 PARENT_SCOPE)
    set(${CCRAM_ORIGIN} 0x10000000 PARENT_SCOPE)
endfunction()
