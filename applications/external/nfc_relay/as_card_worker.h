#pragma once
#include <furi.h>
#include <nfc/nfc_listener.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a.h>
#include "nfc_relay.h"
#include "comm.h"

typedef enum {
    AsCardWorkerStateWaitPong,
    AsCardWorkerStateWaitNfcDevData,
    AsCardWorkerStateWaitApduReq,
    AsCardWorkerStateWaitApduResp,
    AsCardWorkerStateGetApduResp,
} AsCardWorkerState;

typedef bool (*AsCardWorkerCallback)(AsCardWorkerState event, void* context);

typedef struct AsCardWorker {
    FuriThread* thread;
    AsCardWorkerState state;
    Comm* comm;
    NfcRelay* nfc_relay;
    AsCardWorkerCallback callback;

    Iso14443_4aData* dev_data;
    Nfc* nfc;
    NfcListener* listener;
    BitBuffer* bitbuffer;
    uint8_t* buff_tx;
    uint32_t delayus;
    uint16_t apdu_buf_len;
    uint8_t apdu_buf[260];
    bool running;
} AsCardWorker;

AsCardWorker* as_card_worker_alloc();
int32_t as_card_worker_task(void* context);
void as_card_worker_start(AsCardWorker* as_reader_worker, AsCardWorkerCallback callback);
void as_card_worker_stop(AsCardWorker* as_reader_worker);
void as_card_worker_free(AsCardWorker* as_reader_worker);
