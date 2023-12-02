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
// only clobbers A0, A1, A2, A3, A5

const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_program_flash_command_program = {
    .name = "program_flash_command",
    .clobbers = BIT_A0 | BIT_A1 | BIT_A2 | BIT_A3 | BIT_A5,
    .data = {
        0xc94c, // sw      a1,20(a0)
        0xc910, // sw      a2,16(a0)
        0xc914, // sw      a3,16(a0)

        // waitloop:
        0x455c, // lw      a5,12(a0)
        0x8b85, // andi    a5,a5,1
        0xfff5, // bnez    a5, <waitloop>

        0x2823, // sw      zero,16(a0)
        0x0005, //

        0x9002, // ebreak
        0x9002, // ebreak
        0x9002, // ebreak
        0x9002, // ebreak
        0x9002, // ebreak
        0x9002, // ebreak
        0x9002, // ebreak
        0x9002, // ebreak
    }};