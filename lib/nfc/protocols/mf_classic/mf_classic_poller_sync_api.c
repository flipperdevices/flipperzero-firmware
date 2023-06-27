#include "mf_classic_poller_i.h"

#include <nfc/nfc_poller.h>

#include <furi.h>

#define MF_CLASSIC_POLLER_COMPLETE_EVENT (1UL << 0)

typedef enum {
    MfClassicPollerCmdTypeAuth,
    MfClassicPollerCmdTypeReadBlock,

    MfClassicPollerCmdTypeNum,
} MfClassicPollerCmdType;

typedef struct {
    MfClassicPollerCmdType cmd_type;
    FuriThreadId thread_id;
    MfClassicError error;
    MfClassicPollerContextData data;
} MfClassicPollerContext;

typedef MfClassicError (
    *MfClassicPollerCmdHandler)(MfClassicPoller* poller, MfClassicPollerContextData* data);

static MfClassicError
    mf_classic_poller_auth_handler(MfClassicPoller* poller, MfClassicPollerContextData* data) {
    return mf_classic_async_auth(
        poller,
        data->auth_context.block_num,
        &data->auth_context.key,
        data->auth_context.key_type,
        &data->auth_context);
}

static MfClassicError mf_classic_poller_read_block_handler(
    MfClassicPoller* poller,
    MfClassicPollerContextData* data) {
    MfClassicError error = MfClassicErrorNone;

    error = mf_classic_async_auth(
        poller,
        data->read_block_context.block_num,
        &data->read_block_context.key,
        data->read_block_context.key_type,
        NULL);
    if(error == MfClassicErrorNone) {
        error = mf_classic_async_read_block(
            poller, data->read_block_context.block_num, &data->read_block_context.block);
    }

    return error;
}

static const MfClassicPollerCmdHandler mf_classic_poller_cmd_handlers[MfClassicPollerCmdTypeNum] = {
    [MfClassicPollerCmdTypeAuth] = mf_classic_poller_auth_handler,
    [MfClassicPollerCmdTypeReadBlock] = mf_classic_poller_read_block_handler,
};

static NfcCommand mf_ultralgiht_poller_cmd_callback(NfcPollerEvent event, void* context) {
    furi_assert(event.poller);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.data);
    furi_assert(context);

    MfClassicPollerContext* poller_context = context;
    Iso14443_3aPollerEvent* iso14443_3a_event = event.data;
    Iso14443_3aPoller* iso14443_3a_poller = event.poller;
    MfClassicPoller* mfc_poller = mf_classic_poller_alloc(iso14443_3a_poller);

    if(iso14443_3a_event->type == Iso14443_3aPollerEventTypeReady) {
        poller_context->error = mf_classic_poller_cmd_handlers[poller_context->cmd_type](
            mfc_poller, &poller_context->data);
    } else if(iso14443_3a_event->type == Iso14443_3aPollerEventTypeError) {
        poller_context->error = mf_classic_process_error(iso14443_3a_event->data->error);
    }

    furi_thread_flags_set(poller_context->thread_id, MF_CLASSIC_POLLER_COMPLETE_EVENT);

    mf_classic_poller_free(mfc_poller);

    return NfcCommandStop;
}

static MfClassicError mf_classic_poller_cmd_execute(Nfc* nfc, MfClassicPollerContext* poller_ctx) {
    furi_assert(poller_ctx->cmd_type < MfClassicPollerCmdTypeNum);

    poller_ctx->thread_id = furi_thread_get_current_id();

    NfcPoller* poller = nfc_poller_alloc(nfc, NfcProtocolIso14443_3a);
    nfc_poller_start(poller, mf_ultralgiht_poller_cmd_callback, poller_ctx);
    furi_thread_flags_wait(MF_CLASSIC_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(MF_CLASSIC_POLLER_COMPLETE_EVENT);

    nfc_poller_stop(poller);
    nfc_poller_free(poller);

    return poller_ctx->error;
}

MfClassicError mf_classic_poller_auth(
    Nfc* nfc,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data) {
    furi_assert(nfc);
    furi_assert(key);
    furi_assert(data);

    MfClassicPollerContext poller_context = {
        .cmd_type = MfClassicPollerCmdTypeAuth,
        .data.auth_context.block_num = block_num,
        .data.auth_context.key = *key,
        .data.auth_context.key_type = key_type,
    };

    MfClassicError error = mf_classic_poller_cmd_execute(nfc, &poller_context);

    if(error == MfClassicErrorNone) {
        *data = poller_context.data.auth_context;
    }

    return error;
}

MfClassicError mf_classic_poller_read_block(
    Nfc* nfc,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicBlock* data) {
    furi_assert(nfc);
    furi_assert(key);
    furi_assert(data);

    MfClassicPollerContext poller_context = {
        .cmd_type = MfClassicPollerCmdTypeReadBlock,
        .data.read_block_context.block_num = block_num,
        .data.read_block_context.key = *key,
        .data.read_block_context.key_type = key_type,
    };

    MfClassicError error = mf_classic_poller_cmd_execute(nfc, &poller_context);

    if(error == MfClassicErrorNone) {
        *data = poller_context.data.read_block_context.block;
    }

    return error;
}
