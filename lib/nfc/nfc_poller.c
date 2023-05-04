#include "nfc_poller.h"

#include <furi.h>

#include "nfc.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>

typedef enum {
    NfcPollerStateIdle,
    NfcPollerStateCheckPresenceNfca,
    NfcPollerStateCheckPresenceNfcb,
    NfcPollerStateCheckPresenceNfcf,
    NfcPollerStateCheckPresenceNfcv,
} NfcPollerState;

typedef enum {
    NfcPollerSessionStateIdle,
    NfcPollerSessionStateActive,
    NfcPollerSessionStateStopRequest,
} NfcPollerSessionState;

struct NfcPoller {
    Nfc* nfc;
    NfcPollerSessionState session_state;
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

static NfcCommand nfc_poller_event_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcPoller* instance = context;
    furi_assert(instance->callback);
    furi_assert(instance->session_state != NfcPollerSessionStateIdle);

    NfcPollerEvent poller_event;
    NfcCommand command = NfcCommandContinue;

    if(instance->session_state == NfcPollerSessionStateStopRequest) {
        command = NfcCommandStop;
    } else {
        if(event.type == NfcEventTypeConfigureRequest) {
            if(instance->state == NfcPollerStateCheckPresenceNfca) {
                nfca_poller_config(instance->nfca_poller);
            } else if(instance->state == NfcPollerStateCheckPresenceNfcb) {
                // nfcb_poller_config(instance->nfcb_poller);
            }
        } else if(event.type == NfcEventTypePollerReady) {
            if(instance->state == NfcPollerStateCheckPresenceNfca) {
                NfcaError error =
                    nfca_poller_async_activate(instance->nfca_poller, &instance->nfca_data);
                if(error == NfcaErrorNone) {
                    if(mf_ultralight_detect_protocol(&instance->nfca_data)) {
                        poller_event = NfcPollerEventMfUltralightDetected;
                    } else {
                        poller_event = NfcPollerEventNfcaDetected;
                    }
                    instance->callback(poller_event, instance->context);
                } else {
                    // Nfca not present
                    FURI_LOG_E("TAG", "NOT PRESENT");
                    furi_delay_ms(100);
                    instance->state = NfcPollerStateCheckPresenceNfcb;
                    command = NfcCommandReset;
                }
            } else if(instance->state == NfcPollerStateCheckPresenceNfcb) {
                // NfcbError error =
                //     nfcb_poller_activate(instance->nfcb_poller, &instance->nfcb_data);
                // if(error == NfcbErrorNone) {
                //     poller_event = NfcPollerEventNfcbDetected;
                //     instance->callback(poller_event, instance->context);
                // } else {
                //     // Nfcb not present
                //     furi_delay_ms(100);
                    instance->state = NfcPollerStateCheckPresenceNfca;
                //     command = NfcCommandReset;
                // }
            }
        } else if(event.type == NfcEventTypeReset) {
            if(instance->state == NfcPollerStateCheckPresenceNfca) {
                nfca_poller_reset(instance->nfca_poller);
            } else if(instance->state == NfcPollerStateCheckPresenceNfcb) {
                // nfcb_poller_reset(instance->nfcb_poller);
            }
        }
    }

    return command;
}

void nfc_poller_start(NfcPoller* instance, NfcPollerEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(instance->session_state == NfcPollerSessionStateIdle);

    instance->callback = callback;
    instance->context = context;
    instance->state = NfcPollerStateCheckPresenceNfca;
    instance->session_state = NfcPollerSessionStateActive;

    nfc_start_worker(instance->nfc, nfc_poller_event_callback, instance);
}

void nfc_poller_stop(NfcPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    instance->session_state = NfcPollerSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = NfcPollerSessionStateIdle;

    instance->callback = NULL;
    instance->context = NULL;
    instance->state = NfcPollerStateIdle;
}
