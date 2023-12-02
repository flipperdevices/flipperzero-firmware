#pragma once

#include "seader_worker.h"
#include "seader_i.h"

#include <furi.h>
#include <lib/toolbox/stream/file_stream.h>

#include <furi_hal.h>

#include <stdlib.h>
#include <rfal_rf.h>

#include <platform.h>
#include <PAC.h>
#include <SamVersion.h>

struct SeaderWorker {
    FuriThread* thread;
    Storage* storage;
    Stream* dict_stream;
    uint8_t sam_version[2];

    SeaderUartBridge* uart;
    SeaderCredential* credential;
    SeaderWorkerCallback callback;
    void* context;

    SeaderWorkerState state;
};

void seader_worker_change_state(SeaderWorker* seader_worker, SeaderWorkerState state);

int32_t seader_worker_task(void* context);
