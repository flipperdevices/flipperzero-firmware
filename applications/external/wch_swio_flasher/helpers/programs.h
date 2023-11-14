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

#pragma once

#include <stdint.h>

typedef struct {
    const char* const name;
    uint32_t clobbers;
    const uint16_t data[16];
} WchSwioFlasher_RiscVProgram;

#define BIT_T0 (1 << 5)
#define BIT_T1 (1 << 6)
#define BIT_T2 (1 << 7)
#define BIT_S0 (1 << 8)
#define BIT_S1 (1 << 9)
#define BIT_A0 (1 << 10)
#define BIT_A1 (1 << 11)
#define BIT_A2 (1 << 12)
#define BIT_A3 (1 << 13)
#define BIT_A4 (1 << 14)
#define BIT_A5 (1 << 15)

extern const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_get_set_u32_program;
extern const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_get_block_aligned_program;
extern const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_get_machine_isa_program;
extern const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_program_flash_command_program;
extern const WchSwioFlasher_RiscVProgram WchSwioFlasher_RiscVDebug_program_write_flash_program;
