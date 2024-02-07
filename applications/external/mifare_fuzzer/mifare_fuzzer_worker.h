#pragma once
#include <furi.h>
#include <furi_hal.h>

#include <nfc/nfc_device.h>
#include <nfc/nfc_listener.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_listener.h>

typedef enum MifareFuzzerWorkerState {
    MifareFuzzerWorkerStateEmulate,
    MifareFuzzerWorkerStateStop,
} MifareFuzzerWorkerState;

#define UID_LEN 7
#define ATQA_LEN 2

typedef struct MifareFuzzerWorker {
    FuriThread* thread;
    MifareFuzzerWorkerState state;
    NfcListener* nfc_listener;
    NfcDevice* nfc_device;
    Iso14443_3aData nfc_data;
    Nfc* nfc;
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

void mifare_fuzzer_worker_set_nfc_device(
    MifareFuzzerWorker* mifare_fuzzer_worker,
    NfcDevice* nfc_device);
NfcDevice* mifare_fuzzer_worker_get_nfc_device(MifareFuzzerWorker* mifare_fuzzer_worker);

void mifare_fuzzer_worker_set_nfc_data(
    MifareFuzzerWorker* mifare_fuzzer_worker,
    Iso14443_3aData nfc_data);
Iso14443_3aData mifare_fuzzer_worker_get_nfc_data(MifareFuzzerWorker* mifare_fuzzer_worker);