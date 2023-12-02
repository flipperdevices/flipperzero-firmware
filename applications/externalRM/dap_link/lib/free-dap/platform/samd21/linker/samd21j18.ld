/*
 * Copyright (c) 2015, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

MEMORY
{
  flash (rx) : ORIGIN = 0x00000000, LENGTH = 0x40000 /* 256k */
  ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32k */
}

__top_flash = 0x40000;
__top_ram = 0x20000000 + 0x8000;

ENTRY(irq_handler_reset)

SECTIONS
{
  .text : ALIGN(4)
  {
    FILL(0xff)
    KEEP(*(.vectors))
    *(.text*)
    *(.rodata)
    *(.rodata.*)
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
