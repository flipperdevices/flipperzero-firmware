#pragma once

#include <furi.h>

#include "tullave_worker.h"

struct TuLlaveWorker {
    FuriThread* thread;
    NfcDeviceData* dev_data;
    TuLlaveWorkerCallback callback;
    void* context;
    TuLlaveWorkerState state;
};
