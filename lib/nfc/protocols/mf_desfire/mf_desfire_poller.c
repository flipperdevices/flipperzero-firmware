#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

#define MF_DESFIRE_MAX_BUFF_SIZE (64)

typedef MfDesfirePollerCommand (*MfDesfirePollerReadHandler)(MfDesfirePoller* instance);

static NfcaPollerCommand mf_desfire_process_command(MfDesfirePollerCommand command) {
    NfcaPollerCommand ret = NfcaPollerCommandContinue;

    if(command == MfDesfirePollerCommandContinue) {
        ret = NfcaPollerCommandContinue;
    } else if(command == MfDesfirePollerCommandReset) {
        ret = NfcaPollerCommandReset;
    } else if(command == MfDesfirePollerCommandStop) {
        ret = NfcaPollerCommandStop;
    } else {
        furi_crash("Unknown command");
    }

    return ret;
}

MfDesfirePoller* mf_desfire_poller_alloc(NfcaPoller* nfca_poller) {
    MfDesfirePoller* instance = malloc(sizeof(MfDesfirePoller));
    instance->nfca_poller = nfca_poller;

    return instance;
}

void mf_desfire_poller_free(MfDesfirePoller* instance) {
    furi_assert(instance);
    free(instance);
}

static MfDesfirePollerCommand mf_desfire_poller_handler_idle(MfDesfirePoller* instance) {
    nfc_poller_buffer_reset(instance->buffer);
    nfca_poller_get_data(instance->nfca_poller, &instance->data->nfca_data);

    return MfDesfirePollerCommandContinue;
}

static MfDesfirePollerCommand mf_desfire_poller_handler_read_fail(MfDesfirePoller* instance) {
    FURI_LOG_D(TAG, "Read Failed");
    nfca_poller_halt(instance->nfca_poller);
    MfDesfirePollerEventData event_data = {.error = instance->error};
    MfDesfirePollerEvent event = {.type = MfDesfirePollerEventTypeReadFailed, .data = &event_data};
    MfDesfirePollerCommand command = instance->callback(event, instance->context);
    instance->state = MfDesfirePollerStateIdle;
    return command;
}

static MfDesfirePollerCommand mf_desfire_poller_handler_read_success(MfDesfirePoller* instance) {
    FURI_LOG_D(TAG, "Read success.");
    nfca_poller_halt(instance->nfca_poller);
    MfDesfirePollerEvent event = {.type = MfDesfirePollerEventTypeReadSuccess};
    MfDesfirePollerCommand command = instance->callback(event, instance->context);
    return command;
}

static const MfDesfirePollerReadHandler mf_desfire_poller_read_handler[MfDesfirePollerStateNum] = {
    [MfDesfirePollerStateIdle] = mf_desfire_poller_handler_idle,
    [MfDesfirePollerStateReadFailed] = mf_desfire_poller_handler_read_fail,
    [MfDesfirePollerStateReadSuccess] = mf_desfire_poller_handler_read_success,
};

static NfcaPollerCommand mf_desfire_poller_read_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfDesfirePoller* instance = context;
    MfDesfirePollerEventData event_data = {};
    MfDesfirePollerEvent mf_ul_poller_event = {.data = &event_data};
    MfDesfirePollerCommand command = MfDesfirePollerCommandContinue;

    furi_assert(instance->session_state != MfDesfirePollerSessionStateIdle);
    if(instance->session_state == MfDesfirePollerSessionStateStopRequest) {
        command = MfDesfirePollerCommandStop;
    } else {
        if(event.type == NfcaPollerEventTypeReady) {
            command = mf_desfire_poller_read_handler[instance->state](instance);
        } else if(event.type == NfcaPollerEventTypeError) {
            if(instance->callback) {
                mf_ul_poller_event.type = MfDesfirePollerEventTypeReadFailed;
                command = instance->callback(mf_ul_poller_event, instance->context);
            }
        }
    }

    return mf_desfire_process_command(command);
}

MfDesfireError mf_desfire_poller_start(
    MfDesfirePoller* instance,
    NfcaPollerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfDesfirePollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);
    furi_assert(instance->session_state == MfDesfirePollerSessionStateIdle);

    instance->data = malloc(sizeof(MfDesfireData));
    instance->buffer = nfc_poller_buffer_alloc(MF_DESFIRE_MAX_BUFF_SIZE, MF_DESFIRE_MAX_BUFF_SIZE);

    instance->session_state = MfDesfirePollerSessionStateActive;
    nfca_poller_start(instance->nfca_poller, callback, context);

    return MfDesfireErrorNone;
}

MfDesfireError mf_desfire_poller_read(
    MfDesfirePoller* instance,
    MfDesfirePollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfDesfirePollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    return mf_desfire_poller_start(instance, mf_desfire_poller_read_callback, instance);
}

MfDesfireError mf_desfire_poller_get_data(MfDesfirePoller* instance, MfDesfireData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfDesfireErrorNone;
}

MfDesfireError mf_desfire_poller_stop(MfDesfirePoller* instance) {
    furi_assert(instance);

    return MfDesfireErrorNone;
}
