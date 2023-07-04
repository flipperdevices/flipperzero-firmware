
#include "mifare_fuzzer_worker.h"

/// @brief mifare_fuzzer_worker_alloc()
/// @return
MifareFuzzerWorker* mifare_fuzzer_worker_alloc() {
    MifareFuzzerWorker* mifare_fuzzer_worker = malloc(sizeof(MifareFuzzerWorker));
    // Worker thread attributes
    mifare_fuzzer_worker->thread = furi_thread_alloc_ex(
        "MifareFuzzerWorker", 8192, mifare_fuzzer_worker_task, mifare_fuzzer_worker);
    mifare_fuzzer_worker->state = MifareFuzzerWorkerStateStop;
    return mifare_fuzzer_worker;
}

/// @brief mifare_fuzzer_worker_free()
/// @param mifare_fuzzer_worker
void mifare_fuzzer_worker_free(MifareFuzzerWorker* mifare_fuzzer_worker) {
    furi_assert(mifare_fuzzer_worker);
    furi_thread_free(mifare_fuzzer_worker->thread);
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
        FuriHalNfcDevData params = mifare_fuzzer_worker->nfc_dev_data;

        furi_hal_nfc_exit_sleep();
        while(mifare_fuzzer_worker->state == MifareFuzzerWorkerStateEmulate) {
            furi_hal_nfc_listen(params.uid, params.uid_len, params.atqa, params.sak, false, 500);
            furi_delay_ms(50);
        }
        furi_hal_nfc_sleep();
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

/// @brief mifare_fuzzer_worker_set_nfc_dev_data()
/// @param mifare_fuzzer_worker
/// @param nfc_dev_data
void mifare_fuzzer_worker_set_nfc_dev_data(
    MifareFuzzerWorker* mifare_fuzzer_worker,
    FuriHalNfcDevData nfc_dev_data) {
    mifare_fuzzer_worker->nfc_dev_data = nfc_dev_data;
}

/// @brief mifare_fuzzer_worker_get_nfc_dev_data()
/// @param mifare_fuzzer_worker
/// @return
FuriHalNfcDevData mifare_fuzzer_worker_get_nfc_dev_data(MifareFuzzerWorker* mifare_fuzzer_worker) {
    return mifare_fuzzer_worker->nfc_dev_data;
}
