#include "nfc_poller.h"

#include <furi.h>

#include "nfc.h"
#include "protocols/nfca_poller.h"
#include "protocols/mf_ultralight.h"

typedef enum {
    NfcPollerStateIdle,
    NfcPollerStateCheckPresenceNfca,
    NfcPollerStateCheckPresenceNfcb,
    NfcPollerStateCheckPresenceNfcf,
    NfcPollerStateCheckPresenceNfcv,
} NfcPollerState;

struct NfcPoller {
    Nfc* nfc;
    NfcaPoller* nfca_poller;
    NfcaData nfca_data;
    NfcbPoller* nfcb_poller;
    NfcbData nfcb_data;
    NfcPollerState state;
    NfcPollerEventCallback callback;
    void* context;
};

NfcPoller* nfc_poller_alloc(NfcPollerCollection* pollers) {
    furi_assert(pollers);
    furi_assert(pollers->nfc);
    furi_assert(pollers->nfca_poller);
    furi_assert(pollers->nfcb_poller);

    NfcPoller* instance = malloc(sizeof(NfcPoller));
    instance->nfc = pollers->nfc;
    instance->nfca_poller = pollers->nfca_poller;
    instance->nfcb_poller = pollers->nfcb_poller;

    return instance;
}

void nfc_poller_free(NfcPoller* instance) {
    furi_assert(instance);
}

static void nfc_poller_event_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcPoller* instance = context;
    NfcPollerEvent poller_event;
    if(event.type == NfcEventTypeConfigureRequest) {
        if(instance->state == NfcPollerStateCheckPresenceNfca) {
            nfca_poller_config(instance->nfca_poller);
        } else if(instance->state == NfcPollerStateCheckPresenceNfcb) {
            nfcb_poller_config(instance->nfcb_poller);
        }
    } else if(event.type == NfcEventTypePollerReady) {
        if(instance->state == NfcPollerStateCheckPresenceNfca) {
            NfcaError error = nfca_poller_activate(instance->nfca_poller, &instance->nfca_data);
            if(error == NfcaErrorNone) {
                if(mf_ultralight_detect_protocol(&instance->nfca_data)) {
                    poller_event = NfcPollerEventMfUltralightDetected;
                } else {
                    poller_event = NfcPollerEventNfcaDetected;
                }
                instance->callback(poller_event, instance->context);
            } else {
                // Nfca not present
                furi_delay_ms(100);
                nfc_reset(instance->nfc);
                instance->state = NfcPollerStateCheckPresenceNfcb;
            }
        } else if(instance->state == NfcPollerStateCheckPresenceNfcb) {
            NfcbError error = nfcb_poller_activate(instance->nfcb_poller, &instance->nfcb_data);
            if(error == NfcbErrorNone) {
                poller_event = NfcPollerEventNfcbDetected;
                instance->callback(poller_event, instance->context);
            } else {
                // Nfcb not present
                furi_delay_ms(100);
                nfc_reset(instance->nfc);
                instance->state = NfcPollerStateCheckPresenceNfca;
            }
        }
    }
}

void nfc_poller_start(NfcPoller* instance, NfcPollerEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
    instance->state = NfcPollerStateCheckPresenceNfca;

    nfc_start_worker(instance->nfc, nfc_poller_event_callback, instance);
}

void nfc_poller_reset(NfcPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    nfc_poller_abort(instance->nfc);

    instance->callback = NULL;
    instance->context = NULL;
    instance->state = NfcPollerStateIdle;
}
