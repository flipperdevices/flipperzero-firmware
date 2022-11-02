#pragma once

#include <furi.h>

#include "nfc_magic_worker.h"

struct NfcMagicWorker {
    FuriThread* thread;

    NfcDeviceData* dev_data;

    NfcMagicWorkerCallback callback;
    void* context;

    NfcMagicWorkerState state;
};

int32_t nfc_magic_worker_task(void* context);

void nfc_magic_worker_write(NfcMagicWorker* nfc_magic_worker);
