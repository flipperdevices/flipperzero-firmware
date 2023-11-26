#include "mifare_fuzzer_worker.h"

/// @brief mifare_fuzzer_worker_alloc()
/// @return
MifareFuzzerWorker* mifare_fuzzer_worker_alloc() {
    MifareFuzzerWorker* mifare_fuzzer_worker = malloc(sizeof(MifareFuzzerWorker));
    // Worker thread attributes
    mifare_fuzzer_worker->thread = furi_thread_alloc_ex(
        "MifareFuzzerWorker", 8192, mifare_fuzzer_worker_task, mifare_fuzzer_worker);
    mifare_fuzzer_worker->state = MifareFuzzerWorkerStateStop;

    mifare_fuzzer_worker->nfc = nfc_alloc();
    mifare_fuzzer_worker->nfc_device = nfc_device_alloc();

    return mifare_fuzzer_worker;
}

/// @brief mifare_fuzzer_worker_free()
/// @param mifare_fuzzer_worker
void mifare_fuzzer_worker_free(MifareFuzzerWorker* mifare_fuzzer_worker) {
    furi_assert(mifare_fuzzer_worker);
    furi_thread_free(mifare_fuzzer_worker->thread);

    nfc_free(mifare_fuzzer_worker->nfc);
    nfc_device_free(mifare_fuzzer_worker->nfc_device);

    free(mifare_fuzzer_worker);
}

/// @brief mifare_fuzzer_worker_stop()
/// @param mifare_fuzzer_worker
void mifare_fuzzer_worker_stop(MifareFuzzerWorker* mifare_fuzzer_worker) {
    furi_assert(mifare_fuzzer_worker);
    if(mifare_fuzzer_worker->state != MifareFuzzerWorkerStateStop) {
        mifare_fuzzer_worker->state = MifareFuzzerWorkerStateStop;
        furi_thread_join(mifare_fuzzer_worker->thread);
    }
}

/// @brief mifare_fuzzer_worker_start()
/// @param mifare_fuzzer_worker
void mifare_fuzzer_worker_start(MifareFuzzerWorker* mifare_fuzzer_worker) {
    furi_assert(mifare_fuzzer_worker);
    mifare_fuzzer_worker->state = MifareFuzzerWorkerStateEmulate;
    furi_thread_start(mifare_fuzzer_worker->thread);
}

/// @brief mifare_fuzzer_worker_task()
/// @param context
/// @return
int32_t mifare_fuzzer_worker_task(void* context) {
    MifareFuzzerWorker* mifare_fuzzer_worker = context;

    if(mifare_fuzzer_worker->state == MifareFuzzerWorkerStateEmulate) {
        const Iso14443_3aData* data =
            nfc_device_get_data(mifare_fuzzer_worker->nfc_device, NfcProtocolIso14443_3a);

        mifare_fuzzer_worker->nfc_listener =
            nfc_listener_alloc(mifare_fuzzer_worker->nfc, NfcProtocolIso14443_3a, data);
        nfc_listener_start(mifare_fuzzer_worker->nfc_listener, NULL, NULL);

        while(mifare_fuzzer_worker->state == MifareFuzzerWorkerStateEmulate) {
            furi_delay_ms(50);
        }

        nfc_listener_stop(mifare_fuzzer_worker->nfc_listener);
        nfc_listener_free(mifare_fuzzer_worker->nfc_listener);
    }

    mifare_fuzzer_worker->state = MifareFuzzerWorkerStateStop;

    return 0;
}

/// @brief mifare_fuzzer_worker_is_emulating()
/// @param mifare_fuzzer_worker
/// @return
bool mifare_fuzzer_worker_is_emulating(MifareFuzzerWorker* mifare_fuzzer_worker) {
    if(mifare_fuzzer_worker->state == MifareFuzzerWorkerStateEmulate) {
        return true;
    }
    return false;
}

/// @brief mifare_fuzzer_worker_set_nfc_data()
/// @param mifare_fuzzer_worker
/// @param nfc_data
void mifare_fuzzer_worker_set_nfc_data(
    MifareFuzzerWorker* mifare_fuzzer_worker,
    Iso14443_3aData nfc_data) {
    Iso14443_3aData* nfc_14a_data = iso14443_3a_alloc();
    nfc_14a_data->uid_len = nfc_data.uid_len;
    memcpy(nfc_14a_data->uid, nfc_data.uid, nfc_data.uid_len);
    memcpy(nfc_14a_data->atqa, nfc_data.atqa, ATQA_LEN);
    nfc_14a_data->sak = nfc_data.sak;

    nfc_device_clear(mifare_fuzzer_worker->nfc_device);
    nfc_device_set_data(mifare_fuzzer_worker->nfc_device, NfcProtocolIso14443_3a, nfc_14a_data);

    iso14443_3a_free(nfc_14a_data);
}

/// @brief mifare_fuzzer_worker_get_nfc_data()
/// @param mifare_fuzzer_worker
/// @return
Iso14443_3aData mifare_fuzzer_worker_get_nfc_data(MifareFuzzerWorker* mifare_fuzzer_worker) {
    return mifare_fuzzer_worker->nfc_data;
}
