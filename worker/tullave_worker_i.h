#pragma once

#include <furi.h>

#include "tullave_worker.h"

struct TuLlaveWorker {
    FuriThread* thread;
    TuLlaveWorkerCallback callback;
    void* context;
    TuLlaveWorkerState state;
};
