#pragma once
#include <furi.h>
#include <furi_hal.h>

typedef enum MifareFuzzerWorkerState {
    MifareFuzzerWorkerStateEmulate,
    MifareFuzzerWorkerStateStop,
} MifareFuzzerWorkerState;

#define UID_LEN 7
#define ATQA_LEN 2

typedef struct MifareFuzzerWorker {
    FuriThread* thread;
    MifareFuzzerWorkerState state;
    FuriHalNfcDevData nfc_dev_data;
} MifareFuzzerWorker;

// worker
MifareFuzzerWorker* mifare_fuzzer_worker_alloc();
void mifare_fuzzer_worker_free(MifareFuzzerWorker* mifare_fuzzer_worker);
void mifare_fuzzer_worker_stop(MifareFuzzerWorker* mifare_fuzzer_worker);
void mifare_fuzzer_worker_start(MifareFuzzerWorker* mifare_fuzzer_worker);
// task
int32_t mifare_fuzzer_worker_task(void* context);
//
bool mifare_fuzzer_worker_is_emulating(MifareFuzzerWorker* mifare_fuzzer_worker);
void mifare_fuzzer_worker_set_nfc_dev_data(
    MifareFuzzerWorker* mifare_fuzzer_worker,
    FuriHalNfcDevData nfc_dev_data);
FuriHalNfcDevData mifare_fuzzer_worker_get_nfc_dev_data(MifareFuzzerWorker* mifare_fuzzer_worker);
