#include "seader_worker_i.h"

#include <flipper_format/flipper_format.h>
#include <lib/bit_lib/bit_lib.h>

#define TAG "SeaderWorker"

#define APDU_HEADER_LEN 5
#define ASN1_PREFIX 6
#define ASN1_DEBUG true

#define RFAL_PICOPASS_TXRX_FLAGS                                                    \
    (FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_TX_MANUAL | FURI_HAL_NFC_LL_TXRX_FLAGS_AGC_ON | \
     FURI_HAL_NFC_LL_TXRX_FLAGS_PAR_RX_REMV | FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_RX_KEEP)

// TODO: const
uint8_t GET_RESPONSE[] = {0x00, 0xc0, 0x00, 0x00, 0xff};

char display[SEADER_UART_RX_BUF_SIZE * 2 + 1] = {0};

// Forward declaration
void seader_send_card_detected(SeaderUartBridge* seader_uart, CardDetails_t* cardDetails);

/***************************** Seader Worker API *******************************/

SeaderWorker* seader_worker_alloc() {
    SeaderWorker* seader_worker = malloc(sizeof(SeaderWorker));

    // Worker thread attributes
    seader_worker->thread =
        furi_thread_alloc_ex("SeaderWorker", 8192, seader_worker_task, seader_worker);
    seader_worker->messages = furi_message_queue_alloc(3, sizeof(SeaderAPDU));
    seader_worker->mq_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    seader_worker->callback = NULL;
    seader_worker->context = NULL;
    seader_worker->storage = furi_record_open(RECORD_STORAGE);
    memset(seader_worker->sam_version, 0, sizeof(seader_worker->sam_version));

    seader_worker_change_state(seader_worker, SeaderWorkerStateReady);

    return seader_worker;
}

void seader_worker_free(SeaderWorker* seader_worker) {
    furi_assert(seader_worker);

    furi_thread_free(seader_worker->thread);
    furi_message_queue_free(seader_worker->messages);
    furi_mutex_free(seader_worker->mq_mutex);

    furi_record_close(RECORD_STORAGE);

    free(seader_worker);
}

SeaderWorkerState seader_worker_get_state(SeaderWorker* seader_worker) {
    return seader_worker->state;
}

void seader_worker_start(
    SeaderWorker* seader_worker,
    SeaderWorkerState state,
    SeaderUartBridge* uart,
    SeaderWorkerCallback callback,
    void* context) {
    furi_assert(seader_worker);
    furi_assert(uart);

    seader_worker->stage = SeaderPollerEventTypeCardDetect;
    seader_worker->callback = callback;
    seader_worker->context = context;
    seader_worker->uart = uart;
    seader_worker_change_state(seader_worker, state);
    furi_thread_start(seader_worker->thread);
}

void seader_worker_stop(SeaderWorker* seader_worker) {
    furi_assert(seader_worker);
    if(seader_worker->state == SeaderWorkerStateBroken ||
       seader_worker->state == SeaderWorkerStateReady) {
        return;
    }

    seader_worker_change_state(seader_worker, SeaderWorkerStateStop);
    furi_thread_join(seader_worker->thread);
}

void seader_worker_change_state(SeaderWorker* seader_worker, SeaderWorkerState state) {
    seader_worker->state = state;
}

/***************************** Seader Worker Thread *******************************/

bool seader_process_success_response(Seader* seader, uint8_t* apdu, size_t len) {
    SeaderWorker* seader_worker = seader->worker;

    if(seader_process_success_response_i(seader, apdu, len, false, NULL)) {
        // no-op, message was processed
    } else {
        FURI_LOG_I(TAG, "Enqueue SAM message, %d bytes", len);
        uint32_t space = furi_message_queue_get_space(seader_worker->messages);
        if(space > 0) {
            SeaderAPDU seaderApdu = {};
            seaderApdu.len = len;
            memcpy(seaderApdu.buf, apdu, len);

            if(furi_mutex_acquire(seader_worker->mq_mutex, FuriWaitForever) == FuriStatusOk) {
                furi_message_queue_put(seader_worker->messages, &seaderApdu, FuriWaitForever);
                furi_mutex_release(seader_worker->mq_mutex);
            }
        }
    }
    return true;
}

bool seader_worker_process_sam_message(Seader* seader, CCID_Message* message) {
    size_t len = message->dwLength;
    uint8_t* apdu = message->payload;
    SeaderWorker* seader_worker = seader->worker;
    SeaderUartBridge* seader_uart = seader_worker->uart;
    if(len < 2) {
        return false;
    }
    memset(display, 0, sizeof(display));
    for(uint8_t i = 0; i < len; i++) {
        snprintf(display + (i * 2), sizeof(display), "%02x", apdu[i]);
    }
    // FURI_LOG_I(TAG, "APDU: %s", display);

    uint8_t SW1 = apdu[len - 2];
    uint8_t SW2 = apdu[len - 1];

    switch(SW1) {
    case 0x61:
        // FURI_LOG_I(TAG, "Request %d bytes", SW2);
        GET_RESPONSE[4] = SW2;
        seader_ccid_XfrBlock(seader_uart, GET_RESPONSE, sizeof(GET_RESPONSE));
        return true;
        break;

    case 0x90:
        if(SW2 == 0x00) {
            if(len > 2) {
                return seader_process_success_response(seader, apdu, len - 2);
            }
        }
        break;
    }

    return false;
}

