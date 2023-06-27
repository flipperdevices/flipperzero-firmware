#include "nfca_poller_sync_api.h"

#include "nfca_poller_i.h"
#include <nfc/nfc_poller.h>

#include <furi/furi.h>

#define NFCA_POLLER_FLAG_COMMAND_COMPLETE (1UL << 0)

typedef struct {
    NfcaPoller* instance;
    FuriThreadId thread_id;
    NfcaError error;
    NfcaData data;
} NfcaPollerContext;

NfcCommand nfca_poller_read_callback(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.poller);
    furi_assert(event.protocol_type == NfcProtocolTypeIso14443_3a);

    NfcaPollerContext* poller_context = context;
    NfcaPoller* nfca_poller = event.poller;
    NfcaPollerEvent* nfca_event = event.data;

    if(nfca_event->type == NfcaPollerEventTypeReady) {
        nfca_copy(&poller_context->data, nfca_poller->data);
    }
    poller_context->error = nfca_event->data.error;

    furi_thread_flags_set(poller_context->thread_id, NFCA_POLLER_FLAG_COMMAND_COMPLETE);

    return NfcCommandStop;
}

NfcaError nfca_poller_read(Nfc* nfc, NfcaData* nfca_data) {
    furi_assert(nfc);
    furi_assert(nfca_data);

    NfcaPollerContext poller_context = {};
    poller_context.thread_id = furi_thread_get_current_id();

    NfcPoller* poller = nfc_poller_alloc(nfc, NfcProtocolTypeIso14443_3a);
    nfc_poller_start(poller, nfca_poller_read_callback, &poller_context);
    furi_thread_flags_wait(NFCA_POLLER_FLAG_COMMAND_COMPLETE, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(NFCA_POLLER_FLAG_COMMAND_COMPLETE);

    nfc_poller_stop(poller);
    nfc_poller_free(poller);

    if(poller_context.error == NfcaErrorNone) {
        *nfca_data = poller_context.data;
    }

    return poller_context.error;
}