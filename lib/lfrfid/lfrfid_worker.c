#include <furi.h>
#include <furi_hal.h>
#include <atomic.h>
#include "lfrfid_worker_i.h"

typedef enum {
    LFRFIDEventStopThread = (1 << 0),
    LFRFIDEventStopMode = (1 << 1),
    LFRFIDEventRead = (1 << 2),
    LFRFIDEventWrite = (1 << 3),
    LFRFIDEventEmulate = (1 << 4),
    LFRFIDEventReadRaw = (1 << 5),
    LFRFIDEventEmulateRaw = (1 << 6),
    LFRFIDEventAll =
        (LFRFIDEventStopThread | LFRFIDEventStopMode | LFRFIDEventRead | LFRFIDEventWrite |
         LFRFIDEventEmulate | LFRFIDEventReadRaw | LFRFIDEventEmulateRaw),
} LFRFIDEventType;

static int32_t lfrfid_worker_thread(void* thread_context);

LFRFIDWorker* lfrfid_worker_alloc() {
    LFRFIDWorker* worker = malloc(sizeof(LFRFIDWorker));
    worker->mode_index = LFRFIDWorkerIdle;
    worker->read_cb = NULL;
    worker->write_cb = NULL;
    worker->emulate_cb = NULL;
    worker->cb_ctx = NULL;
    worker->raw_filename = NULL;
    worker->mode_storage = NULL;
    worker->raw_worker = lfrfid_raw_worker_alloc();

    worker->thread = furi_thread_alloc();
    furi_thread_set_name(worker->thread, "lfrfid_worker");
    furi_thread_set_callback(worker->thread, lfrfid_worker_thread);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_stack_size(worker->thread, 2048);

    worker->protocols = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);

    return worker;
}

void lfrfid_worker_free(LFRFIDWorker* worker) {
    if(worker->raw_filename) {
        free(worker->raw_filename);
    }

    lfrfid_raw_worker_free(worker->raw_worker);
    protocol_dict_free(worker->protocols);
    furi_thread_free(worker->thread);
    free(worker);
}

void lfrfid_worker_read_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerReadCallback callback,
    void* context) {
    furi_check(worker->mode_index == LFRFIDWorkerIdle);
    worker->read_cb = callback;
    worker->cb_ctx = context;
}

void lfrfid_worker_write_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerWriteCallback callback,
    void* context) {
    furi_check(worker->mode_index == LFRFIDWorkerIdle);
    worker->write_cb = callback;
    worker->cb_ctx = context;
}

void lfrfid_worker_emulate_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerEmulateCallback callback,
    void* context) {
    furi_check(worker->mode_index == LFRFIDWorkerIdle);
    worker->emulate_cb = callback;
    worker->cb_ctx = context;
}

void lfrfid_worker_read_raw_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerReadRawCallback callback,
    void* context) {
    furi_check(worker->mode_index == LFRFIDWorkerIdle);
    worker->read_raw_cb = callback;
    worker->cb_ctx = context;
}

void lfrfid_worker_emulate_raw_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerEmulateRawCallback callback,
    void* context) {
    furi_check(worker->mode_index == LFRFIDWorkerIdle);
    worker->emulate_raw_cb = callback;
    worker->cb_ctx = context;
}

void lfrfid_worker_read_start(LFRFIDWorker* worker, LFRFIDWorkerReadType type) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    worker->read_type = type;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventRead);
}

void lfrfid_worker_write_start(LFRFIDWorker* worker) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventWrite);
}

void lfrfid_worker_emulate_start(LFRFIDWorker* worker) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventEmulate);
}

void lfrfid_worker_set_filename(LFRFIDWorker* worker, const char* filename) {
    if(worker->raw_filename) {
        free(worker->raw_filename);
    }

    worker->raw_filename = strdup(filename);
}

void lfrfid_worker_read_raw_start(
    LFRFIDWorker* worker,
    const char* filename,
    LFRFIDWorkerReadType type) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    worker->read_type = type;
    lfrfid_worker_set_filename(worker, filename);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventReadRaw);
}

void lfrfid_worker_emulate_raw_start(LFRFIDWorker* worker, const char* filename) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    lfrfid_worker_set_filename(worker, filename);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventEmulateRaw);
}

void lfrfid_worker_stop(LFRFIDWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventStopMode);
}

void lfrfid_worker_start_thread(LFRFIDWorker* worker) {
    furi_thread_start(worker->thread);
}

void lfrfid_worker_stop_thread(LFRFIDWorker* worker) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), LFRFIDEventStopThread);
    furi_thread_join(worker->thread);
}

bool lfrfid_worker_check_for_stop(LFRFIDWorker* worker) {
    UNUSED(worker);
    uint32_t flags = furi_thread_flags_get();
    return (flags & LFRFIDEventStopMode);
}

size_t lfrfid_worker_dict_get_data_size(LFRFIDWorker* worker, ProtocolId protocol) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    return protocol_dict_get_data_size(worker->protocols, protocol);
}

void lfrfid_worker_dict_get_data(
    LFRFIDWorker* worker,
    ProtocolId protocol,
    uint8_t* data,
    size_t data_size) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    protocol_dict_get_data(worker->protocols, protocol, data, data_size);
}

const char* lfrfid_worker_dict_get_name(LFRFIDWorker* worker, ProtocolId protocol) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    return protocol_dict_get_name(worker->protocols, protocol);
}

const char* lfrfid_worker_dict_get_manufacturer(LFRFIDWorker* worker, ProtocolId protocol) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    return protocol_dict_get_manufacturer(worker->protocols, protocol);
}

void lfrfid_worker_dict_render(LFRFIDWorker* worker, ProtocolId protocol, string_t result) {
    furi_assert(worker->mode_index == LFRFIDWorkerIdle);
    protocol_dict_render_data(worker->protocols, result, protocol);
}

static int32_t lfrfid_worker_thread(void* thread_context) {
    LFRFIDWorker* worker = thread_context;
    bool running = true;

    while(running) {
        uint32_t flags = furi_thread_flags_wait(LFRFIDEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(flags != FuriFlagErrorTimeout) {
            // stop thread
            if(flags & LFRFIDEventStopThread) break;

            // switch mode
            if(flags & LFRFIDEventRead) worker->mode_index = LFRFIDWorkerRead;
            if(flags & LFRFIDEventWrite) worker->mode_index = LFRFIDWorkerWrite;
            if(flags & LFRFIDEventEmulate) worker->mode_index = LFRFIDWorkerEmulate;
            if(flags & LFRFIDEventReadRaw) worker->mode_index = LFRFIDWorkerReadRaw;
            if(flags & LFRFIDEventEmulateRaw) worker->mode_index = LFRFIDWorkerEmulateRaw;

            // do mode, if it exists
            if(lfrfid_worker_modes[worker->mode_index].process) {
                lfrfid_worker_modes[worker->mode_index].process(worker);
            }

            // reset mode
            worker->mode_index = LFRFIDWorkerIdle;
        }
    }

    return 0;
}