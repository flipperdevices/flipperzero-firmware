/*
  SPDX-License-Identifier: BSD-3-Clause
  Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.
*/

MEMORY
{
  flash (rx) : ORIGIN = 0x00000000, LENGTH = 0x80000 /* 512k */
  ram (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x20000 /* 128k */
}

__top_flash = ORIGIN(flash) + LENGTH(flash);
__top_ram = ORIGIN(ram) + LENGTH(ram);

ENTRY(irq_handler_reset)

SECTIONS
{
  .text : ALIGN(4)
  {
    FILL(0xff)
    KEEP(*(.rom_vectors))
    KEEP(*(.reset_handler))
    . = ALIGN(4);
  } > flash

  . = ALIGN(4);
  _etext = .;

  .uninit_RESERVED : ALIGN(4)
  {
    KEEP(*(.bss.$RESERVED*))
  } > ram

  .data : ALIGN(4)
  {
    FILL(0xff)
    _data = .;
    *(.vectors)
    *(.text*)
    *(.rodata)
    *(.rodata.*)
    *(.ramfunc .ramfunc.*);
    *(vtable)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > ram AT > flash

  .bss : ALIGN(4)
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    PROVIDE(_end = .);
  } > ram

  PROVIDE(_stack_top = __top_ram - 0);
}

