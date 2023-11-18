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
#define SEADER_POLLER_MAX_BUFFER_SIZE (64U)

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

void seader_worker_change_state(SeaderWorker* seader_worker, SeaderWorkerState state);

int32_t seader_worker_task(void* context);
