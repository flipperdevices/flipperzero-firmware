#pragma once

#include "seader_i.h"
#include "seader_worker.h"

#include <furi.h>
#include <lib/toolbox/stream/file_stream.h>

#include <furi_hal.h>

#include <stdlib.h>

#include <PAC.h>
#include <SamVersion.h>

#define SEADER_POLLER_MAX_FWT (200000U)
#define SEADER_POLLER_MAX_BUFFER_SIZE (255U)

struct SeaderWorker {
    FuriThread* thread;
    Storage* storage;
    uint8_t sam_version[2];
    FuriMessageQueue* messages;
    FuriMutex* mq_mutex;

    SeaderUartBridge* uart;
    SeaderWorkerCallback callback;
    void* context;

    SeaderPollerEventType stage;
    SeaderWorkerState state;
};

struct SeaderAPDU {
    size_t len;
    uint8_t buf[SEADER_POLLER_MAX_BUFFER_SIZE];
};

struct SeaderPollerContainer {
    Iso14443_4aPoller* iso14443_4a_poller;
    PicopassPoller* picopass_poller;
};

void seader_worker_change_state(SeaderWorker* seader_worker, SeaderWorkerState state);

int32_t seader_worker_task(void* context);
