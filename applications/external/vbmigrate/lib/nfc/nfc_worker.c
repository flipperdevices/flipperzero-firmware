#include "nfc_worker_i.h"
#include <furi_hal.h>

#include <platform.h>

#define TAG "NfcWorker"

/***************************** NFC Worker API *******************************/

NfcWorker* nfc_worker_alloc() {
    NfcWorker* nfc_worker = malloc(sizeof(NfcWorker));

    // Worker thread attributes
    nfc_worker->thread = furi_thread_alloc_ex("NfcWorker", 8192, nfc_worker_task, nfc_worker);

    nfc_worker->callback = NULL;
    nfc_worker->context = NULL;
    nfc_worker->storage = furi_record_open(RECORD_STORAGE);

    // Initialize rfal
    while(furi_hal_nfc_is_busy()) {
        furi_delay_ms(10);
    }
    nfc_worker_change_state(nfc_worker, NfcWorkerStateReady);

    nfc_worker->reader_analyzer = reader_analyzer_alloc(nfc_worker->storage);

    return nfc_worker;
}

void nfc_worker_free(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);

    furi_thread_free(nfc_worker->thread);

    furi_record_close(RECORD_STORAGE);

    reader_analyzer_free(nfc_worker->reader_analyzer);

    free(nfc_worker);
}

NfcWorkerState nfc_worker_get_state(NfcWorker* nfc_worker) {
    return nfc_worker->state;
}

void nfc_worker_start(
    NfcWorker* nfc_worker,
    NfcWorkerState state,
    NfcDeviceData* dev_data,
    NfcWorkerCallback callback,
    void* context) {
    furi_assert(nfc_worker);
    furi_assert(dev_data);
    while(furi_hal_nfc_is_busy()) {
        furi_delay_ms(10);
    }
    // XXX: These functions are not exported, and trying to link them separately causes
    // catastrophic issues. They weren't here before, so we'll pretend they're not
    // necessary.
    // furi_hal_nfc_deinit();
    // furi_hal_nfc_init();

    nfc_worker->callback = callback;
    nfc_worker->context = context;
    nfc_worker->dev_data = dev_data;
    nfc_worker_change_state(nfc_worker, state);
    furi_thread_start(nfc_worker->thread);
}

void nfc_worker_stop(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->thread);
    if(furi_thread_get_state(nfc_worker->thread) != FuriThreadStateStopped) {
        furi_hal_nfc_stop();
        nfc_worker_change_state(nfc_worker, NfcWorkerStateStop);
        furi_thread_join(nfc_worker->thread);
    }
}

void nfc_worker_change_state(NfcWorker* nfc_worker, NfcWorkerState state) {
    nfc_worker->state = state;
}

/***************************** NFC Worker Thread *******************************/

int32_t nfc_worker_task(void* context) {
    NfcWorker* nfc_worker = context;

    furi_hal_nfc_exit_sleep();

    if(nfc_worker->state == NfcWorkerStateRead) {
        if(nfc_worker->dev_data->read_mode == NfcReadModeAuto) {
            nfc_worker_read(nfc_worker);
        } else {
            nfc_worker_read_type(nfc_worker);
        }
    } else if(nfc_worker->state == NfcWorkerStateUidEmulate) {
        nfc_worker_emulate_uid(nfc_worker);
    } else if(nfc_worker->state == NfcWorkerStateMfUltralightEmulate) {
        nfc_worker_emulate_mf_ultralight(nfc_worker);
    } else if(nfc_worker->state == NfcWorkerStateReadMfUltralightReadAuth) {
        nfc_worker_mf_ultralight_read_auth(nfc_worker);
    }
    furi_hal_nfc_sleep();
    nfc_worker_change_state(nfc_worker, NfcWorkerStateReady);

    return 0;
}

static bool nfc_worker_read_mf_ultralight(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    bool read_success = false;
    MfUltralightReader reader = {};
    MfUltralightData data = {};

    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        reader_analyzer_prepare_tx_rx(nfc_worker->reader_analyzer, tx_rx, false);
        reader_analyzer_start(nfc_worker->reader_analyzer, ReaderAnalyzerModeDebugLog);
    }

    do {
        // Otherwise, try to read as usual
        if(!furi_hal_nfc_detect(&nfc_worker->dev_data->nfc_data, 200)) break;
        if(!mf_ul_read_card(tx_rx, &reader, &data)) break;
        // Copy data
        nfc_worker->dev_data->mf_ul_data = data;
        read_success = true;
    } while(false);

    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        reader_analyzer_stop(nfc_worker->reader_analyzer);
    }

    return read_success;
}

