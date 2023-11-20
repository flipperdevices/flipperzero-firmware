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

#include "errors.h"
#include "helpers/swio.h"
#include "helpers/riscv_debug.h"

typedef enum {
    WchSwioFlasher_MinichlinkDebugger_InitSessionEvent = 0,
    WchSwioFlasher_MinichlinkDebugger_EndSessionEvent,
    WchSwioFlasher_MinichlinkDebugger_EventCount,
} WchSwioFlasher_MinichlinkDebugger_Events;

typedef struct _WchSwioFlasher_MinichlinkDebugger WchSwioFlasher_MinichlinkDebugger;

typedef void (*WchSwioFlasher_MinichlinkDebugger_EventCb)(
    void* context,
    WchSwioFlasher_MinichlinkDebugger_Events ev);

struct _WchSwioFlasher_MinichlinkDebugger {
    //FuriThread* worker_thread;
    //FuriMessageQueue* event_queue;
    //FuriStreamBuffer* data_stream;

    FuriTimer* timer;
    FuriMutex* mutex;
    uint32_t connected;
    uint32_t last_communication_time;

    WchSwioFlasher_SWIO* swio;
    WchSwioFlasher_RiscVDebug* riscv_debug;
    struct {
        WchSwioFlasher_MinichlinkDebugger_EventCb cb;
        void* context;
    } events[WchSwioFlasher_MinichlinkDebugger_EventCount];
};

WchSwioFlasher_MinichlinkDebugger* WchSwioFlasher_MinichlinkDebugger_create(
    WchSwioFlasher_SWIO* swio,
    WchSwioFlasher_RiscVDebug* riscv_debug);

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_registerEvent(
    WchSwioFlasher_MinichlinkDebugger* handle,
    WchSwioFlasher_MinichlinkDebugger_Events type,
    WchSwioFlasher_MinichlinkDebugger_EventCb cb,
    void* context);

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_unregisterEvent(
    WchSwioFlasher_MinichlinkDebugger* handle,
    WchSwioFlasher_MinichlinkDebugger_Events type);

void WchSwioFlasher_MinichlinkDebugger_destroy(WchSwioFlasher_MinichlinkDebugger* handle);

WchSwioFlasher_Error
    WchSwioFlasher_MinichlinkDebugger_initSession(WchSwioFlasher_MinichlinkDebugger* handle);

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_readRegister(
    WchSwioFlasher_MinichlinkDebugger* handle,
    uint8_t address,
    uint32_t* response);

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_writeRegister(
    WchSwioFlasher_MinichlinkDebugger* handle,
    uint8_t address,
    uint32_t data);

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_delayUs(
    WchSwioFlasher_MinichlinkDebugger* handle,
    uint32_t delay);

WchSwioFlasher_Error
    WchSwioFlasher_MinichlinkDebugger_endSession(WchSwioFlasher_MinichlinkDebugger* handle);
