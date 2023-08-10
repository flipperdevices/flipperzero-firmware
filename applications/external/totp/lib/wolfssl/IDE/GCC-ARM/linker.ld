MEMORY
{
  FLASH (wx) : ORIGIN = 0x00000000, LENGTH = 256K
  RAM   (wx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    __vectors_start__ = .;
    .vectors : { *(.vectors) } > FLASH
    __vectors_end__ = __vectors_start__ + 0x400;
	.sys    : { *(.sys*) }    > FLASH
    .text   : { *(.text*) }   > FLASH
    .rodata : { *(.text*) }   > FLASH

	__data_load_start__ = .;
    __data_start__ = .;
    .data   : { *(.data*) }   > RAM
    __data_end__ = __data_start__ + SIZEOF(.data);

    __bss_start__ = .;
    .bss    : { *(.bss*)  }   > RAM
    __bss_end__   = __bss_start__ + SIZEOF(.bss);

    __heap_start__ = .;
    .heap   : { *(.heap*)  }   > RAM
    __heap_end__ = __heap_start__ + SIZEOF(.heap);

    end = .;
}
