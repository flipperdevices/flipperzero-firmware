#include "mf_classic_poller_i.h"

#include <nfc/nfc_poller.h>

#include <furi.h>

#define TAG "MfClassicPollerLl"

#define MF_CLASSIC_POLLER_LL_COMPLETE_EVENT (1UL << 0)

typedef enum {
    MfClassicPollerCmdTypeLlAuth,
    MfClassicPollerCmdTypeLlReadBlock,
    MfClassicPollerCmdTypeLlWriteBlock,
    MfClassicPollerCmdTypeLlValueBlockCommand,
    MfClassicPollerCmdTypeLlValueBlockTransfer,
    MfClassicPollerCmdTypeLlHalt,

    MfClassicPollerCmdTypeLlNum,
} MfClassicPollerLlCmdType;

typedef struct {
    NfcPoller* poller;
    MfClassicPoller* mfc_poller;
    FuriMessageQueue* cmd_queue;
} MfClassicPollerLl;

typedef struct {
    MfClassicPollerLlCmdType cmd_type;
    MfClassicPollerContextData* data;
    MfClassicError* result;
    FuriThreadId thread_id;
} MfClassicPollerLlCmd;

typedef MfClassicError (
    *MfClassicPollerLlCmdHandler)(MfClassicPoller* poller, MfClassicPollerContextData* data);

static MfClassicError
    mf_classic_poller_ll_auth_handler(MfClassicPoller* poller, MfClassicPollerContextData* data) {
    return mf_classic_async_auth(
        poller,
        data->auth_context.block_num,
        &data->auth_context.key,
        data->auth_context.key_type,
        &data->auth_context,
        data->auth_context.is_nested);
}

static MfClassicError mf_classic_poller_ll_read_block_handler(
    MfClassicPoller* poller,
    MfClassicPollerContextData* data) {
    MfClassicError error = mf_classic_async_read_block(
        poller, data->read_block_context.block_num, &data->read_block_context.block);
    return error;
}

static MfClassicError mf_classic_poller_ll_write_block_handler(
    MfClassicPoller* poller,
    MfClassicPollerContextData* data) {
    MfClassicError error = mf_classic_async_write_block(
        poller, data->read_block_context.block_num, &data->read_block_context.block);
    return error;
}

static MfClassicError mf_classic_poller_ll_value_block_command_handler(
    MfClassicPoller* poller,
    MfClassicPollerContextData* data) {
    MfClassicError error = error = mf_classic_async_value_cmd(
        poller,
        data->change_value_context.block_num,
        data->change_value_context.value_cmd,
        data->change_value_context.data);
    return error;
}

static MfClassicError mf_classic_poller_ll_value_block_transfer_handler(
    MfClassicPoller* poller,
    MfClassicPollerContextData* data) {
    MfClassicError error =
        mf_classic_async_value_transfer(poller, data->transfer_value_context.block_num);
    return error;
}

static MfClassicError
    mf_classic_poller_ll_halt_handler(MfClassicPoller* poller, MfClassicPollerContextData* data) {
    UNUSED(data);
    MfClassicError error = mf_classic_async_halt(poller);
    return error;
}

static const MfClassicPollerLlCmdHandler
    mf_classic_poller_ll_cmd_handlers[MfClassicPollerCmdTypeLlNum] = {

        [MfClassicPollerCmdTypeLlAuth] = mf_classic_poller_ll_auth_handler,
        [MfClassicPollerCmdTypeLlReadBlock] = mf_classic_poller_ll_read_block_handler,
        [MfClassicPollerCmdTypeLlWriteBlock] = mf_classic_poller_ll_write_block_handler,
        [MfClassicPollerCmdTypeLlValueBlockCommand] =
            mf_classic_poller_ll_value_block_command_handler,
        [MfClassicPollerCmdTypeLlValueBlockTransfer] =
            mf_classic_poller_ll_value_block_transfer_handler,
        [MfClassicPollerCmdTypeLlHalt] = mf_classic_poller_ll_halt_handler,
};

static NfcCommand mf_classic_poller_ll_cmd_callback(NfcGenericEvent event, void* context) {
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.event_data);
    furi_assert(context);

    MfClassicPollerLl* poller = context;
    Iso14443_3aPollerEvent* iso14443_3a_event = event.event_data;
    Iso14443_3aPoller* iso14443_3a_poller = event.instance;

    if(poller->mfc_poller == NULL) {
        poller->mfc_poller = mf_classic_poller_alloc(iso14443_3a_poller);
    }

    MfClassicPollerLlCmd poller_cmd;
    furi_message_queue_get(poller->cmd_queue, &poller_cmd, FuriWaitForever);

    if(iso14443_3a_event->type == Iso14443_3aPollerEventTypeReady) {
        *poller_cmd.result = mf_classic_poller_ll_cmd_handlers[poller_cmd.cmd_type](
            poller->mfc_poller, poller_cmd.data);
    } else if(iso14443_3a_event->type == Iso14443_3aPollerEventTypeError) {
        *poller_cmd.result = mf_classic_process_error(iso14443_3a_event->data->error);
    }

    furi_thread_flags_set(poller_cmd.thread_id, MF_CLASSIC_POLLER_LL_COMPLETE_EVENT);

    return poller_cmd.cmd_type == MfClassicPollerCmdTypeLlHalt ? NfcCommandStop :
                                                                 NfcCommandContinue;
}

