#pragma once

#include "tullave_worker.h"

#include <furi.h>
#include <furi_hal_nfc.h>

#include <core/string.h>

struct TuLlaveInfo {
    uint64_t balance;
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
