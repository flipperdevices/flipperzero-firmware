#include "mf_ultralight_poller_i.h"

#include <furi.h>

#define MF_ULTRALIGHT_POLLER_COMPLETE_EVENT (1UL << 0)

typedef struct {
    MfUltralightPoller* instance;
    FuriThreadId thread_id;
    MfUltralightError error;
    MfUltralightPollerContextData data;
} MfUltralightPollerContext;

static NfcaPollerCommand mf_ultraight_read_page_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_page(
            poller_context->instance,
            poller_context->data.read_cmd.start_page,
            &poller_context->data.read_cmd.data);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfUltralightError mf_ultralight_poller_read_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.read_cmd.start_page = page;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_page_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfUltralightErrorNone) {
        *data = poller_context.data.read_cmd.data.page[0];
    }
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}

static NfcaPollerCommand mf_ultraight_write_page_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_write_page(
            poller_context->instance,
            poller_context->data.write_cmd.page_to_write,
            &poller_context->data.write_cmd.page);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfUltralightError mf_ultralight_poller_write_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.write_cmd.page_to_write = page;
    poller_context.data.write_cmd.page = *data;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_write_page_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}

static NfcaPollerCommand mf_ultraight_read_version_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_version(
            poller_context->instance, &poller_context->data.version);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfUltralightError
    mf_ultralight_poller_read_version(MfUltralightPoller* instance, MfUltralightVersion* data) {
    furi_assert(instance);
    furi_assert(data);
    MfUltralightPollerContext poller_context = {};
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_version_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfUltralightErrorNone) {
        *data = poller_context.data.version;
    }
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}

static NfcaPollerCommand
    mf_ultraight_read_signature_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_signature(
            poller_context->instance, &poller_context->data.signature);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfUltralightError
    mf_ultralight_poller_read_signature(MfUltralightPoller* instance, MfUltralightSignature* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_signature_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfUltralightErrorNone) {
        *data = poller_context.data.signature;
    }
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}

static NfcaPollerCommand mf_ultraight_read_counter_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_counter(
            poller_context->instance,
            poller_context->data.counter_cmd.counter_num,
            &poller_context->data.counter_cmd.data);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfUltralightError mf_ultralight_poller_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.counter_cmd.counter_num = counter_num;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_counter_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfUltralightErrorNone) {
        *data = poller_context.data.counter_cmd.data;
    }
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}

static NfcaPollerCommand
    mf_ultraight_read_tering_flag_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_tearing_flag(
            poller_context->instance,
            poller_context->data.tearing_flag_cmd.tearing_flag_num,
            &poller_context->data.tearing_flag_cmd.data);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfUltralightError mf_ultralight_poller_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t flag_num,
    MfUltralightTearingFlag* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.tearing_flag_cmd.tearing_flag_num = flag_num;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_tering_flag_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfUltralightErrorNone) {
        *data = poller_context.data.tearing_flag_cmd.data;
    }
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}

static MfUltralightPollerCommand
    mf_ultralight_poller_read_callback(MfUltralightPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    MfUltralightPollerCommand command = MfUltralightPollerCommandContinue;

    if(event.type == MfUltralightPollerEventTypeReadSuccess) {
        mf_ultralight_poller_get_data(poller_context->instance, &poller_context->data.data);
        poller_context->error = MfUltralightErrorNone;
        command = MfUltralightPollerCommandStop;
    } else if(event.type == MfUltralightPollerEventTypeReadFailed) {
        poller_context->error = event.data->error;
        command = MfUltralightPollerCommandStop;
    } else if(event.type == MfUltralightPollerEventTypeAuthRequest) {
        event.data->auth_context.skip_auth = true;
    }

    if(command == MfUltralightPollerCommandStop) {
        furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
    }

    return command;
}

MfUltralightError
    mf_ultralight_poller_read_card(MfUltralightPoller* instance, MfUltralightData* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_read(instance, mf_ultralight_poller_read_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfUltralightErrorNone) {
        mf_ultralight_copy(data, &poller_context.data.data);
    }
    mf_ultralight_poller_stop(instance);

    return poller_context.error;
}
