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
#include "../errors.h"
#include "riscv_debug.h"

typedef struct {
    WchSwioFlasher_RiscVDebug* debug;

} WchSwioFlasher_WchFlasher;

WchSwioFlasher_WchFlasher* WchSwioFlasher_WchFlasher_create(WchSwioFlasher_RiscVDebug* debug);

void WchSwioFlasher_WchFlasher_destroy(WchSwioFlasher_WchFlasher* handle);

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_unlock_flash(WchSwioFlasher_WchFlasher* handle);

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_lock_flash(WchSwioFlasher_WchFlasher* handle);

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_wipe_chip(WchSwioFlasher_WchFlasher* handle);

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_write_flash(
    WchSwioFlasher_WchFlasher* handle,
    uint32_t dst_addr,
    void* blob,
    int size);
