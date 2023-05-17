#include "mf_classic_poller_i.h"

#include <furi.h>

#define MF_CLASSIC_POLLER_COMPLETE_EVENT (1UL << 0)

typedef struct {
    MfClassicPoller* instance;
    FuriThreadId thread_id;
    MfClassicError error;
    MfClassicPollerContextData data;
} MfClassicPollerContext;

NfcaPollerCommand mf_classic_auth_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfClassicPollerContext* poller_context = context;
    MfClassicAuthContext* auth_context = &poller_context->data.auth_context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_classic_async_auth(
            poller_context->instance,
            auth_context->block_num,
            &auth_context->key,
            auth_context->key_type,
            auth_context);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_classic_process_error(event.data.error);
    }
    furi_thread_flags_set(poller_context->thread_id, MF_CLASSIC_POLLER_COMPLETE_EVENT);

    return NfcaPollerCommandStop;
}

MfClassicError mf_classic_poller_auth(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data) {
    furi_assert(instance);
    furi_assert(key);
    furi_assert(data);

    MfClassicPollerContext poller_context = {};
    poller_context.data.auth_context.block_num = block_num;
    poller_context.data.auth_context.key = *key;
    poller_context.data.auth_context.key_type = key_type;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_classic_poller_start(instance, mf_classic_auth_callback, &poller_context);
    furi_thread_flags_wait(MF_CLASSIC_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);

    if(poller_context.error == MfClassicErrorNone) {
        *data = poller_context.data.auth_context;
    }
    mf_classic_poller_stop(instance);

    return poller_context.error;
}

MfClassicError mf_classic_poller_read_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicBlock* data) {
    furi_assert(instance);
    furi_assert(key);
    furi_assert(data);
    UNUSED(block_num);
    UNUSED(key_type);

    if(data) {
        for(size_t i = 0; i < 16; i++) {
            data->data[i] = i;
        }
    }

    return MfClassicErrorNone;
}