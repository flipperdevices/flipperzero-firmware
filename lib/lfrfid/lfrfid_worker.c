#include <furi.h>
#include <furi_hal.h>
#include <atomic.h>
#include "lfrfid_worker_i.h"

typedef enum {
    LFRFIDMessageEnd,
    LFRFIDMessageStop,
    LFRFIDMessageRead,
    LFRFIDMessageWrite,
    LFRFIDMessageEmulate,
} LFRFIDMessageType;

typedef struct {
    LFRFIDMessageType type;
    union {
        LFRFIDKey* key;
    } data;
} LFRFIDMessage;

static int32_t lfrfid_worker_thread(void* thread_context);

LFRFIDWorker* lfrfid_worker_alloc() {
    LFRFIDWorker* worker = malloc(sizeof(LFRFIDWorker));
    worker->key_p = NULL;
    worker->key_data = malloc(lfrfid_key_get_max_size());
    worker->messages = osMessageQueueNew(1, sizeof(LFRFIDMessage), NULL);
    worker->mode_index = LFRFIDWorkerIdle;
    worker->read_cb = NULL;
    worker->write_cb = NULL;
    worker->cb_ctx = NULL;

    worker->thread = furi_thread_alloc();
    furi_thread_set_name(worker->thread, "lfrfid_worker");
    furi_thread_set_callback(worker->thread, lfrfid_worker_thread);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_stack_size(worker->thread, 2048);

    worker->protocols = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);

    return worker;
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

void lfrfid_worker_read_start(LFRFIDWorker* worker, LFRFIDKey* key) {
    LFRFIDMessage message = {.type = LFRFIDMessageRead, .data.key = key};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void lfrfid_worker_write_start(LFRFIDWorker* worker, LFRFIDKey* key) {
    LFRFIDMessage message = {.type = LFRFIDMessageWrite, .data.key = key};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void lfrfid_worker_emulate_start(LFRFIDWorker* worker, LFRFIDKey* key) {
    LFRFIDMessage message = {.type = LFRFIDMessageEmulate, .data.key = key};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void lfrfid_worker_stop(LFRFIDWorker* worker) {
    LFRFIDMessage message = {.type = LFRFIDMessageStop};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
}

void lfrfid_worker_free(LFRFIDWorker* worker) {
    protocol_dict_free(worker->protocols);
    osMessageQueueDelete(worker->messages);
    furi_thread_free(worker->thread);
    free(worker->key_data);
    free(worker);
}

void lfrfid_worker_start_thread(LFRFIDWorker* worker) {
    furi_thread_start(worker->thread);
}

void lfrfid_worker_stop_thread(LFRFIDWorker* worker) {
    LFRFIDMessage message = {.type = LFRFIDMessageEnd};
    furi_check(osMessageQueuePut(worker->messages, &message, 0, osWaitForever) == osOK);
    furi_thread_join(worker->thread);
}

void lfrfid_worker_switch_mode(LFRFIDWorker* worker, LFRFIDWorkerMode mode) {
    lfrfid_worker_modes[worker->mode_index].stop(worker);
    worker->mode_index = mode;
    lfrfid_worker_modes[worker->mode_index].start(worker);
}

void lfrfid_worker_set_key_p(LFRFIDWorker* worker, LFRFIDKey* key) {
    worker->key_p = key;
}

static int32_t lfrfid_worker_thread(void* thread_context) {
    LFRFIDWorker* worker = thread_context;
    bool running = true;
    LFRFIDMessage message;
    osStatus_t status;

    lfrfid_worker_modes[worker->mode_index].start(worker);

    while(running) {
        status = osMessageQueueGet(
            worker->messages, &message, NULL, lfrfid_worker_modes[worker->mode_index].quant);
        if(status == osOK) {
            switch(message.type) {
            case LFRFIDMessageEnd:
                lfrfid_worker_switch_mode(worker, LFRFIDWorkerIdle);
                lfrfid_worker_set_key_p(worker, NULL);
                running = false;
                break;
            case LFRFIDMessageStop:
                lfrfid_worker_switch_mode(worker, LFRFIDWorkerIdle);
                lfrfid_worker_set_key_p(worker, NULL);
                break;
            case LFRFIDMessageRead:
                lfrfid_worker_set_key_p(worker, message.data.key);
                lfrfid_worker_switch_mode(worker, LFRFIDWorkerRead);
                break;
            case LFRFIDMessageWrite:
                lfrfid_worker_set_key_p(worker, message.data.key);
                lfrfid_worker_switch_mode(worker, LFRFIDWorkerWrite);
                break;
            case LFRFIDMessageEmulate:
                lfrfid_worker_set_key_p(worker, message.data.key);
                lfrfid_worker_switch_mode(worker, LFRFIDWorkerEmulate);
                break;
            }
        } else if(status == osErrorTimeout) {
            lfrfid_worker_modes[worker->mode_index].tick(worker);
        } else {
            furi_crash("LFRFID worker error");
        }
    }

    lfrfid_worker_modes[worker->mode_index].stop(worker);

    return 0;
}