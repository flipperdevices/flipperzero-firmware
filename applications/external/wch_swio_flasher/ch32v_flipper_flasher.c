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

#include "ch32v_flipper_flasher.h"

#include <storage/storage.h>
#include "utils.h"

#define TAG "WCH_CFF"

struct WchSwioFlasher_Ch32vFlipperFlasher {
    struct {
        WchSwioFlasher_CFF_Callback cb;
        void* cb_context;
    } callbacks;
    struct {
        WchSwioFlasher_RiscVDebug* debugger;
        WchSwioFlasher_WchFlasher* flasher;
    } helpers;

    struct {
        WchSwioFlasher_CFF_ChipInfo chip_info;
        WchSwioFlasher_CFF_EraseChip erase_chip;
        WchSwioFlasher_CFF_WriteChip write_chip;
    } history;

    FuriThread* worker_thread;
    FuriMessageQueue* event_queue;
};

typedef struct {
    enum {
        Ev_ChipInfo = 0,
        Ev_OpenFile,
        Ev_EraseChip,
        Ev_WriteChip,
        Ev_Stop,
    } type;
} Event;

static void fire_event(WchSwioFlasher_Ch32vFlipperFlasher* handle, ViewFlasher_Action type) {
    if(handle->callbacks.cb) {
        handle->callbacks.cb(handle->callbacks.cb_context, type);
    }
}

static WchSwioFlasher_CFF_ResultStatus map_error_status(WchSwioFlasher_Error err) {
    switch(err) {
    case WchSwioFlasher_Ok:
        return VWchSwioFlasher_CFF_Ok;
    case WchSwioFlasher_Error_Timeout:
    case WchSwioFlasher_Error_SwdResetDetected:
    case WchSwioFlasher_Error_SwdParityCheckError:
    case WchSwioFlasher_Error_TargetNotKnown:
    case WchSwioFlasher_Error_TargetInInvalidState:
    case WchSwioFlasher_Error_ProgramNotFinishedYet:
    case WchSwioFlasher_Error_ProgramRunError:
    case WchSwioFlasher_Error_DirtyRegs: {
        return VWchSwioFlasher_CFF_ChipNotConnected;
    }
    default:
        return VWchSwioFlasher_CFF_UnknownError;
    }
}

static void handle_chip_info(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    UNUSED(handle);
    FURI_LOG_D(TAG, "handle_chip_info");

    WchSwioFlasher_CFF_ChipInfo* ci = &handle->history.chip_info;

    WchSwioFlasher_RiscVDebug_ChipInfo info = {
        .esig_uniid = {0, 0, 0},
        .flash_size = 0,
    };

    WchSwioFlasher_Error err =
        WchSwioFlasher_RiscVDebug_get_chip_info(handle->helpers.debugger, &info);
    if(err != WchSwioFlasher_Ok) {
        LOG_ERR(err);
    }
    ci->status = map_error_status(err);
    ci->flash_size = info.flash_size;
    memcpy(ci->esig_uniid, info.esig_uniid, sizeof(ci->esig_uniid));

    fire_event(handle, WchSwioFlasher_CFF_ChipInfoCompleted);
}

static void handle_erase_chip(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    UNUSED(handle);
    FURI_LOG_D(TAG, "handle_erase_chip");

    WchSwioFlasher_CFF_EraseChip* ec = &handle->history.erase_chip;
    WchSwioFlasher_Error err = WchSwioFlasher_WchFlasher_wipe_chip(handle->helpers.flasher);
    if(err != WchSwioFlasher_Ok) {
        LOG_ERR(err);
    }
    ec->status = map_error_status(err);

    fire_event(handle, WchSwioFlasher_CFF_EraseChipCompleted);
}

