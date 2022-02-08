ENTRY(Reset_Handler)

_estack = ORIGIN(DataSRAM) + LENGTH(DataSRAM); 

_Min_Heap_Size = 0x200 ; 
_Min_Stack_Size = 0x400 ;

MEMORY
{
  RetentionRAM (xrw)  : ORIGIN = 0x00000000,  LENGTH = 0x00000600
  TextSRAM     (xrw)  : ORIGIN = 0x10000000,  LENGTH = 128K
  DataSRAM     (xrw)  : ORIGIN = 0x10020000,  LENGTH = 128K
  SharedSRAM   (xrw)  : ORIGIN = 0x10040000,  LENGTH = 64K
  DmaSRAM      (xrw)  : ORIGIN = 0x10050000,  LENGTH = 64K
}

__OPENAMP_region_start__ = ORIGIN(SharedSRAM);
__OPENAMP_region_end__   = ORIGIN(SharedSRAM) + LENGTH(SharedSRAM);

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) 
    . = ALIGN(4);
  } >RetentionRAM

  .text :
  {
    . = ALIGN(4);
    *(.text)  
    *(.text*)      
    *(.glue_7) 
    *(.glue_7t)  
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;   
  } >TextSRAM

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata) 
    *(.rodata*)
    . = ALIGN(4);
  } >TextSRAM

  .ARM.extab   : {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >TextSRAM

  .ARM : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >TextSRAM

  .preinit_array     :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >TextSRAM

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >TextSRAM

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >TextSRAM

  __DATA_ROM = .;
  _sidata = LOADADDR(.data);

  .data :  AT(__DATA_ROM)
  {
    . = ALIGN(4);
    _sdata = .; 
    *(.data) 
    *(.data*) 

    . = ALIGN(4);
    _edata = .; 
  } >DataSRAM

  __DATA_END = __DATA_ROM + (_edata - _sdata);
  text_end = ORIGIN(TextSRAM) + LENGTH(TextSRAM);
  ASSERT(__DATA_END <= text_end, "region TextSRAM overflowed with text and data")

  .resource_table :
  {
    . = ALIGN(4);
    KEEP (*(.resource_table*))
    . = ALIGN(4);
  } >DataSRAM

  . = ALIGN(4);
  .bss :
  {
    _sbss = .;  
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .; 
    __bss_end__ = _ebss;
  } >DataSRAM

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >DataSRAM

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
