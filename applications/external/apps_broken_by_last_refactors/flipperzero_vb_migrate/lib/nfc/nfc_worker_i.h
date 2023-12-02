#pragma once

#include "nfc_worker.h"

#include <furi.h>
#include <lib/toolbox/stream/file_stream.h>

#include "protocols/nfc_util.h"
#include "protocols/mifare_common.h"
#include "protocols/nfca.h"
#include "helpers/reader_analyzer.h"

struct NfcWorker {
    FuriThread* thread;
    Storage* storage;
    Stream* dict_stream;

    NfcDeviceData* dev_data;

    NfcWorkerCallback callback;
    void* context;

    NfcWorkerState state;

    ReaderAnalyzer* reader_analyzer;
};

void nfc_worker_change_state(NfcWorker* nfc_worker, NfcWorkerState state);

int32_t nfc_worker_task(void* context);

void nfc_worker_read(NfcWorker* nfc_worker);

void nfc_worker_read_type(NfcWorker* nfc_worker);

void nfc_worker_emulate_uid(NfcWorker* nfc_worker);

void nfc_worker_emulate_mf_ultralight(NfcWorker* nfc_worker);

void nfc_worker_mf_ultralight_read_auth(NfcWorker* nfc_worker);

void nfc_worker_mf_ul_auth_attack(NfcWorker* nfc_worker);
