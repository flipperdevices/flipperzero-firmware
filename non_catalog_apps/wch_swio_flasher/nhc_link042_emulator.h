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
#include <furi.h>
#include <furi_hal_usb.h>

#include "minichlink_debugger.h"
#include "errors.h"

typedef struct {
    uint32_t attached;
    FuriHalUsbInterface* usb_mode_prev;
    WchSwioFlasher_MinichlinkDebugger* debugger;
    FuriMutex* mutex;

} WchSwioFlasher_NhcLink042Emu;

WchSwioFlasher_NhcLink042Emu*
    WchSwioFlasher_NhcLink042Emu_create(WchSwioFlasher_MinichlinkDebugger* debugger);

void WchSwioFlasher_NhcLink042Emu_destroy(WchSwioFlasher_NhcLink042Emu* handle);

WchSwioFlasher_Error WchSwioFlasher_NhcLink042Emu_attach(WchSwioFlasher_NhcLink042Emu* handle);

WchSwioFlasher_Error WchSwioFlasher_NhcLink042Emu_detach(WchSwioFlasher_NhcLink042Emu* handle);
