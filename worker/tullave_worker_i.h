#pragma once

#include <furi.h>
#include "../drv/tullave_drv/tullave_drv.h"

#include "tullave_worker.h"

struct TuLlaveWorker {
    FuriThread* thread;
    TuLlaveInfo* card_info;
    TuLlaveWorkerCallback callback;
    void* context;
    TuLlaveWorkerState state;
};
