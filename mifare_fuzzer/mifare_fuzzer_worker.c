#include "mifare_fuzzer_worker.h"
#include "mifare_fuzzer_i.h"

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
    FURI_LOG_D(TAG, "mifare_fuzzer_worker_task()");

    if(mifare_fuzzer_worker->state == MifareFuzzerWorkerStateEmulate) {
        NfcDevice* device = mifare_fuzzer_worker->nfc_device;
        NfcProtocol protocol = nfc_device_get_protocol(device);

        FURI_LOG_D(
            TAG,
            "mifare_fuzzer_worker_task() :: Allocating and starting with %s",
            nfc_device_get_protocol_name(protocol));
        mifare_fuzzer_worker->nfc_listener = nfc_listener_alloc(
            mifare_fuzzer_worker->nfc, protocol, nfc_device_get_data(device, protocol));
        nfc_listener_start(mifare_fuzzer_worker->nfc_listener, NULL, NULL);

        FURI_LOG_D(TAG, "mifare_fuzzer_worker_task() :: Emulating...");
        while(mifare_fuzzer_worker->state == MifareFuzzerWorkerStateEmulate) {
            furi_delay_ms(50);
        }

        FURI_LOG_D(TAG, "mifare_fuzzer_worker_task() :: Stopping NFC Listener");
        nfc_listener_stop(mifare_fuzzer_worker->nfc_listener);
        nfc_listener_free(mifare_fuzzer_worker->nfc_listener);
    }

    FURI_LOG_D(TAG, "mifare_fuzzer_worker_task() :: Stopping");
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

/// @brief mifare_fuzzer_worker_set_nfc_device()
/// @param mifare_fuzzer_worker
/// @param nfc_device
void mifare_fuzzer_worker_set_nfc_device(
    MifareFuzzerWorker* mifare_fuzzer_worker,
    NfcDevice* nfc_device) {
    FURI_LOG_D(
        TAG,
        "mifare_fuzzer_worker_set_nfc_device() :: Protocol: %s",
        nfc_device_get_protocol_name(nfc_device_get_protocol(nfc_device)));
    mifare_fuzzer_worker->nfc_device = nfc_device;
}

/// @brief mifare_fuzzer_worker_get_nfc_device()
/// @param mifare_fuzzer_worker
/// @return
NfcDevice* mifare_fuzzer_worker_get_nfc_device(MifareFuzzerWorker* mifare_fuzzer_worker) {
    return mifare_fuzzer_worker->nfc_device;
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

    FURI_LOG_D(
        TAG,
        "mifare_fuzzer_worker_set_nfc_data() :: Clearing nfc_device and setting Iso14443_3aData");
    nfc_device_clear(mifare_fuzzer_worker->nfc_device);
    nfc_device_set_data(mifare_fuzzer_worker->nfc_device, NfcProtocolIso14443_3a, nfc_14a_data);
    FURI_LOG_D(
        TAG,
        "mifare_fuzzer_worker_set_nfc_data() :: Protocol: %s",
        nfc_device_get_protocol_name(nfc_device_get_protocol(mifare_fuzzer_worker->nfc_device)));
    iso14443_3a_free(nfc_14a_data);
}

/// @brief mifare_fuzzer_worker_get_nfc_data()
/// @param mifare_fuzzer_worker
/// @return
Iso14443_3aData mifare_fuzzer_worker_get_nfc_data(MifareFuzzerWorker* mifare_fuzzer_worker) {
    return mifare_fuzzer_worker->nfc_data;
}