static void handle_write_chip(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    WchSwioFlasher_CFF_WriteChip* wc = &handle->history.write_chip;

    FURI_LOG_D(TAG, "handle_write_chip");

    WchSwioFlasher_Error err;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    // Open file
    FURI_LOG_D(TAG, "Open bin file to write");
    if(!storage_file_open(file, handle->history.write_chip.path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open bin file");
        wc->status = WchSwioFlasher_CFF_UnableToOpenFile;
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    // Get file size
    FURI_LOG_D(TAG, "Get file size");
    uint64_t file_size = 0;
    if(storage_file_seek(file, 32 * 1024, false)) { // TODO: proper seek to end
        file_size = storage_file_tell(file);
        storage_file_seek(file, 0, true);
    }
    FURI_LOG_D(TAG, "File size is %llu", file_size);

    // Erase chip
    FURI_LOG_D(TAG, "Wipe chip");
    err = WchSwioFlasher_WchFlasher_wipe_chip(handle->helpers.flasher);
    if(err != WchSwioFlasher_Ok) {
        LOG_ERR(err);

        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);

        wc->status = map_error_status(err);
        fire_event(handle, WchSwioFlasher_CFF_WriteChipCompleted);
        return;
    }

    wc->percent = 0.1;

    // Load data
    FURI_LOG_D(TAG, "Flash data");

    if(file_size > 32 * 1024 || file_size == 0) {
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);

        wc->status = WchSwioFlasher_CFF_EmptyOrTooBigFile;
        fire_event(handle, WchSwioFlasher_CFF_WriteChipCompleted);

        return;
    }

    char* data = malloc(file_size);
    uint32_t address = 0;
    uint32_t to_write = 0;

    // Write page

    to_write = storage_file_read(file, data, file_size);
    if(to_write == file_size) {
        FURI_LOG_D(TAG, "Write %lu on " FMT_4HEX, to_write, _UI(address));
        err = WchSwioFlasher_WchFlasher_write_flash(
            handle->helpers.flasher, address, data, to_write);

        address += to_write;

        if(err != WchSwioFlasher_Ok) {
            LOG_ERR(err);
        }
    }

    FURI_LOG_D(TAG, "Writed %lu of %llu", address, file_size);
    wc->percent = 0.9;

    free(data);
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    if(err != WchSwioFlasher_Ok) {
        LOG_ERR(err);
        wc->status = map_error_status(err);
        fire_event(handle, WchSwioFlasher_CFF_WriteChipCompleted);
        return;
    }

    // Finally start chip
    FURI_LOG_D(TAG, "Reset to run");
    err = WchSwioFlasher_RiscVDebug_reset(
        handle->helpers.debugger, WchSwioFlasher_RVD_ResetToRunNoCheck);
    wc->status = map_error_status(err);

    wc->percent = 1;

    fire_event(handle, WchSwioFlasher_CFF_WriteChipCompleted);
}

static int32_t cff_create_thread_callback(void* context) {
    furi_assert(context);
    WchSwioFlasher_Ch32vFlipperFlasher* handle = context;

    for(;;) {
        Event event;
        FuriStatus status = furi_message_queue_get(handle->event_queue, &event, FuriWaitForever);
        if(status != FuriStatusOk) {
            FURI_LOG_E(TAG, "Message queue get error: %d", status);
            furi_delay_ms(100);
            continue;
        }

        if(event.type == Ev_Stop) {
            break;
        }

        switch(event.type) {
        case Ev_ChipInfo:
            handle_chip_info(handle);
            break;
        case Ev_EraseChip:
            handle_erase_chip(handle);
            break;
        case Ev_WriteChip:
            handle_write_chip(handle);
            break;
        default:
            break;
        }
    }

    return 0;
}

WchSwioFlasher_Ch32vFlipperFlasher* WchSwioFlasher_Ch32vFlipperFlasher_create(
    WchSwioFlasher_RiscVDebug* debugger,
    WchSwioFlasher_WchFlasher* flasher) {
    WchSwioFlasher_Ch32vFlipperFlasher* handle =
        malloc(sizeof(WchSwioFlasher_Ch32vFlipperFlasher));

    handle->helpers.debugger = debugger;
    handle->helpers.flasher = flasher;

    handle->event_queue = furi_message_queue_alloc(10, sizeof(Event));
    handle->worker_thread =
        furi_thread_alloc_ex("MusicWorker", 1024, cff_create_thread_callback, handle);

    return handle;
}