static bool nfc_worker_read_nfca(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    FuriHalNfcDevData* nfc_data = &nfc_worker->dev_data->nfc_data;
    FuriHalNfcADevData* a_data = &nfc_data->a_data;

    bool card_read = false;
    furi_hal_nfc_sleep();
    if(mf_ul_check_card_type(a_data)) {
        FURI_LOG_I(TAG, "Mifare Ultralight / NTAG detected");
        nfc_worker->dev_data->protocol = NfcDeviceProtocolMifareUl;
        card_read = nfc_worker_read_mf_ultralight(nfc_worker, tx_rx);
    } else {
        nfc_worker->dev_data->protocol = NfcDeviceProtocolUnknown;
        card_read = true;
    }

    return card_read;
}

void nfc_worker_read(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->callback);

    nfc_device_data_clear(nfc_worker->dev_data);
    NfcDeviceData* dev_data = nfc_worker->dev_data;
    FuriHalNfcDevData* nfc_data = &nfc_worker->dev_data->nfc_data;
    FuriHalNfcTxRxContext tx_rx = {};
    NfcWorkerEvent event = 0;
    bool card_not_detected_notified = false;

    while(nfc_worker->state == NfcWorkerStateRead) {
        if(furi_hal_nfc_detect(nfc_data, 300)) {
            // Process first found device
            nfc_worker->callback(NfcWorkerEventCardDetected, nfc_worker->context);
            card_not_detected_notified = false;
            if(nfc_data->type == FuriHalNfcTypeA) {
                if(nfc_worker_read_nfca(nfc_worker, &tx_rx)) {
                    if(dev_data->protocol == NfcDeviceProtocolMifareUl) {
                        event = NfcWorkerEventReadMfUltralight;
                        break;
                    } else if(dev_data->protocol == NfcDeviceProtocolUnknown) {
                        event = NfcWorkerEventReadUidNfcA;
                        break;
                    }
                }
            } else if(nfc_data->type == FuriHalNfcTypeB) {
                event = NfcWorkerEventReadUidNfcB;
                break;
            } else if(nfc_data->type == FuriHalNfcTypeF) {
                event = NfcWorkerEventReadUidNfcF;
                break;
            } else if(nfc_data->type == FuriHalNfcTypeV) {
                event = NfcWorkerEventReadUidNfcV;
                break;
            }
        } else {
            if(!card_not_detected_notified) {
                nfc_worker->callback(NfcWorkerEventNoCardDetected, nfc_worker->context);
                card_not_detected_notified = true;
            }
        }
        furi_hal_nfc_sleep();
        furi_delay_ms(100);
    }
    // Notify caller and exit
    if(event > NfcWorkerEventReserved) {
        nfc_worker->callback(event, nfc_worker->context);
    }
}

void nfc_worker_read_type(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->callback);

    NfcReadMode read_mode = nfc_worker->dev_data->read_mode;
    nfc_device_data_clear(nfc_worker->dev_data);
    FuriHalNfcDevData* nfc_data = &nfc_worker->dev_data->nfc_data;
    FuriHalNfcTxRxContext tx_rx = {};
    NfcWorkerEvent event = 0;
    bool card_not_detected_notified = false;

    while(nfc_worker->state == NfcWorkerStateRead) {
        if(furi_hal_nfc_detect(nfc_data, 300)) {
            FURI_LOG_D(TAG, "Card detected");
            furi_hal_nfc_sleep();
            // Process first found device
            nfc_worker->callback(NfcWorkerEventCardDetected, nfc_worker->context);
            card_not_detected_notified = false;
            if(nfc_data->type == FuriHalNfcTypeA) {
                if(read_mode == NfcReadModeMfUltralight) {
                    FURI_LOG_I(TAG, "Mifare Ultralight / NTAG");
                    nfc_worker->dev_data->protocol = NfcDeviceProtocolMifareUl;
                    if(nfc_worker_read_mf_ultralight(nfc_worker, &tx_rx)) {
                        event = NfcWorkerEventReadMfUltralight;
                        break;
                    }
                } else if(read_mode == NfcReadModeNFCA) {
                    nfc_worker->dev_data->protocol = NfcDeviceProtocolUnknown;
                    event = NfcWorkerEventReadUidNfcA;
                    break;
                }
            }
        } else {
            if(!card_not_detected_notified) {
                nfc_worker->callback(NfcWorkerEventNoCardDetected, nfc_worker->context);
                card_not_detected_notified = true;
            }
        }
        furi_hal_nfc_sleep();
        furi_delay_ms(100);
    }
    // Notify caller and exit
    if(event > NfcWorkerEventReserved) {
        nfc_worker->callback(event, nfc_worker->context);
    }
}

void nfc_worker_emulate_uid(NfcWorker* nfc_worker) {
    FuriHalNfcTxRxContext tx_rx = {};
    FuriHalNfcDevData* data = &nfc_worker->dev_data->nfc_data;
    NfcReaderRequestData* reader_data = &nfc_worker->dev_data->reader_data;

    // TODO add support for RATS
    // Need to save ATS to support ISO-14443A-4 emulation

    while(nfc_worker->state == NfcWorkerStateUidEmulate) {
        if(furi_hal_nfc_listen(data->uid, data->uid_len, data->atqa, data->sak, false, 100)) {
            if(furi_hal_nfc_tx_rx(&tx_rx, 100)) {
                reader_data->size = tx_rx.rx_bits / 8;
                if(reader_data->size > 0) {
                    memcpy(reader_data->data, tx_rx.rx_data, reader_data->size);
                    if(nfc_worker->callback) {
                        nfc_worker->callback(NfcWorkerEventSuccess, nfc_worker->context);
                    }
                }
            } else {
                FURI_LOG_E(TAG, "Failed to get reader commands");
            }
        }
    }
}

