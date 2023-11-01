#pragma once
#include <furi.h>
#include "nfc_relay.h"
#include "comm.h"

typedef enum {
    AsReaderWorkerStateWaitPong,
    AsReaderWorkerStateCardSearch,
    AsReaderWorkerStateCardFound,
} AsReaderWorkerState;

typedef bool (*AsReaderWorkerCallback)(AsReaderWorkerState event, void* context);

typedef struct AsReaderWorker {
    FuriThread* thread;
    AsReaderWorkerState state;
    Comm* comm;
    NfcRelay* nfc_relay;
    AsReaderWorkerCallback callback;
    bool running;
} AsReaderWorker;

AsReaderWorker* as_reader_worker_alloc();
int32_t as_reader_worker_task(void* context);
void as_reader_worker_start(AsReaderWorker* as_reader_worker, AsReaderWorkerCallback callback);
void as_reader_worker_stop(AsReaderWorker* as_reader_worker);
void as_reader_worker_free(AsReaderWorker* as_reader_worker);
