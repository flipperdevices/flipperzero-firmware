#include "nfc_magic_worker_i.h"

#include "nfc_magic_i.h"
#include "lib/magic/common.h"
#include "lib/magic/classic_gen1.h"
#include "lib/magic/gen4.h"

#define TAG "NfcMagicWorker"

static void
    nfc_magic_worker_change_state(NfcMagicWorker* nfc_magic_worker, NfcMagicWorkerState state) {
    furi_assert(nfc_magic_worker);

    nfc_magic_worker->state = state;
}

NfcMagicWorker* nfc_magic_worker_alloc() {
    NfcMagicWorker* nfc_magic_worker = malloc(sizeof(NfcMagicWorker));

    // Worker thread attributes
    nfc_magic_worker->thread =
        furi_thread_alloc_ex("NfcMagicWorker", 8192, nfc_magic_worker_task, nfc_magic_worker);

    nfc_magic_worker->callback = NULL;
    nfc_magic_worker->context = NULL;

    nfc_magic_worker_change_state(nfc_magic_worker, NfcMagicWorkerStateReady);

    return nfc_magic_worker;
}

void nfc_magic_worker_free(NfcMagicWorker* nfc_magic_worker) {
    furi_assert(nfc_magic_worker);

    furi_thread_free(nfc_magic_worker->thread);
    free(nfc_magic_worker);
}

void nfc_magic_worker_stop(NfcMagicWorker* nfc_magic_worker) {
    furi_assert(nfc_magic_worker);

    nfc_magic_worker_change_state(nfc_magic_worker, NfcMagicWorkerStateStop);
    furi_thread_join(nfc_magic_worker->thread);
}

void nfc_magic_worker_start(
    NfcMagicWorker* nfc_magic_worker,
    NfcMagicWorkerState state,
    NfcDeviceData* dev_data,
    NfcMagicWorkerCallback callback,
    void* context) {
    furi_assert(nfc_magic_worker);
    furi_assert(dev_data);

    nfc_magic_worker->callback = callback;
    nfc_magic_worker->context = context;
    nfc_magic_worker->dev_data = dev_data;
    nfc_magic_worker_change_state(nfc_magic_worker, state);
    furi_thread_start(nfc_magic_worker->thread);
}

int32_t nfc_magic_worker_task(void* context) {
    NfcMagicWorker* nfc_magic_worker = context;

    if(nfc_magic_worker->state == NfcMagicWorkerStateCheck) {
        nfc_magic_worker_check(nfc_magic_worker);
    } else if(nfc_magic_worker->state == NfcMagicWorkerStateWrite) {
        nfc_magic_worker_write(nfc_magic_worker);
    } else if(nfc_magic_worker->state == NfcMagicWorkerStateWipe) {
        nfc_magic_worker_wipe(nfc_magic_worker);
    }

    nfc_magic_worker_change_state(nfc_magic_worker, NfcMagicWorkerStateReady);

    return 0;
}

void nfc_magic_worker_write(NfcMagicWorker* nfc_magic_worker) {
    bool card_found_notified = false;
    FuriHalNfcDevData nfc_data = {};
    MfClassicData* src_data = &nfc_magic_worker->dev_data->mf_classic_data;

    while(nfc_magic_worker->state == NfcMagicWorkerStateWrite) {
        if(furi_hal_nfc_detect(&nfc_data, 200)) {
            if(!card_found_notified) {
                nfc_magic_worker->callback(
                    NfcMagicWorkerEventCardDetected, nfc_magic_worker->context);
                card_found_notified = true;
            }
            furi_hal_nfc_sleep();

            magic_activate();
            if(!magic_gen1_wupa()) {
                FURI_LOG_E(TAG, "Not Magic card");
                nfc_magic_worker->callback(
                    NfcMagicWorkerEventWrongCard, nfc_magic_worker->context);
                break;
            }
            if(!magic_gen1_data_access_cmd()) {
                FURI_LOG_E(TAG, "Not Magic card");
                nfc_magic_worker->callback(
                    NfcMagicWorkerEventWrongCard, nfc_magic_worker->context);
                break;
            }
            for(size_t i = 0; i < 64; i++) {
                FURI_LOG_D(TAG, "Writing block %d", i);
                if(!magic_gen1_write_blk(i, &src_data->block[i])) {
                    FURI_LOG_E(TAG, "Failed to write %d block", i);
                    nfc_magic_worker->callback(NfcMagicWorkerEventFail, nfc_magic_worker->context);
                    break;
                }
            }
            nfc_magic_worker->callback(NfcMagicWorkerEventSuccess, nfc_magic_worker->context);
            break;
        } else {
            if(card_found_notified) {
                nfc_magic_worker->callback(
                    NfcMagicWorkerEventNoCardDetected, nfc_magic_worker->context);
                card_found_notified = false;
            }
        }
        furi_delay_ms(300);
    }
    magic_deactivate();
}

void nfc_magic_worker_check(NfcMagicWorker* nfc_magic_worker) {
    NfcMagic* nfc_magic = nfc_magic_worker->context;
    bool card_found_notified = false;
    uint8_t gen4_config[MAGIC_GEN4_CONFIG_LEN];

    while(nfc_magic_worker->state == NfcMagicWorkerStateCheck) {
        magic_activate();
        if(magic_gen1_wupa()) {
            nfc_magic->card_type = MagicTypeClassicGen1A;
            if(!card_found_notified) {
                nfc_magic_worker->callback(
                    NfcMagicWorkerEventCardDetected, nfc_magic_worker->context);
                card_found_notified = true;
            }

            nfc_magic_worker->callback(NfcMagicWorkerEventSuccess, nfc_magic_worker->context);
            break;
        }

        magic_deactivate();
        furi_delay_ms(300);
        magic_activate();

        uint32_t cuid;
        furi_hal_nfc_activate_nfca(200, &cuid);
        if(magic_gen4_get_cfg(MAGIC_GEN4_DEFAULT_PWD, gen4_config)) {
            nfc_magic->card_type = MagicTypeGen4;
            if(!card_found_notified) {
                nfc_magic_worker->callback(
                    NfcMagicWorkerEventCardDetected, nfc_magic_worker->context);
                card_found_notified = true;
            }

            nfc_magic_worker->callback(NfcMagicWorkerEventSuccess, nfc_magic_worker->context);
            break;
        }

        if(card_found_notified) {
            nfc_magic_worker->callback(
                NfcMagicWorkerEventNoCardDetected, nfc_magic_worker->context);
            card_found_notified = false;
        }
        furi_delay_ms(300);
    }
    magic_deactivate();
}

void nfc_magic_worker_wipe(NfcMagicWorker* nfc_magic_worker) {
    MfClassicBlock block;
    memset(&block, 0, sizeof(MfClassicBlock));
    block.value[0] = 0x01;
    block.value[1] = 0x02;
    block.value[2] = 0x03;
    block.value[3] = 0x04;
    block.value[4] = 0x04;
    block.value[5] = 0x08;
    block.value[6] = 0x04;

    while(nfc_magic_worker->state == NfcMagicWorkerStateWipe) {
        magic_deactivate();
        furi_delay_ms(300);
        if(!magic_activate()) continue;
        if(!magic_gen1_wupa()) continue;
        if(!magic_gen1_wipe()) continue;
        if(!magic_gen1_data_access_cmd()) continue;
        if(!magic_gen1_write_blk(0, &block)) continue;
        nfc_magic_worker->callback(NfcMagicWorkerEventSuccess, nfc_magic_worker->context);
        break;
    }
    magic_deactivate();
}
