#pragma once
#include <furi.h>
#include "nfc_relay.h"
#include "comm.h"
#include "emu_card_worker.h"

typedef enum {
    AsCardWorkerStateWaitPong,
    AsCardWorkerStateWaitNfcDevData,
    AsCardWorkerStateWaitApduReq,
    AsCardWorkerStateWaitApduResp,
} AsCardWorkerState;

typedef bool (*AsCardWorkerCallback)(AsCardWorkerState event, void* context);

typedef struct AsCardWorker {
    FuriThread* thread;
    AsCardWorkerState state;
    Comm* comm;
    NfcRelay* nfc_relay;
    AsCardWorkerCallback callback;
    EmuCardWorker* emu_card_worker;
    bool running;
} AsCardWorker;

AsCardWorker* as_card_worker_alloc();
int32_t as_card_worker_task(void* context);
void as_card_worker_start(AsCardWorker* as_reader_worker, AsCardWorkerCallback callback);
void as_card_worker_stop(AsCardWorker* as_reader_worker);
void as_card_worker_free(AsCardWorker* as_reader_worker);
