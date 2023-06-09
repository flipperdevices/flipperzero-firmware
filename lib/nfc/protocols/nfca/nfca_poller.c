#include "nfca_poller_i.h"

#include <furi.h>

#define TAG "NFCA"

#define NFCA_POLLER_FLAG_COMMAND_COMPLETE (1UL << 0)

typedef struct {
    NfcaPoller* instance;
    FuriThreadId thread_id;
    NfcaError error;
} NfcaPollerContext;

NfcaPoller* nfca_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    NfcaPoller* instance = malloc(sizeof(NfcaPoller));
    instance->nfc = nfc;

    return instance;
}

void nfca_poller_free(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    free(instance);
}

static NfcCommand nfca_poller_process_command(NfcaPollerCommand command) {
    NfcCommand ret = NfcCommandContinue;

    if(command == NfcaPollerCommandContinue) {
        ret = NfcCommandContinue;
    } else if(command == NfcaPollerCommandReset) {
        ret = NfcCommandReset;
    } else if(command == NfcaPollerCommandStop) {
        ret = NfcCommandStop;
    } else {
        furi_crash("Unknown command");
    }

    return ret;
}

static NfcCommand nfca_poller_event_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcaPoller* instance = context;
    furi_assert(instance->callback);
    furi_assert(instance->session_state != NfcaPollerSessionStateIdle);

    NfcaPollerEvent nfca_poller_event = {};
    NfcaPollerCommand command = NfcaPollerCommandContinue;

    if(instance->session_state == NfcaPollerSessionStateStopRequest) {
        command = NfcaPollerCommandStop;
    } else {
        if(event.type == NfcEventTypeConfigureRequest) {
            nfca_poller_config(instance);
        } else if(event.type == NfcEventTypePollerReady) {
            if(instance->state != NfcaPollerStateActivated) {
                NfcaData data = {};
                NfcaError error = nfca_poller_async_activate(instance, &data);
                if(error == NfcaErrorNone) {
                    nfca_poller_event.type = NfcaPollerEventTypeReady;
                    instance->state = NfcaPollerStateActivated;
                    nfca_poller_event.data.error = error;
                    command = instance->callback(nfca_poller_event, instance->context);
                } else {
                    nfca_poller_event.type = NfcaPollerEventTypeError;
                    nfca_poller_event.data.error = error;
                    command = instance->callback(nfca_poller_event, instance->context);
                    // Add delay to switch context
                    furi_delay_ms(100);
                }
            } else {
                nfca_poller_event.type = NfcaPollerEventTypeReady;
                nfca_poller_event.data.error = NfcaErrorNone;
                command = instance->callback(nfca_poller_event, instance->context);
            }
        } else if(event.type == NfcEventTypeReset) {
            nfca_poller_reset(instance);
        }
    }

    return nfca_poller_process_command(command);
}

NfcaError
    nfca_poller_start(NfcaPoller* instance, NfcaPollerEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);
    furi_assert(instance->session_state == NfcaPollerSessionStateIdle);

    instance->callback = callback;
    instance->context = context;

    instance->data = nfca_alloc();

    instance->session_state = NfcaPollerSessionStateActive;
    nfc_start_poller(instance->nfc, nfca_poller_event_callback, instance);

    return NfcaErrorNone;
}

NfcaError nfca_poller_get_data(NfcaPoller* instance, NfcaData* data) {
    furi_assert(instance);
    furi_assert(data);

    nfca_copy(data, instance->data);
    return NfcaErrorNone;
}

NfcaError nfca_poller_stop(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    NfcaError error = NfcaErrorNone;

    instance->session_state = NfcaPollerSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = NfcaPollerSessionStateIdle;

    if(instance->config_state == NfcaPollerConfigStateDone) {
        error = nfca_poller_reset(instance);
    }

    // Check that data is freed
    furi_assert(instance->buff == NULL);

    nfca_free(instance->data);

    return error;
}

static NfcaPollerCommand nfca_poller_sync_callback(NfcaPollerEvent event, void* context) {
    NfcaPollerContext* nfca_poller_context = context;
    nfca_poller_context->error = event.data.error;
    furi_thread_flags_set(nfca_poller_context->thread_id, NFCA_POLLER_FLAG_COMMAND_COMPLETE);

    return NfcaPollerCommandStop;
}

NfcaError nfca_poller_read(NfcaPoller* instance, NfcaData* nfca_data) {
    furi_assert(instance);

    NfcaPollerContext context = {};
    context.instance = instance;
    context.thread_id = furi_thread_get_current_id();
    nfca_poller_start(instance, nfca_poller_sync_callback, &context);
    furi_thread_flags_wait(NFCA_POLLER_FLAG_COMMAND_COMPLETE, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(NFCA_POLLER_FLAG_COMMAND_COMPLETE);
    if(context.error == NfcaErrorNone) {
        nfca_copy(nfca_data, instance->data);
    }
    nfc_stop(instance->nfc);

    return context.error;
}
