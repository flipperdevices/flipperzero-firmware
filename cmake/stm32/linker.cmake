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
    
    if((NOT CCRAM_SIZE) OR (CCRAM_SIZE STREQUAL "0K"))
        set(CCRAM_DEFINITION "")
        set(CCRAM_SECTION "")
    else()
        set(CCRAM_DEFINITION "    CCMRAM (rw) : ORIGIN = ${CCRAM_ORIGIN}, LENGTH = ${CCRAM_SIZE}\n")
        set(CCRAM_SECTION "
_siccmram = LOADADDR(.ccmram);\n\
.ccmram :\n\
{\n\
. = ALIGN(4);\n\
_sccmram = .;\n\
*(.ccmram)\n\
*(.ccmram*)\n\
. = ALIGN(4);\n\
_eccmram = .;\n\
} >CCMRAM AT> FLASH\n\
        ")
    endif()
    
    set(SCRIPT_TEXT 
"ENTRY(Reset_Handler)\n\
\n\
_estack = ${RAM_ORIGIN} + ${RAM_SIZE};\n\
_Min_Heap_Size = ${HEAP_SIZE};\n\
_Min_Stack_Size = ${STACK_SIZE};\n\
\n\
MEMORY\n\
{\n\
    FLASH (rx)      : ORIGIN = ${FLASH_ORIGIN}, LENGTH = ${FLASH_SIZE}\n\
    RAM (xrw)      : ORIGIN = ${RAM_ORIGIN}, LENGTH = ${RAM_SIZE}\n\
${CCRAM_DEFINITION}\n\
}\n\
\n\
SECTIONS\n\
{\n\
  .isr_vector :\n\
  {\n\
    . = ALIGN(4);\n\
    KEEP(*(.isr_vector))\n\
    . = ALIGN(4);\n\
  } >FLASH\n\
\n\
  .text :\n\
  {\n\
    . = ALIGN(4);\n\
    *(.text)\n\
    *(.text*)\n\
    *(.glue_7)\n\
    *(.glue_7t)\n\
    *(.eh_frame)\n\
\n\
    KEEP (*(.init))\n\
    KEEP (*(.fini))\n\
\n\
    . = ALIGN(4);\n\
    _etext = .;\n\
  } >FLASH\n\
\n\
  .rodata :\n\
  {\n\
    . = ALIGN(4);\n\
    *(.rodata)\n\
    *(.rodata*)\n\
    . = ALIGN(4);\n\
  } >FLASH\n\
\n\
  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH\n\
  .ARM : {\n\
    __exidx_start = .;\n\
    *(.ARM.exidx*)\n\
    __exidx_end = .;\n\
  } >FLASH\n\
\n\
  .preinit_array     :\n\
  {\n\
    PROVIDE_HIDDEN (__preinit_array_start = .);\n\
    KEEP (*(.preinit_array*))\n\
    PROVIDE_HIDDEN (__preinit_array_end = .);\n\
  } >FLASH\n\
  .init_array :\n\
  {\n\
    PROVIDE_HIDDEN (__init_array_start = .);\n\
    KEEP (*(SORT(.init_array.*)))\n\
    KEEP (*(.init_array*))\n\
    PROVIDE_HIDDEN (__init_array_end = .);\n\
  } >FLASH\n\
  .fini_array :\n\
  {\n\
    PROVIDE_HIDDEN (__fini_array_start = .);\n\
    KEEP (*(SORT(.fini_array.*)))\n\
    KEEP (*(.fini_array*))\n\
    PROVIDE_HIDDEN (__fini_array_end = .);\n\
  } >FLASH\n\
\n\
  _sidata = LOADADDR(.data);\n\
\n\
  .data : \n\
  {\n\
    . = ALIGN(4);\n\
    _sdata = .; \n\
    *(.data)\n\
    *(.data*)\n\
\n\
    . = ALIGN(4);\n\
    _edata = .;\n\
  } >RAM AT> FLASH\n\
${CCRAM_SECTION}\n\
  . = ALIGN(4);\n\
  .bss :\n\
  {\n\
    _sbss = .;\n\
    __bss_start__ = _sbss;\n\
    *(.bss)\n\
    *(.bss*)\n\
    *(COMMON)\n\
\n\
    . = ALIGN(4);\n\
    _ebss = .;\n\
    __bss_end__ = _ebss;\n\
  } >RAM\n\
\n\
  ._user_heap_stack :\n\
  {\n\
    . = ALIGN(8);\n\
    PROVIDE ( end = . );\n\
    PROVIDE ( _end = . );\n\
    . = . + _Min_Heap_Size;\n\
    . = . + _Min_Stack_Size;\n\
    . = ALIGN(8);\n\
  } >RAM\n\
\n\
  /DISCARD/ :\n\
  {\n\
    libc.a ( * )\n\
    libm.a ( * )\n\
    libgcc.a ( * )\n\
  }\n\
\n\
  .ARM.attributes 0 : { *(.ARM.attributes) }\n\
}"
    )
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld CONTENT "${SCRIPT_TEXT}")
    stm32_set_linker_script(${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_flash.ld)
endfunction() 
