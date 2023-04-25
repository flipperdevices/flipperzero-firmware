#include "mf_ultralight_poller_i.h"

#include <furi.h>

#define TAG "MfUltralightPoller"

#define MF_ULTRALIGHT_MAX_BUFF_SIZE (64)

typedef MfUltralightPollerCommand (*MfUltralightPollerReadHandler)(MfUltralightPoller* instance);

static NfcaPollerCommand mf_ultralight_process_command(MfUltralightPollerCommand command) {
    NfcaPollerCommand ret = NfcaPollerCommandContinue;

    if(command == MfUltralightPollerCommandContinue) {
        ret = NfcaPollerCommandContinue;
    } else if(command == MfUltralightPollerCommandReset) {
        ret = NfcaPollerCommandReset;
    } else if(command == MfUltralightPollerCommandStop) {
        ret = NfcaPollerCommandStop;
    } else {
        furi_crash("Unknown command");
    }

    return ret;
}

MfUltralightPoller* mf_ultralight_poller_alloc(NfcaPoller* nfca_poller) {
    MfUltralightPoller* instance = malloc(sizeof(MfUltralightPoller));
    instance->nfca_poller = nfca_poller;

    return instance;
}

void mf_ultralight_poller_free(MfUltralightPoller* instance) {
    furi_assert(instance);

    free(instance);
}

