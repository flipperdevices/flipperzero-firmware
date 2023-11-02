#pragma once

#include <lib/nfc/nfc_device.h>

typedef struct NfcCrocodileWorker NfcCrocodileWorker;

typedef enum {
    NfcCrocodileStorageText = 0,
    NfcCrocodileStorageURL,
} NfcCrocodileStorageType;

typedef enum {
    NfcCrocodileWorkerStateNone,
    NfcCrocodileWorkerStateReady,
    NfcCrocodileWorkerStateStop,
} NfcCrocodileWorkerState;

NfcCrocodileWorker* nfc_crocodile_worker_alloc();
void nfc_crocodile_worker_free(NfcCrocodileWorker* nfc_worker);

void nfc_crocodile_worker_start(
    NfcCrocodileWorker* nfc_worker,
    NfcCrocodileWorkerState state,
    NfcCrocodileStorageType storage_type,
    char* str,
    void* callback,
    void* context);

void nfc_crocodile_worker_stop(NfcCrocodileWorker* nfc_worker);