#include "nfc.h"
#include "st25tb_poller_i.h"

#include <nfc/nfc_poller.h>

#include <furi.h>

#define ST25TB_POLLER_FLAG_COMMAND_COMPLETE (1UL << 0)

typedef enum {
    St25tbPollerCmdTypeDetectType,
    St25tbPollerCmdTypeRead,
    St25tbPollerCmdTypeReadBlock,

    St25tbPollerCmdTypeNum,
} St25tbPollerCmdType;

typedef union {
    struct {
        St25tbType* type;
    } detect_type;
    struct {
        St25tbData* data;
    } read;
    struct {
        uint8_t block_num;
        uint32_t* block;
    } read_block;
} St25tbPollerCmdData;

typedef struct {
    FuriThreadId thread_id;
    St25tbError error;
    St25tbPollerCmdType cmd_type;
    St25tbPollerCmdData cmd_data;
} St25tbPollerContext;

typedef St25tbError (*St25tbPollerCmdHandler)(St25tbPoller* poller, St25tbPollerCmdData* data);

static St25tbError st25tb_poller_detect_handler(St25tbPoller* poller, St25tbPollerCmdData* data) {
    uint8_t uid[ST25TB_UID_SIZE];
    St25tbError error = st25tb_poller_async_get_uid(poller, uid);
    if(error == St25tbErrorNone) {
        *data->detect_type.type = st25tb_get_type_from_uid(uid);
    }
    return error;
}

static St25tbError st25tb_poller_read_handler(St25tbPoller* poller, St25tbPollerCmdData* data) {
    return st25tb_poller_async_read(poller, data->read.data);
}

static St25tbError
    st25tb_poller_read_block_handler(St25tbPoller* poller, St25tbPollerCmdData* data) {
    return st25tb_poller_async_read_block(
        poller, data->read_block.block, data->read_block.block_num);
}

static St25tbPollerCmdHandler st25tb_poller_cmd_handlers[St25tbPollerCmdTypeNum] = {
    [St25tbPollerCmdTypeDetectType] = st25tb_poller_detect_handler,
    [St25tbPollerCmdTypeRead] = st25tb_poller_read_handler,
    [St25tbPollerCmdTypeReadBlock] = st25tb_poller_read_block_handler,
};

static NfcCommand st25tb_poller_cmd_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolSt25tb);

    St25tbPollerContext* poller_context = context;
    St25tbPoller* st25tb_poller = event.instance;
    St25tbPollerEvent* st25tb_event = event.event_data;

    if(st25tb_event->type == St25tbPollerEventTypeReady) {
        poller_context->error = st25tb_poller_cmd_handlers[poller_context->cmd_type](
            st25tb_poller, &poller_context->cmd_data);
    } else {
        poller_context->error = st25tb_event->data->error;
    }

    furi_thread_flags_set(poller_context->thread_id, ST25TB_POLLER_FLAG_COMMAND_COMPLETE);

    return NfcCommandStop;
}

static St25tbError st25tb_poller_cmd_execute(Nfc* nfc, St25tbPollerContext* poller_ctx) {
    furi_assert(poller_ctx->cmd_type < St25tbPollerCmdTypeNum);
    poller_ctx->thread_id = furi_thread_get_current_id();

    NfcPoller* poller = nfc_poller_alloc(nfc, NfcProtocolSt25tb);
    nfc_poller_start(poller, st25tb_poller_cmd_callback, poller_ctx);
    furi_thread_flags_wait(ST25TB_POLLER_FLAG_COMMAND_COMPLETE, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(ST25TB_POLLER_FLAG_COMMAND_COMPLETE);

    nfc_poller_stop(poller);
    nfc_poller_free(poller);

    return poller_ctx->error;
}

St25tbError st25tb_poller_read_block(Nfc* nfc, uint8_t block_num, uint32_t* block) {
    St25tbPollerContext poller_context = {
        .cmd_type = St25tbPollerCmdTypeReadBlock,
        .cmd_data = {
            .read_block = {
                .block = block,
                .block_num = block_num,
            }}};
    return st25tb_poller_cmd_execute(nfc, &poller_context);
}

St25tbError st25tb_poller_detect_type(Nfc* nfc, St25tbType* type) {
    St25tbPollerContext poller_context = {
        .cmd_type = St25tbPollerCmdTypeDetectType, .cmd_data = {.detect_type = {.type = type}}};
    return st25tb_poller_cmd_execute(nfc, &poller_context);
}

St25tbError st25tb_poller_read(Nfc* nfc, St25tbData* data) {
    St25tbPollerContext poller_context = {
        .cmd_type = St25tbPollerCmdTypeRead, .cmd_data = {.read = {.data = data}}};
    return st25tb_poller_cmd_execute(nfc, &poller_context);
}