static MfClassicError mf_classic_poller_ll_cmd_execute(
    MfClassicPollerLl* poller,
    MfClassicPollerLlCmdType cmd_type,
    MfClassicPollerContextData* cmd_data) {
    furi_assert(cmd_type < MfClassicPollerCmdTypeLlNum);

    MfClassicError result;
    MfClassicPollerLlCmd cmd = {
        .cmd_type = cmd_type,
        .data = cmd_data,
        .result = &result,
        .thread_id = furi_thread_get_current_id()};

    furi_message_queue_put(poller->cmd_queue, &cmd, FuriWaitForever);

    furi_thread_flags_wait(MF_CLASSIC_POLLER_LL_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(MF_CLASSIC_POLLER_LL_COMPLETE_EVENT);

    return result;
}

MfClassicPollerLl* mf_classic_poller_ll_alloc(Nfc* nfc) {
    furi_assert(nfc);

    MfClassicPollerLl* poller = malloc(sizeof(MfClassicPollerLl));
    poller->poller = nfc_poller_alloc(nfc, NfcProtocolIso14443_3a);
    poller->cmd_queue = furi_message_queue_alloc(1, sizeof(MfClassicPollerLlCmd));
    poller->mfc_poller = NULL;
    nfc_poller_start(poller->poller, mf_classic_poller_ll_cmd_callback, poller);
    return poller;
}

void mf_classic_poller_ll_free(MfClassicPollerLl* poller) {
    furi_assert(poller);

    nfc_poller_stop(poller->poller);
    if(poller->mfc_poller != NULL) {
        mf_classic_poller_free(poller->mfc_poller);
    }

    nfc_poller_free(poller->poller);
    free(poller);
}

MfClassicError mf_classic_poller_ll_auth(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    bool is_nested) {
    furi_assert(poller);
    furi_assert(key);
    MfClassicPollerContextData cmd_data = {
        .auth_context.block_num = block_num,
        .auth_context.key = *key,
        .auth_context.key_type = key_type,
        .auth_context.is_nested = is_nested,
    };

    MfClassicError error =
        mf_classic_poller_ll_cmd_execute(poller, MfClassicPollerCmdTypeLlAuth, &cmd_data);

    return error;
}

MfClassicError mf_classic_poller_ll_read_block(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    MfClassicBlock* data) {
    furi_assert(poller);
    furi_assert(data);

    MfClassicPollerContextData cmd_data = {
        .read_block_context.block_num = block_num,
    };

    MfClassicError error =
        mf_classic_poller_ll_cmd_execute(poller, MfClassicPollerCmdTypeLlReadBlock, &cmd_data);

    if(error == MfClassicErrorNone) {
        *data = cmd_data.read_block_context.block;
    }

    return error;
}

MfClassicError mf_classic_poller_ll_write_block(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    MfClassicBlock* data) {
    furi_assert(poller);
    furi_assert(data);

    MfClassicPollerContextData cmd_data = {
        .write_block_context.block_num = block_num,
        .write_block_context.block = *data,
    };

    MfClassicError error =
        mf_classic_poller_ll_cmd_execute(poller, MfClassicPollerCmdTypeLlWriteBlock, &cmd_data);

    return error;
}

MfClassicError mf_classic_poller_ll_value_block_command(
    MfClassicPollerLl* poller,
    uint8_t block_num,
    uint8_t operation,
    uint32_t data) {
    furi_assert(poller);

    MfClassicPollerContextData cmd_data = {
        .change_value_context.block_num = block_num,
        .change_value_context.value_cmd = operation,
        .change_value_context.data = data,
    };

    MfClassicError error = mf_classic_poller_ll_cmd_execute(
        poller, MfClassicPollerCmdTypeLlValueBlockCommand, &cmd_data);

    return error;
}

MfClassicError
    mf_classic_poller_ll_value_block_transfer(MfClassicPollerLl* poller, uint8_t block_num) {
    furi_assert(poller);

    MfClassicPollerContextData cmd_data = {
        .transfer_value_context.block_num = block_num,
    };

    MfClassicError error = mf_classic_poller_ll_cmd_execute(
        poller, MfClassicPollerCmdTypeLlValueBlockTransfer, &cmd_data);

    return error;
}

MfClassicError mf_classic_poller_ll_halt(MfClassicPollerLl* poller) {
    furi_assert(poller);

    MfClassicPollerContextData cmd_data = {};

    MfClassicError error =
        mf_classic_poller_ll_cmd_execute(poller, MfClassicPollerCmdTypeLlHalt, &cmd_data);

    return error;
}