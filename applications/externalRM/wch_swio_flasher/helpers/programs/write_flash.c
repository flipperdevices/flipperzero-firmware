/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdint.h>
#include "../programs.h"
//------------------------------------------------------------------------------
// only clobbers S0, A0, A1, A2, A3, A4, A5

// A0 = 0x40022000 - flash registers base
// A1 = 0xE00000F4 - ptr to DATA0 @ 0xE00000F4
// A2 = dst_addr
// A3 = BIT_CTLR_FTPG | BIT_CTLR_BUFLOAD
// A4 = BIT_CTLR_FTPG | BIT_CTLR_STRT
// A5 = BIT_CTLR_FTPG | BIT_CTLR_BUFRST

const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_program_write_flash_program = {
    .name = "write_flash",
    .clobbers = BIT_S0 | BIT_A0 | BIT_A1 | BIT_A2 | BIT_A3 | BIT_A4 | BIT_A5,
    .data = {
        // Copy word and trigger BUFLOAD
        0x4180, // lw      s0,0(a1) - load data byte from DATA0
        0xc200, // sw      s0,0(a2) - store data byte to 'dst_addr'
        0xc914, // sw      a3,16(a0) - store BIT_CTLR_FTPG | BIT_CTLR_BUFLOAD to FLASH_CTRL

        // waitloop1: Busywait for copy to complete - this seems to be required now?
        0x4540, // lw      s0,12(a0) - load STATR
        0x8805, // andi    s0,s0,1 - check BUSY flag
        0xfc75, // bnez    s0, <waitloop1>

        // Advance dest pointer and trigger START if we ended a page
        0x0611, // addi    a2,a2,4
        0x7413, // andi    s0,a2,63
        0x03f6, //
        0xe419, // bnez    s0, <end>
        0xc918, // sw      a4,16(a0)

        // waitloop2: Busywait for page write to complete
        0x4540, // lw      s0,12(a0)
        0x8805, // andi    s0,s0,1
        0xfc75, // bnez    s0, <waitloop2>

        // Reset buffer, don't need busywait as it'll complete before we send the
        // next dword.
        0xc91c, // sw      a5,16(a0)

        // Update page address
        0xc950, // sw      a2,20(a0)
    }};