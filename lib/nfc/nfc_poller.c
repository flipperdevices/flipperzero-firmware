#include "nfc_poller.h"

#include <furi.h>

#include "nfc.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>
#include <lib/nfc/protocols/mf_desfire/mf_desfire_poller.h>

typedef enum {
    NfcPollerOldStateIdle,
    NfcPollerOldStateCheckPresenceNfca,
    NfcPollerOldStateCheckPresenceNfcb,
    NfcPollerOldStateCheckPresenceNfcf,
    NfcPollerOldStateCheckPresenceNfcv,
} NfcPollerOldState;

typedef enum {
    NfcPollerOldSessionStateIdle,
    NfcPollerOldSessionStateActive,
    NfcPollerOldSessionStateStopRequest,
} NfcPollerOldSessionState;

struct NfcPollerOld {
    Nfc* nfc;
    NfcPollerOldSessionState session_state;
    NfcaPoller* nfca_poller;
    NfcaData nfca_data;
    NfcbPoller* nfcb_poller;
    NfcbData nfcb_data;
    NfcPollerOldState state;
    NfcPollerOldEventCallback callback;
    void* context;
};

NfcPollerOld* nfc_poller_alloc(NfcPollerOldCollection* pollers) {
    furi_assert(pollers);
    furi_assert(pollers->nfc);
    furi_assert(pollers->nfca_poller);
    furi_assert(pollers->nfcb_poller);

    NfcPollerOld* instance = malloc(sizeof(NfcPollerOld));
    instance->nfc = pollers->nfc;
    instance->nfca_poller = pollers->nfca_poller;
    instance->nfcb_poller = pollers->nfcb_poller;

    return instance;
}

void nfc_poller_free(NfcPollerOld* instance) {
    furi_assert(instance);
}

static NfcCommand nfc_poller_process_command(NfcPollerOldCommand command) {
    NfcCommand ret = NfcCommandContinue;

    if(command == NfcPollerOldCommandContinue) {
        ret = NfcCommandContinue;
    } else if(command == NfcPollerOldCommandReset) {
        ret = NfcCommandReset;
    } else if(command == NfcPollerOldCommandStop) {
        ret = NfcCommandStop;
    } else {
        furi_crash("Unknown command");
    }

    return ret;
}

static NfcCommand nfc_poller_event_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcPollerOld* instance = context;
    furi_assert(instance->callback);
    furi_assert(instance->session_state != NfcPollerOldSessionStateIdle);

    NfcPollerOldEvent poller_event;
    NfcPollerOldCommand command = NfcPollerOldCommandContinue;

    if(instance->session_state == NfcPollerOldSessionStateStopRequest) {
        command = NfcPollerOldCommandStop;
    } else {
        if(event.type == NfcEventTypeConfigureRequest) {
            if(instance->state == NfcPollerOldStateCheckPresenceNfca) {
                nfca_poller_config(instance->nfca_poller);
            } else if(instance->state == NfcPollerOldStateCheckPresenceNfcb) {
                // nfcb_poller_config(instance->nfcb_poller);
            }
        } else if(event.type == NfcEventTypePollerReady) {
            if(instance->state == NfcPollerOldStateCheckPresenceNfca) {
                NfcaError error =
                    nfca_poller_async_activate(instance->nfca_poller, &instance->nfca_data);
                if(error == NfcaErrorNone) {
                    if(mf_ultralight_detect_protocol(&instance->nfca_data)) {
                        poller_event = NfcPollerOldEventMfUltralightDetected;
                    } else {
                        poller_event = NfcPollerOldEventNfcaDetected;
                    }
                    command = instance->callback(poller_event, instance->context);
                } else {
                    // Nfca not present
                    FURI_LOG_E("TAG", "NOT PRESENT");
                    furi_delay_ms(100);
                    instance->state = NfcPollerOldStateCheckPresenceNfcb;
                    command = NfcPollerOldCommandReset;
                }
            } else if(instance->state == NfcPollerOldStateCheckPresenceNfcb) {
                // NfcbError error =
                //     nfcb_poller_activate(instance->nfcb_poller, &instance->nfcb_data);
                // if(error == NfcbErrorNone) {
                //     poller_event = NfcPollerOldEventNfcbDetected;
                //     instance->callback(poller_event, instance->context);
                // } else {
                //     // Nfcb not present
                //     furi_delay_ms(100);
                instance->state = NfcPollerOldStateCheckPresenceNfca;
                //     command = NfcCommandReset;
                // }
            }
        } else if(event.type == NfcEventTypeReset) {
            if(instance->state == NfcPollerOldStateCheckPresenceNfca) {
                nfca_poller_reset(instance->nfca_poller);
            } else if(instance->state == NfcPollerOldStateCheckPresenceNfcb) {
                // nfcb_poller_reset(instance->nfcb_poller);
            }
        }
    }

    return nfc_poller_process_command(command);
}

void nfc_poller_start(NfcPollerOld* instance, NfcPollerOldEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(instance->session_state == NfcPollerOldSessionStateIdle);

    instance->callback = callback;
    instance->context = context;
    instance->state = NfcPollerOldStateCheckPresenceNfca;
    instance->session_state = NfcPollerOldSessionStateActive;

    instance->nfca_poller->data = malloc(sizeof(NfcaData));

    nfc_start_poller(instance->nfc, nfc_poller_event_callback, instance);
}

void nfc_poller_stop(NfcPollerOld* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    instance->session_state = NfcPollerOldSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = NfcPollerOldSessionStateIdle;

    free(instance->nfca_poller->data);

    instance->callback = NULL;
    instance->context = NULL;
    instance->state = NfcPollerOldStateIdle;
}
