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
// data0 = data to write
// data1 = address. set low bit if this is a write
// only clobbers A0, A1

const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_get_set_u32_program = {
    .name = "get_set_u32",
    .clobbers = BIT_A0 | BIT_A1,
    .data = {
        //0xe0000537,   // lui a0,0xe0000
        0x0537,
        0xe000,

        //0x0f450513,   // addi a0,a0,0xF4
        0x0513,
        0x0f45,

        0x414c, // lw a1,4(a0)
        0x8985, // andi a1,a1,1
        0xc591, // beqz a1,get_u32

        //set_u32:
        0x414c, // lw a1,4(a0)
        0x15fd, // addi a1,a1,-1
        0x4108, // lw a0,0(a0)
        0xc188, // sw a0,0(a1)
        0x9002, // ebreak

        //get_u32:
        0x414c, // lw a1,4(a0)
        0x418c, // lw a1,0(a1)
        0xc10c, // sw a1,0(a0)
        0x9002, // ebreak
    }};
