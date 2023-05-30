#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

#define MF_DESFIRE_BUF_SIZE_MAX (64U)

typedef MfDesfirePollerCommand (*MfDesfirePollerReadHandler)(MfDesfirePoller* instance);

static Iso14443_4aPollerCommand mf_desfire_process_command(MfDesfirePollerCommand command) {
    Iso14443_4aPollerCommand ret = Iso14443_4aPollerCommandContinue;

    if(command == MfDesfirePollerCommandContinue) {
        ret = Iso14443_4aPollerCommandContinue;
    } else if(command == MfDesfirePollerCommandReset) {
        ret = Iso14443_4aPollerCommandReset;
    } else if(command == MfDesfirePollerCommandStop) {
        ret = Iso14443_4aPollerCommandStop;
    } else {
        furi_crash("Unknown command");
    }

    return ret;
}

MfDesfirePoller* mf_desfire_poller_alloc(Iso14443_4aPoller* iso14443_4a_poller) {
    MfDesfirePoller* instance = malloc(sizeof(MfDesfirePoller));
    instance->iso14443_4a_poller = iso14443_4a_poller;

    return instance;
}

void mf_desfire_poller_free(MfDesfirePoller* instance) {
    furi_assert(instance);
    free(instance);
}

static MfDesfirePollerCommand mf_desfire_poller_handler_idle(MfDesfirePoller* instance) {
    nfc_poller_buffer_reset(instance->buffer);
    iso14443_4a_poller_get_data(instance->iso14443_4a_poller, &instance->data->iso14443_4a_data);

    instance->state = MfDesfirePollerStateReadVersion;
    return MfDesfirePollerCommandContinue;
}

static MfDesfirePollerCommand mf_desfire_poller_handler_read_version(MfDesfirePoller* instance) {
    instance->error = mf_desfire_poller_async_read_version(instance, &instance->data->version);
    if(instance->error == MfDesfireErrorNone) {
        FURI_LOG_D(TAG, "Read version success");
        instance->state = MfDesfirePollerStateReadSuccess;
    } else {
        FURI_LOG_E(TAG, "Failed to read version");
        iso14443_4a_poller_halt(instance->iso14443_4a_poller);
        instance->state = MfDesfirePollerStateReadFailed;
    }

    return MfDesfirePollerCommandContinue;
}

static MfDesfirePollerCommand mf_desfire_poller_handler_read_fail(MfDesfirePoller* instance) {
    FURI_LOG_D(TAG, "Read Failed");
    iso14443_4a_poller_halt(instance->iso14443_4a_poller);
    MfDesfirePollerEventData event_data = {.error = instance->error};
    MfDesfirePollerEvent event = {.type = MfDesfirePollerEventTypeReadFailed, .data = &event_data};
    MfDesfirePollerCommand command = instance->callback(event, instance->context);
    instance->state = MfDesfirePollerStateIdle;
    return command;
}

static MfDesfirePollerCommand mf_desfire_poller_handler_read_success(MfDesfirePoller* instance) {
    FURI_LOG_D(TAG, "Read success.");
    iso14443_4a_poller_halt(instance->iso14443_4a_poller);
    MfDesfirePollerEvent event = {.type = MfDesfirePollerEventTypeReadSuccess};
    MfDesfirePollerCommand command = instance->callback(event, instance->context);
    return command;
}

static const MfDesfirePollerReadHandler mf_desfire_poller_read_handler[MfDesfirePollerStateNum] = {
    [MfDesfirePollerStateIdle] = mf_desfire_poller_handler_idle,
    [MfDesfirePollerStateReadVersion] = mf_desfire_poller_handler_read_version,
    [MfDesfirePollerStateReadFailed] = mf_desfire_poller_handler_read_fail,
    [MfDesfirePollerStateReadSuccess] = mf_desfire_poller_handler_read_success,
};

static Iso14443_4aPollerCommand
    mf_desfire_poller_read_callback(Iso14443_4aPollerEvent event, void* context) {
    furi_assert(context);

    MfDesfirePoller* instance = context;
    MfDesfirePollerEventData event_data = {};
    MfDesfirePollerEvent poller_event = {.data = &event_data};
    MfDesfirePollerCommand command = MfDesfirePollerCommandContinue;

    furi_assert(instance->session_state != MfDesfirePollerSessionStateIdle);
    if(instance->session_state == MfDesfirePollerSessionStateStopRequest) {
        command = MfDesfirePollerCommandStop;
    } else {
        if(event.type == Iso14443_4aPollerEventTypeReady) {
            command = mf_desfire_poller_read_handler[instance->state](instance);
        } else if(event.type == Iso14443_4aPollerEventTypeError) {
            if(instance->callback) {
                poller_event.type = MfDesfirePollerEventTypeReadFailed;
                command = instance->callback(poller_event, instance->context);
            }
        }
    }

    return mf_desfire_process_command(command);
}

MfDesfireError mf_desfire_poller_start(
    MfDesfirePoller* instance,
    Iso14443_4aPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfDesfirePollerStateIdle);
    furi_assert(instance->iso14443_4a_poller);
    furi_assert(callback);
    furi_assert(instance->session_state == MfDesfirePollerSessionStateIdle);

    instance->data = malloc(sizeof(MfDesfireData));
    instance->buffer = nfc_poller_buffer_alloc(MF_DESFIRE_BUF_SIZE_MAX, MF_DESFIRE_BUF_SIZE_MAX);

    instance->session_state = MfDesfirePollerSessionStateActive;
    iso14443_4a_poller_read(instance->iso14443_4a_poller, callback, context);

    return MfDesfireErrorNone;
}

MfDesfireError mf_desfire_poller_read(
    MfDesfirePoller* instance,
    MfDesfirePollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfDesfirePollerStateIdle);
    furi_assert(instance->iso14443_4a_poller);
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

MfDesfireError mf_desfire_poller_reset(MfDesfirePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->buffer);
    furi_assert(instance->iso14443_4a_poller);

    nfc_poller_buffer_free(instance->buffer);
    instance->callback = NULL;
    instance->context = NULL;
    instance->state = MfDesfirePollerStateIdle;

    return MfDesfireErrorNone;
}

MfDesfireError mf_desfire_poller_stop(MfDesfirePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->iso14443_4a_poller);

    instance->session_state = MfDesfirePollerSessionStateStopRequest;
    iso14443_4a_poller_stop(instance->iso14443_4a_poller);
    instance->session_state = MfDesfirePollerSessionStateIdle;
    free(instance->data);

    return mf_desfire_poller_reset(instance);
}
