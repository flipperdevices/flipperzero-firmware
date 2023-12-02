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

#include <furi.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>
#include <hid_usage_desktop.h>
#include <furi_hal_usb_cdc.h>
#include <hid_usage_button.h>
#include "usb.h"
#include "usb_hid.h"

#include "utils.h"
#include "errors.h"
#include "minichlink_debugger.h"

#define TAG "WCH_DBG"

//static inline void fireEvent(    WchSwioFlasher_MinichlinkDebugger* handle,    WchSwioFlasher_MinichlinkDebugger_Events type);

static void last_com_check_timer_callback(void* context) {
    WchSwioFlasher_MinichlinkDebugger* handle = (WchSwioFlasher_MinichlinkDebugger*)context;
    UNUSED(handle);

    uint32_t is_connected = handle->connected;

    uint32_t lastlock = furi_kernel_lock();
    uint32_t last_activity = handle->last_communication_time;
    furi_kernel_restore_lock(lastlock);

    if((furi_get_tick() - last_activity) > furi_kernel_get_tick_frequency() / 2) {
        is_connected = 0;
    } else {
        is_connected = 1;
    }

    if(is_connected != handle->connected) {
        WchSwioFlasher_MinichlinkDebugger_Events type =
            is_connected ? WchSwioFlasher_MinichlinkDebugger_InitSessionEvent :
                           WchSwioFlasher_MinichlinkDebugger_EndSessionEvent;

        uint32_t lastlock = furi_kernel_lock();
        WchSwioFlasher_MinichlinkDebugger_EventCb cb = handle->events[type].cb;
        void* context = handle->events[type].context;
        furi_kernel_restore_lock(lastlock);

        if(cb != NULL) {
            UNUSED(context);
            cb(context, type);
        }

        handle->connected = is_connected;
    }

    //furi_mutex_acquire(handle->mutex, FuriWaitForever);

    //if(handle->connected && (furi_get_tick() - handle->last_communication_time) > 3000) {
    //    handle->connected = 0;
    //    fireEvent(handle, WchSwioFlasher_MinichlinkDebugger_EndSessionEvent);
    //}

    //furi_mutex_release(handle->mutex);
}

WchSwioFlasher_MinichlinkDebugger* WchSwioFlasher_MinichlinkDebugger_create(
    WchSwioFlasher_SWIO* swio,
    WchSwioFlasher_RiscVDebug* riscv_debug) {
    WchSwioFlasher_MinichlinkDebugger* handle = malloc(sizeof(WchSwioFlasher_MinichlinkDebugger));
    handle->swio = swio;
    handle->riscv_debug = riscv_debug;
    handle->connected = 0;
    handle->last_communication_time = 0;

    //handle->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    handle->timer = furi_timer_alloc(last_com_check_timer_callback, FuriTimerTypePeriodic, handle);
    memset(handle->events, 0, sizeof(handle->events));

    furi_timer_start(handle->timer, furi_kernel_get_tick_frequency() / 10);

    return handle;
}

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_registerEvent(
    WchSwioFlasher_MinichlinkDebugger* handle,
    WchSwioFlasher_MinichlinkDebugger_Events type,
    WchSwioFlasher_MinichlinkDebugger_EventCb cb,
    void* context) {
    uint32_t lastlock = furi_kernel_lock();
    handle->events[type].cb = cb;
    handle->events[type].context = context;
    furi_kernel_restore_lock(lastlock);
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_unregisterEvent(
    WchSwioFlasher_MinichlinkDebugger* handle,
    WchSwioFlasher_MinichlinkDebugger_Events type) {
    uint32_t lastlock = furi_kernel_lock();
    handle->events[type].cb = NULL;
    handle->events[type].context = NULL;
    furi_kernel_restore_lock(lastlock);
    return WchSwioFlasher_Ok;
}

static void log_activity(WchSwioFlasher_MinichlinkDebugger* handle) {
    uint32_t lastlock = furi_kernel_lock();
    handle->last_communication_time = furi_get_tick();
    furi_kernel_restore_lock(lastlock);
}

WchSwioFlasher_Error
    WchSwioFlasher_MinichlinkDebugger_initSession(WchSwioFlasher_MinichlinkDebugger* handle) {
    UNUSED(handle);
    log_activity(handle);
    //CHECK_ERR_M(WchSwioFlasher_SWIO_init(handle->swio), "unable to init SWIO");
    //CHECK_ERR_M(WchSwioFlasher_RiscVDebug_reset(handle->riscv_debug), "unable to reset DAP");

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_readRegister(
    WchSwioFlasher_MinichlinkDebugger* handle,
    uint8_t address,
    uint32_t* response) {
    log_activity(handle);
    CHECK_ERR_M(
        WchSwioFlasher_SWIO_read(handle->swio, address, response), "unable to read register");
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_writeRegister(
    WchSwioFlasher_MinichlinkDebugger* handle,
    uint8_t address,
    uint32_t data) {
    log_activity(handle);
    CHECK_ERR_M(
        WchSwioFlasher_SWIO_write(handle->swio, address, data), "unable to write register");
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_MinichlinkDebugger_delayUs(
    WchSwioFlasher_MinichlinkDebugger* handle,
    uint32_t delay) {
    UNUSED(handle);
    log_activity(handle);
    furi_delay_us(delay);
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_MinichlinkDebugger_endSession(WchSwioFlasher_MinichlinkDebugger* handle) {
    UNUSED(handle);
    return WchSwioFlasher_Ok;
}

void WchSwioFlasher_MinichlinkDebugger_destroy(WchSwioFlasher_MinichlinkDebugger* handle) {
    furi_timer_stop(handle->timer);
    furi_timer_free(handle->timer);

    free(handle);
}