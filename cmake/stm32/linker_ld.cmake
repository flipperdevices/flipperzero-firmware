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

if((NOT RAM_SHARE_SIZE) OR (RAM_SHARE_SIZE STREQUAL "0K"))
    set(RAM_SHARE_DEFINITION "")
    set(RAM_SHARE_SECTION "")
else()
    set(RAM_SHARE_DEFINITION "    RAM_SHARED (rw) : ORIGIN = ${RAM_SHARE_ORIGIN}, LENGTH = ${RAM_SHARE_SIZE}\n")
    set(RAM_SHARE_SECTION "
MAPPING_TABLE (NOLOAD) : { *(MAPPING_TABLE) } >RAM_SHARED\n\
MB_MEM1 (NOLOAD)       : { *(MB_MEM1) } >RAM_SHARED\n\
_siMB_MEM2 = LOADADDR(.MB_MEM2);\n\
.MB_MEM2 :\n\
{\n\
  _sMB_MEM2 = . ;\n\
  *(MB_MEM2) ;\n\
_eMB_MEM2 = . ;\n\
} >RAM_SHARED AT> FLASH\n\
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
${RAM_SHARE_DEFINITION}\n\
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
${RAM_SHARE_SECTION}\n\
}"
)
file(WRITE "${LINKER_SCRIPT}" "${SCRIPT_TEXT}")