void seader_worker_virtual_credential(Seader* seader) {
    SeaderWorker* seader_worker = seader->worker;

    // Detect card
    seader_worker_card_detect(
        seader, 0, NULL, seader->credential->diversifier, sizeof(PicopassSerialNum), NULL, 0);

    bool running = true;
    // Max times the loop will run with no message to process
    uint8_t dead_loops = 20;

    while(running) {
        if(furi_mutex_acquire(seader_worker->mq_mutex, 0) == FuriStatusOk) {
            uint32_t count = furi_message_queue_get_count(seader_worker->messages);
            if(count > 0) {
                FURI_LOG_I(TAG, "Dequeue SAM message [%ld messages]", count);

                SeaderAPDU seaderApdu = {};
                FuriStatus status =
                    furi_message_queue_get(seader_worker->messages, &seaderApdu, FuriWaitForever);
                if(status != FuriStatusOk) {
                    FURI_LOG_W(TAG, "furi_message_queue_get fail %d", status);
                    view_dispatcher_send_custom_event(
                        seader->view_dispatcher, SeaderCustomEventWorkerExit);
                }
                if(seader_process_success_response_i(
                       seader, seaderApdu.buf, seaderApdu.len, true, NULL)) {
                    // no-op
                } else {
                    FURI_LOG_I(TAG, "Response false");
                    running = false;
                }
            }
            furi_mutex_release(seader_worker->mq_mutex);
        } else {
            dead_loops--;
            running = (dead_loops > 0);
            FURI_LOG_D(
                TAG, "Dead loops: %d -> Running: %s", dead_loops, running ? "true" : "false");
        }
        running = (seader_worker->stage != SeaderPollerEventTypeComplete);
    }

    if(dead_loops > 0) {
        FURI_LOG_D(TAG, "Final dead loops: %d", dead_loops);
    } else {
        view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderCustomEventWorkerExit);
    }
}

int32_t seader_worker_task(void* context) {
    SeaderWorker* seader_worker = context;
    Seader* seader = seader_worker->context;
    SeaderUartBridge* seader_uart = seader_worker->uart;

    if(seader_worker->state == SeaderWorkerStateCheckSam) {
        FURI_LOG_D(TAG, "Check for SAM");
        seader_ccid_check_for_sam(seader_uart);
    } else if(seader_worker->state == SeaderWorkerStateVirtualCredential) {
        FURI_LOG_D(TAG, "Virtual Credential");
        seader_worker_virtual_credential(seader);
    }
    seader_worker_change_state(seader_worker, SeaderWorkerStateReady);

    return 0;
}

void seader_worker_poller_conversation(Seader* seader, SeaderPollerContainer* spc) {
    SeaderWorker* seader_worker = seader->worker;

    if(furi_mutex_acquire(seader_worker->mq_mutex, 0) == FuriStatusOk) {
        furi_thread_set_current_priority(FuriThreadPriorityHighest);
        uint32_t count = furi_message_queue_get_count(seader_worker->messages);
        if(count > 0) {
            FURI_LOG_I(TAG, "Dequeue SAM message [%ld messages]", count);

            SeaderAPDU seaderApdu = {};
            FuriStatus status =
                furi_message_queue_get(seader_worker->messages, &seaderApdu, FuriWaitForever);
            if(status != FuriStatusOk) {
                FURI_LOG_W(TAG, "furi_message_queue_get fail %d", status);
                seader_worker->stage = SeaderPollerEventTypeComplete;
                view_dispatcher_send_custom_event(
                    seader->view_dispatcher, SeaderCustomEventWorkerExit);
            }

            if(seader_process_success_response_i(
                   seader, seaderApdu.buf, seaderApdu.len, true, spc)) {
                // no-op
            } else {
                FURI_LOG_I(TAG, "Response false");
                view_dispatcher_send_custom_event(
                    seader->view_dispatcher, SeaderCustomEventWorkerExit);
                seader_worker->stage = SeaderPollerEventTypeComplete;
            }
        }
        furi_mutex_release(seader_worker->mq_mutex);
    } else {
        furi_thread_set_current_priority(FuriThreadPriorityLowest);
    }
}