void nfc_worker_mf_ultralight_auth_received_callback(MfUltralightAuth auth, void* context) {
    furi_assert(context);

    NfcWorker* nfc_worker = context;
    nfc_worker->dev_data->mf_ul_auth = auth;
    if(nfc_worker->callback) {
        nfc_worker->callback(NfcWorkerEventMfUltralightPwdAuth, nfc_worker->context);
    }
}

void nfc_worker_emulate_mf_ultralight(NfcWorker* nfc_worker) {
    FuriHalNfcDevData* nfc_data = &nfc_worker->dev_data->nfc_data;
    MfUltralightEmulator emulator = {};
    mf_ul_prepare_emulation(&emulator, &nfc_worker->dev_data->mf_ul_data);

    // TODO rework with reader analyzer
    emulator.auth_received_callback = nfc_worker_mf_ultralight_auth_received_callback;
    emulator.context = nfc_worker;

    while(nfc_worker->state == NfcWorkerStateMfUltralightEmulate) {
        mf_ul_reset_emulation(&emulator, true);
        furi_hal_nfc_emulate_nfca(
            nfc_data->uid,
            nfc_data->uid_len,
            nfc_data->atqa,
            nfc_data->sak,
            mf_ul_prepare_emulation_response,
            &emulator,
            5000);
        // Check if data was modified
        if(emulator.data_changed) {
            nfc_worker->dev_data->mf_ul_data = emulator.data;
            if(nfc_worker->callback) {
                nfc_worker->callback(NfcWorkerEventSuccess, nfc_worker->context);
            }
            emulator.data_changed = false;
        }
    }
}

void nfc_worker_mf_ultralight_read_auth(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->callback);

    MfUltralightData* data = &nfc_worker->dev_data->mf_ul_data;
    FuriHalNfcDevData* nfc_data = &nfc_worker->dev_data->nfc_data;
    FuriHalNfcTxRxContext tx_rx = {};
    MfUltralightReader reader = {};
    mf_ul_reset(data);

    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        reader_analyzer_prepare_tx_rx(nfc_worker->reader_analyzer, &tx_rx, true);
        reader_analyzer_start(nfc_worker->reader_analyzer, ReaderAnalyzerModeDebugLog);
    }

    uint32_t key = 0;
    uint16_t pack = 0;
    while(nfc_worker->state == NfcWorkerStateReadMfUltralightReadAuth) {
        furi_hal_nfc_sleep();
        if(furi_hal_nfc_detect(nfc_data, 300) && nfc_data->type == FuriHalNfcTypeA) {
            if(mf_ul_check_card_type(&nfc_data->a_data)) {
                nfc_worker->callback(NfcWorkerEventCardDetected, nfc_worker->context);
                if(data->auth_method == MfUltralightAuthMethodManual ||
                   data->auth_method == MfUltralightAuthMethodAuto) {
                    nfc_worker->callback(NfcWorkerEventMfUltralightPassKey, nfc_worker->context);
                    key = nfc_util_bytes2num(data->auth_key, 4);
                } else {
                    FURI_LOG_E(TAG, "Incorrect auth method");
                    break;
                }

                data->auth_success = mf_ultralight_authenticate(&tx_rx, key, &pack);

                if(!data->auth_success) {
                    // Reset card
                    furi_hal_nfc_sleep();
                    if(!furi_hal_nfc_activate_nfca(300, NULL)) {
                        nfc_worker->callback(NfcWorkerEventFail, nfc_worker->context);
                        break;
                    }
                }

                mf_ul_read_card(&tx_rx, &reader, data);
                if(data->auth_success) {
                    MfUltralightConfigPages* config_pages = mf_ultralight_get_config_pages(data);
                    if(config_pages != NULL) {
                        config_pages->auth_data.pwd.value = REVERSE_BYTES_U32(key);
                        config_pages->auth_data.pack.value = pack;
                    }
                    nfc_worker->callback(NfcWorkerEventSuccess, nfc_worker->context);
                    break;
                } else {
                    nfc_worker->callback(NfcWorkerEventFail, nfc_worker->context);
                    break;
                }
            } else {
                nfc_worker->callback(NfcWorkerEventWrongCardDetected, nfc_worker->context);
                furi_delay_ms(10);
            }
        } else {
            nfc_worker->callback(NfcWorkerEventNoCardDetected, nfc_worker->context);
            furi_delay_ms(10);
        }
    }

    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        reader_analyzer_stop(nfc_worker->reader_analyzer);
    }
}
