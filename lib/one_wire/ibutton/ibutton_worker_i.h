/**
 * @file ibutton_worker_i.h
 * 
 * iButton worker, internal definitions 
 */

#pragma once

#include <core/thread.h>
#include <core/message_queue.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

#include "ibutton_worker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint32_t quant;
    void (*const start)(iButtonWorker* worker);
    void (*const tick)(iButtonWorker* worker);
    void (*const stop)(iButtonWorker* worker);
} iButtonWorkerModeType;

typedef enum {
    iButtonWorkerIdle = 0,
    iButtonWorkerRead = 1,
    iButtonWorkerWrite = 2,
    iButtonWorkerEmulate = 3,
} iButtonWorkerMode;

struct iButtonWorker {
    iButtonKey* key;
    OneWireHost* host;
    OneWireSlave* bus;
    iButtonWorkerMode mode_index;
    FuriMessageQueue* messages;
    FuriThread* thread;

    iButtonWorkerReadCallback read_cb;
    iButtonWorkerWriteCallback write_cb;
    iButtonWorkerEmulateCallback emulate_cb;

    void* cb_ctx;
};

extern const iButtonWorkerModeType ibutton_worker_modes[];

void ibutton_worker_switch_mode(iButtonWorker* worker, iButtonWorkerMode mode);
void ibutton_worker_notify_emulate(iButtonWorker* worker);

#ifdef __cplusplus
}
#endif