NfcCommand seader_worker_poller_callback_iso14443_4a(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolIso14443_4a);
    NfcCommand ret = NfcCommandContinue;

    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;

    const Iso14443_4aPollerEvent* iso14443_4a_event = event.event_data;
    SeaderPollerContainer spc = {.iso14443_4a_poller = event.instance};

    if(iso14443_4a_event->type == Iso14443_4aPollerEventTypeReady) {
        if(seader_worker->stage == SeaderPollerEventTypeCardDetect) {
            nfc_device_set_data(
                seader->nfc_device, NfcProtocolIso14443_4a, nfc_poller_get_data(seader->poller));

            size_t uid_len;
            const uint8_t* uid = nfc_device_get_uid(seader->nfc_device, &uid_len);

            const Iso14443_3aData* iso14443_3a_data =
                nfc_device_get_data(seader->nfc_device, NfcProtocolIso14443_3a);
            uint8_t sak = iso14443_3a_get_sak(iso14443_3a_data);

            seader_worker_card_detect(
                seader, sak, (uint8_t*)iso14443_3a_data->atqa, uid, uid_len, NULL, 0);

            // nfc_set_fdt_poll_fc(event.instance, SEADER_POLLER_MAX_FWT);
            furi_thread_set_current_priority(FuriThreadPriorityLowest);
            seader_worker->stage = SeaderPollerEventTypeConversation;
        } else if(seader_worker->stage == SeaderPollerEventTypeConversation) {
            seader_worker_poller_conversation(seader, &spc);
        } else if(seader_worker->stage == SeaderPollerEventTypeComplete) {
            ret = NfcCommandStop;
        }
    } else if(iso14443_4a_event->type == Iso14443_4aPollerEventTypeError) {
        Iso14443_4aPollerEventData* data = iso14443_4a_event->data;
        Iso14443_4aError error = data->error;
        FURI_LOG_W(TAG, "Iso14443_4aError %i", error);
        // I was hoping to catch MFC here, but it seems to be treated the same (None) as no card being present.
        switch(error) {
        case Iso14443_4aErrorNone:
            break;
        case Iso14443_4aErrorNotPresent:
            break;
        case Iso14443_4aErrorProtocol:
            ret = NfcCommandStop;
            break;
        case Iso14443_4aErrorTimeout:
            break;
        case Iso14443_4aErrorSendExtra:
            break;
        }
    }

    return ret;
}

NfcCommand seader_worker_poller_callback_mfc(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolMfClassic);
    NfcCommand ret = NfcCommandContinue;

    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;

    MfClassicPollerEvent* mfc_event = event.event_data;
    SeaderPollerContainer spc = {.mfc_poller = event.instance};

    if(mfc_event->type == MfClassicPollerEventTypeSuccess) {
        if(seader_worker->stage == SeaderPollerEventTypeCardDetect) {
            const MfClassicData* mfc_data = nfc_poller_get_data(seader->poller);
            uint8_t sak = iso14443_3a_get_sak(mfc_data->iso14443_3a_data);
            size_t uid_len = 0;
            const uint8_t* uid = mf_classic_get_uid(mfc_data, &uid_len);
            seader_worker_card_detect(seader, sak, NULL, uid, uid_len, NULL, 0);
            furi_thread_set_current_priority(FuriThreadPriorityLowest);
            seader_worker->stage = SeaderPollerEventTypeConversation;
        } else if(seader_worker->stage == SeaderPollerEventTypeConversation) {
            seader_worker_poller_conversation(seader, &spc);
        } else if(seader_worker->stage == SeaderPollerEventTypeComplete) {
            ret = NfcCommandStop;
        } else if(seader_worker->stage == SeaderPollerEventTypeFail) {
            ret = NfcCommandStop;
        }
    } else if(mfc_event->type == MfClassicPollerEventTypeFail) {
        ret = NfcCommandStop;
    }

    return ret;
}

NfcCommand seader_worker_poller_callback_picopass(PicopassPollerEvent event, void* context) {
    furi_assert(context);
    NfcCommand ret = NfcCommandContinue;

    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;
    // I know this is is passing the same thing that is on seader all the way down, but I prefer the symmetry between the 15a and iso15 stuff
    PicopassPoller* instance = seader->picopass_poller;
    SeaderPollerContainer spc = {.picopass_poller = instance};

    if(event.type == PicopassPollerEventTypeCardDetected) {
        seader_worker->stage = SeaderPollerEventTypeCardDetect;
    } else if(event.type == PicopassPollerEventTypeSuccess) {
        if(seader_worker->stage == SeaderPollerEventTypeCardDetect) {
            uint8_t* csn = picopass_poller_get_csn(instance);
            seader_worker_card_detect(seader, 0, NULL, csn, sizeof(PicopassSerialNum), NULL, 0);
            furi_thread_set_current_priority(FuriThreadPriorityLowest);
            seader_worker->stage = SeaderPollerEventTypeConversation;
        } else if(seader_worker->stage == SeaderPollerEventTypeConversation) {
            seader_worker_poller_conversation(seader, &spc);
        } else if(seader_worker->stage == SeaderPollerEventTypeComplete) {
            ret = NfcCommandStop;
        }
    } else if(event.type == PicopassPollerEventTypeFail) {
        ret = NfcCommandStop;
        FURI_LOG_W(TAG, "PicopassPollerEventTypeFail");
    } else {
        FURI_LOG_D(TAG, "picopass event type %x", event.type);
    }

    return ret;
}