static MfUltralightPollerCommand mf_ultralight_poller_handler_idle(MfUltralightPoller* instance) {
    nfc_poller_buffer_reset(instance->buffer);
    nfca_poller_get_data(instance->nfca_poller, &instance->data->nfca_data);
    instance->state = MfUltralightPollerStateReadVersion;

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_version(MfUltralightPoller* instance) {
    MfUltralightError error = mf_ultralight_poller_async_read_version(instance);
    if(error == MfUltralightErrorNone) {
        FURI_LOG_I(TAG, "Read version success");
        instance->data->type = mf_ultralight_get_type_by_version(&instance->data->version);
        instance->state = MfUltralightPollerStateGetFeatureSet;
    } else {
        FURI_LOG_W(TAG, "Didn't response. Check NTAG 203");
        nfca_poller_halt(instance->nfca_poller);
        instance->state = MfUltralightPollerStateDetectNtag203;
    }

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_check_ntag_203(MfUltralightPoller* instance) {
    MfUltralightError error = mf_ultralight_poller_async_read_page(instance, 41);
    if(error == MfUltralightErrorNone) {
        FURI_LOG_I(TAG, "NTAG203 detected");
        instance->data->type = MfUltralightTypeNTAG203;
    } else {
        FURI_LOG_I(TAG, "Original Ultralight detected");
        nfca_poller_halt(instance->nfca_poller);
        instance->data->type = MfUltralightTypeUnknown;
    }
    instance->state = MfUltralightPollerStateGetFeatureSet;

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_get_feature_set(MfUltralightPoller* instance) {
    MfUltralightPollerCommand command = MfUltralightPollerCommandContinue;

    instance->feature_set = mf_ultralight_get_feature_support_set(instance->data->type);
    instance->pages_total = mf_ultralight_get_pages_total(instance->data->type);
    instance->pages_read = 0;
    instance->data->pages_total = instance->pages_total;
    FURI_LOG_I(
        TAG,
        "%s detected. Total pages: %d",
        mf_ultralight_get_name(instance->data->type, true),
        instance->pages_total);

    if(instance->feature_set & MfUltralightFeatureSupportAuthentication) {
        MfUltralightPollerEventData event_data = {};
        MfUltralightPollerEvent event = {
            .type = MfUltralightPollerEventTypeAuthRequest,
            .data = &event_data,
        };

        command = instance->callback(event, instance->context);
        if(!event.data->auth_context.skip_auth) {
            instance->auth_password = event.data->auth_context.password;
            instance->state = MfUltralightPollerStateAuth;
        } else {
            instance->state = MfUltralightPollerStateReadPages;
        }
    } else {
        instance->state = MfUltralightPollerStateReadPages;
    }

    return command;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_signature(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Reading signature");
    instance->state = MfUltralightPollerStateReadCounters;

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_counters(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Reading counters");
    instance->state = MfUltralightPollerStateReadTearingFlags;

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_traring_flags(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Reading tearing flags");
    instance->state = MfUltralightPollerStateReadTearingFlags;

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand mf_ultralight_poller_handler_auth(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Trying to authenticate with password %08lX", instance->auth_password.pass);
    MfUltralightPollerCommand command = MfUltralightPollerCommandContinue;

    MfUltralightError error = mf_ultralight_poller_async_auth(instance, &instance->auth_password);
    if(error == MfUltralightErrorNone) {
        FURI_LOG_D(TAG, "Auth success");
        // TODO fill PACK in event
        MfUltralightPollerEventData data = {.pack.pack = 0x8080};
        MfUltralightPollerEvent event = {
            .type = MfUltralightPollerEventTypeAuthSuccess, .data = &data};
        command = instance->callback(event, instance->context);
    } else {
        FURI_LOG_W(TAG, "Auth failed");
        MfUltralightPollerEvent event = {.type = MfUltralightPollerEventTypeAuthFailed};
        command = instance->callback(event, instance->context);
        // TODO rework with HALT cmd
        nfca_poller_halt(instance->nfca_poller);
    }
    instance->state = MfUltralightPollerStateReadPages;

    return command;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_pages(MfUltralightPoller* instance) {
    MfUltralightError error = mf_ultralight_poller_async_read_page(instance, instance->pages_read);
    if(error == MfUltralightErrorNone) {
        FURI_LOG_I(TAG, "Read page %d success", instance->pages_read);
        instance->pages_read++;
        instance->data->pages_read = instance->pages_read;
        if(instance->pages_read == instance->pages_total - 2) {
            instance->state = MfUltralightPollerStateReadSuccess;
        }
    } else {
        FURI_LOG_E(TAG, "Read page %d failed", instance->pages_read);
        if(instance->pages_read) {
            instance->state = MfUltralightPollerStateReadSuccess;
        } else {
            instance->state = MfUltralightPollerStateReadFailed;
        }
    }

    return MfUltralightPollerCommandContinue;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_fail(MfUltralightPoller* instance) {
    FURI_LOG_W(TAG, "Read Failed");
    MfUltralightPollerEvent event = {.type = MfUltralightPollerEventTypeReadFailed};
    MfUltralightPollerCommand command = instance->callback(event, instance->context);
    instance->state = MfUltralightPollerStateIdle;
    return command;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_handler_read_success(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Read success.");
    MfUltralightPollerEvent event = {.type = MfUltralightPollerEventTypeReadSuccess};
    MfUltralightPollerCommand command = instance->callback(event, instance->context);
    return command;
}

static const MfUltralightPollerReadHandler
    mf_ultralight_poller_read_handler[MfUltralightPollerStateNum] = {
        [MfUltralightPollerStateIdle] = mf_ultralight_poller_handler_idle,
        [MfUltralightPollerStateReadVersion] = mf_ultralight_poller_handler_read_version,
        [MfUltralightPollerStateDetectNtag203] = mf_ultralight_poller_handler_check_ntag_203,
        [MfUltralightPollerStateGetFeatureSet] = mf_ultralight_poller_handler_get_feature_set,
        [MfUltralightPollerStateReadSignature] = mf_ultralight_poller_handler_read_signature,
        [MfUltralightPollerStateReadCounters] = mf_ultralight_poller_handler_read_counters,
        [MfUltralightPollerStateReadTearingFlags] =
            mf_ultralight_poller_handler_read_traring_flags,
        [MfUltralightPollerStateAuth] = mf_ultralight_poller_handler_auth,
        [MfUltralightPollerStateReadPages] = mf_ultralight_poller_handler_read_pages,
        [MfUltralightPollerStateReadFailed] = mf_ultralight_poller_handler_read_fail,
        [MfUltralightPollerStateReadSuccess] = mf_ultralight_poller_handler_read_success,

};

static NfcaPollerCommand mf_ultralight_poller_read_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPoller* instance = context;
    MfUltralightPollerEventData event_data = {};
    MfUltralightPollerEvent mf_ul_poller_event = {.data = &event_data};
    MfUltralightPollerCommand command = MfUltralightPollerCommandContinue;

    furi_assert(instance->session_state != MfUltralightPollerSessionStateIdle);
    if(instance->session_state == MfUltralightPollerSessionStateStopRequest) {
        command = MfUltralightPollerCommandStop;
    } else {
        if(event.type == NfcaPollerEventTypeReady) {
            command = mf_ultralight_poller_read_handler[instance->state](instance);
        } else if(event.type == NfcaPollerEventTypeError) {
            if(instance->callback) {
                mf_ul_poller_event.type = MfUltralightPollerEventTypeReadFailed;
                command = instance->callback(mf_ul_poller_event, instance->context);
            }
        }
    }

    return mf_ultralight_process_command(command);
}

MfUltralightError mf_ultralight_poller_start(
    MfUltralightPoller* instance,
    NfcaPollerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfUltralightPollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);
    furi_assert(instance->session_state == MfUltralightPollerSessionStateIdle);

    instance->data = malloc(sizeof(MfUltralightData));
    instance->buffer =
        nfc_poller_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE, MF_ULTRALIGHT_MAX_BUFF_SIZE);

    instance->session_state = MfUltralightPollerSessionStateActive;
    nfca_poller_start(instance->nfca_poller, callback, context);

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_read(
    MfUltralightPoller* instance,
    MfUltralightPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfUltralightPollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    return mf_ultralight_poller_start(instance, mf_ultralight_poller_read_callback, instance);
}

MfUltralightError
    mf_ultralight_poller_get_data(MfUltralightPoller* instance, MfUltralightData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_reset(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->buffer);
    furi_assert(instance->nfca_poller);

    nfc_poller_buffer_free(instance->buffer);
    instance->callback = NULL;
    instance->context = NULL;
    free(instance->data);
    instance->state = MfUltralightPollerStateIdle;

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_stop(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfca_poller);

    instance->session_state = MfUltralightPollerSessionStateStopRequest;
    nfca_poller_stop(instance->nfca_poller);
    instance->session_state = MfUltralightPollerSessionStateIdle;

    return mf_ultralight_poller_reset(instance);
}
