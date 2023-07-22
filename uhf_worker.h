#pragma once
#include <furi.h>
#include <furi_hal.h>
#include "uhf_data.h"

typedef struct UHFRFIDWorker {
    FuriThread* thread;
    UHFReturnData* uhf_data;
    // UHFRFIDWorkerCMD cmd;
    // Storage* storage;
    // UHFRFIDWorkerCallback callback;

} UHFRFIDWorker;