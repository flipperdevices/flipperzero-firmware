#pragma once

#include "tullave_worker.h"

#include <furi.h>
#include <furi_hal_nfc.h>
#include <float.h>

#include <core/string.h>

struct TuLlaveInfo {
    double balance;
    FuriString* card_number;
};

typedef struct TuLlaveInfo TuLlaveInfo;

struct TuLlaveWorker {
    FuriThread* thread;
    TuLlaveInfo* card_info;
    TuLlaveWorkerCallback callback;
    void* context;
    TuLlaveWorkerState state;
};

int32_t tullave_worker_task(void* context);
