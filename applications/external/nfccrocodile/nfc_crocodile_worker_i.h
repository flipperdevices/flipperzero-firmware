#pragma once

#include "nfc_crocodile_worker.h"

#include <furi.h>

#include <lib/nfc/protocols/nfc_util.h>

struct NfcCrocodileWorker {
    FuriThread* thread;

    bool (*callback)(void*);
    void* context;

    NfcCrocodileWorkerState state;
    bool connection;

    uint8_t uid_data[7];
    uint8_t atqa_data[2];
    uint8_t sak;

    uint8_t card[0x87 * 4];
    char* card_content;

    NfcCrocodileStorageType storage_type;
};

void nfc_crocodile_worker_change_state(
    NfcCrocodileWorker* nfc_worker,
    NfcCrocodileWorkerState state);

int32_t nfc_crocodile_worker_task(void* context);

void nfc_crocodile_worker_card_init(NfcCrocodileWorker* nfc_crocodile_worker);
static void nfc_crocodile_worker_card_init_row(
    uint8_t* data,
    uint8_t row,
    uint8_t val1,
    uint8_t val2,
    uint8_t val3,
    uint8_t val4);