static void send_stop(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    Event event = {
        .type = Ev_Stop,
    };

    FuriStatus result = furi_message_queue_put(handle->event_queue, &event, 0);
    if(result != FuriStatusOk) {
        FURI_LOG_E(TAG, "Message queue put error: %d", result);
    }
}

void WchSwioFlasher_Ch32vFlipperFlasher_detach(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    if(furi_thread_get_state(handle->worker_thread) != FuriThreadStateStopped) {
        send_stop(handle);
        furi_thread_join(handle->worker_thread);
    }

    if(handle->history.write_chip.path != NULL) {
        free(handle->history.write_chip.path);
        handle->history.write_chip.path = NULL;
    }
}

void WchSwioFlasher_Ch32vFlipperFlasher_attach(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    // Just for sure
    WchSwioFlasher_Ch32vFlipperFlasher_detach(handle);

    handle->history.chip_info.status = VWchSwioFlasher_CFF_NoData;

    furi_thread_start(handle->worker_thread);
}

void WchSwioFlasher_Ch32vFlipperFlasher_destroy(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    WchSwioFlasher_Ch32vFlipperFlasher_detach(handle);

    furi_thread_free(handle->worker_thread);
    furi_message_queue_free(handle->event_queue);

    free(handle);
}

void WchSwioFlasher_Ch32vFlipperFlasher_event_callback(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_Callback cb,
    void* cb_context) {
    handle->callbacks.cb = cb;
    handle->callbacks.cb_context = cb_context;
}

void WchSwioFlasher_Ch32vFlipperFlasher_chip_info_data(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_ChipInfo* data) {
    memcpy(data, &handle->history.chip_info, sizeof(WchSwioFlasher_CFF_ChipInfo));
}

void WchSwioFlasher_Ch32vFlipperFlasher_chip_info(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    Event event = {
        .type = Ev_ChipInfo,
    };

    FuriStatus result = furi_message_queue_put(handle->event_queue, &event, 0);
    if(result != FuriStatusOk) {
        FURI_LOG_E(TAG, "Message queue put error: %d", result);
    }
}

void WchSwioFlasher_Ch32vFlipperFlasher_erase_chip_data(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_EraseChip* data) {
    memcpy(data, &handle->history.erase_chip, sizeof(WchSwioFlasher_CFF_EraseChip));
}

void WchSwioFlasher_Ch32vFlipperFlasher_erase_chip(WchSwioFlasher_Ch32vFlipperFlasher* handle) {
    Event event = {
        .type = Ev_EraseChip,
    };

    FuriStatus result = furi_message_queue_put(handle->event_queue, &event, 0);
    if(result != FuriStatusOk) {
        FURI_LOG_E(TAG, "Message queue put error: %d", result);
    }
}

void WchSwioFlasher_Ch32vFlipperFlasher_write_chip(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    char* path) {
    Event event = {
        .type = Ev_WriteChip,
    };

    if(handle->history.write_chip.path != NULL) {
        free(handle->history.write_chip.path);
        handle->history.write_chip.path = NULL;
    }

    uint32_t len = strlen(path);
    handle->history.write_chip.path = malloc(len + 1);
    strcpy(handle->history.write_chip.path, path);

    FuriStatus result = furi_message_queue_put(handle->event_queue, &event, 0);
    if(result != FuriStatusOk) {
        FURI_LOG_E(TAG, "Message queue put error: %d", result);
    }
}

void WchSwioFlasher_Ch32vFlipperFlasher_write_chip_data(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_WriteChip* data) {
    memcpy(data, &handle->history.write_chip, sizeof(WchSwioFlasher_CFF_WriteChip));
}