#include "ask_ctx_poller.h"
#include "ask_ctx_i.h"
#include "ask_ctx_poller_defs.h"

#include <furi.h>
#include <nfc/helpers/iso14443_crc.h>
#include <nfc/nfc_poller.h>

#define ASK_CTX_CMD_REQT           (0x10U)
#define ASK_CTX_CMD_SELECT_ALL     (0x9FU)
#define ASK_CTX_CMD_AUTHENTICATE   (0x40U)
#define ASK_CTX_BLOCK_ADDRESS_MASK (0x1FU)
#define ASK_CTX_UID_LSB_BLOCK      (3U)
#define ASK_CTX_UID_MSB_BLOCK      (4U)

#define ASK_CTX_AUTHENTICATION_ATTEMPTS (3U)

#define ASK_CTX_GUARD_TIME_US      (5000U)
#define ASK_CTX_FDT_POLL_FC        (9000U)
#define ASK_CTX_FWT_FC             (60000U)
#define ASK_CTX_POLL_POLL_MIN_US   (1280U)
#define ASK_CTX_POLLER_BUFFER_SIZE (16U)

typedef enum {
    AskCtxPollerStateSelect,
    AskCtxPollerStateDetectType,
    AskCtxPollerStateRead,
    AskCtxPollerStateReadSuccess,
    AskCtxPollerStateReadFailed,

    AskCtxPollerStateNum,
} AskCtxPollerState;

struct AskCtxPoller {
    Nfc* nfc;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    AskCtxData* data;
    AskCtxPollerState state;
    uint8_t current_block;
    uint8_t authentication_attempts;

    NfcGenericCallback callback;
    void* context;
    NfcGenericEvent general_event;
    AskCtxPollerEvent ask_ctx_event;
    AskCtxPollerEventData ask_ctx_event_data;
};

static AskCtxError ask_ctx_poller_exchange(
    AskCtxPoller* instance,
    const uint8_t* command,
    size_t command_size,
    uint8_t response[ASK_CTX_BLOCK_SIZE]) {
    bit_buffer_copy_bytes(instance->tx_buffer, command, command_size);
    iso14443_crc_append(Iso14443CrcTypeB, instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    const NfcError error =
        nfc_poller_trx(instance->nfc, instance->tx_buffer, instance->rx_buffer, ASK_CTX_FWT_FC);
    if(error != NfcErrorNone) {
        return error == NfcErrorTimeout ? AskCtxErrorTimeout : AskCtxErrorNotPresent;
    }
    const size_t rx_size = bit_buffer_get_size_bytes(instance->rx_buffer);
    if(rx_size != ASK_CTX_BLOCK_SIZE + ISO14443_CRC_SIZE) {
        return AskCtxErrorCommunication;
    }
    if(!iso14443_crc_check(Iso14443CrcTypeB, instance->rx_buffer)) {
        return AskCtxErrorWrongCrc;
    }

    memcpy(response, bit_buffer_get_data(instance->rx_buffer), ASK_CTX_BLOCK_SIZE);
    return AskCtxErrorNone;
}

static AskCtxError ask_ctx_poller_read_block(
    AskCtxPoller* instance,
    AskCtxType type,
    uint8_t block,
    uint8_t response[ASK_CTX_BLOCK_SIZE]) {
    furi_assert(type < AskCtxTypeNum);
    const uint8_t command = ask_ctx_features[type].read_command |
                            (block & ASK_CTX_BLOCK_ADDRESS_MASK);
    return ask_ctx_poller_exchange(instance, &command, sizeof(command), response);
}

static AskCtxError ask_ctx_poller_activate(AskCtxPoller* instance, AskCtxData* data) {
    const uint8_t reqt[] = {ASK_CTX_CMD_REQT};
    const uint8_t select_all[] = {ASK_CTX_CMD_SELECT_ALL, 0xFFU, 0xFFU};
    uint8_t response[ASK_CTX_BLOCK_SIZE];
    AskCtxData result = {};

    AskCtxError error = ask_ctx_poller_exchange(instance, reqt, sizeof(reqt), response);
    if(error == AskCtxErrorNone) {
        result.product_code = response[0];
        result.fab_code = response[1];
        // CTx256 and CTx512 generations use different activation sequences and READ commands.
        if((result.product_code & ASK_CTX_PRODUCT_MASK) == ASK_CTX_PRODUCT_CTS256B) {
            result.type = AskCtxTypeCts256B;
            error = ask_ctx_poller_read_block(
                instance, result.type, ASK_CTX_UID_LSB_BLOCK, result.uid);
        } else {
            if((result.product_code & ASK_CTX_PRODUCT_MASK) == ASK_CTX_PRODUCT_CTX512B) {
                result.type = AskCtxTypeCts512B;
            }
            error = ask_ctx_poller_exchange(instance, select_all, sizeof(select_all), result.uid);
        }
    }
    if(error == AskCtxErrorNone) {
        error = ask_ctx_poller_read_block(
            instance, result.type, ASK_CTX_UID_MSB_BLOCK, &result.uid[ASK_CTX_BLOCK_SIZE]);
    }
    if(error == AskCtxErrorNone) {
        *data = result;
    }

    return error;
}

static const AskCtxData* ask_ctx_poller_get_data(AskCtxPoller* instance) {
    furi_assert(instance);
    return instance->data;
}

static AskCtxPoller* ask_ctx_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    AskCtxPoller* instance = malloc(sizeof(AskCtxPoller));
    instance->nfc = nfc;
    instance->tx_buffer = bit_buffer_alloc(ASK_CTX_POLLER_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(ASK_CTX_POLLER_BUFFER_SIZE);
    instance->data = malloc(sizeof(AskCtxData));
    instance->state = AskCtxPollerStateSelect;

    nfc_config(instance->nfc, NfcModePoller, NfcTechAskCtx);
    nfc_set_guard_time_us(instance->nfc, ASK_CTX_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, ASK_CTX_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, ASK_CTX_POLL_POLL_MIN_US);

    instance->ask_ctx_event.data = &instance->ask_ctx_event_data;
    instance->general_event.protocol = NfcProtocolAskCtx;
    instance->general_event.event_data = &instance->ask_ctx_event;
    instance->general_event.instance = instance;

    return instance;
}

static void ask_ctx_poller_free(AskCtxPoller* instance) {
    furi_assert(instance);
    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    free(instance->data);
    free(instance);
}

static void ask_ctx_poller_set_callback(
    AskCtxPoller* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

typedef NfcCommand (*AskCtxPollerStateHandler)(AskCtxPoller* instance);

static NfcCommand ask_ctx_poller_select_handler(AskCtxPoller* instance) {
    const AskCtxError error = ask_ctx_poller_activate(instance, instance->data);
    if(error == AskCtxErrorNone) {
        instance->data->block_count = ask_ctx_features[instance->data->type].blocks_total;
        instance->current_block = 0;
        instance->authentication_attempts = 0;
        instance->state = (instance->data->product_code & ASK_CTX_PRODUCT_MASK) ==
                                  ASK_CTX_PRODUCT_CTX512B ?
                              AskCtxPollerStateDetectType :
                              AskCtxPollerStateRead;
    } else {
        instance->ask_ctx_event_data.error = error;
        instance->state = AskCtxPollerStateReadFailed;
    }

    return NfcCommandContinue;
}

static NfcCommand ask_ctx_poller_detect_type_handler(AskCtxPoller* instance) {
    uint8_t response[ASK_CTX_BLOCK_SIZE];
    AskCtxError error;
    if(instance->authentication_attempts < ASK_CTX_AUTHENTICATION_ATTEMPTS) {
        // Probe AUTHENTICATE at block 0 with a six-byte challenge; no signature verification.
        const uint8_t command[] = {ASK_CTX_CMD_AUTHENTICATE, 0, 0, 0, 0, 0, 0};
        error = ask_ctx_poller_exchange(instance, command, sizeof(command), response);
        instance->authentication_attempts++;
        if(error == AskCtxErrorTimeout) return NfcCommandContinue;
        if(error == AskCtxErrorNone) instance->data->type = AskCtxTypeCtm512B;
    } else {
        // Check that the tag is still present after authentication timeouts.
        error = ask_ctx_poller_read_block(instance, instance->data->type, 0, response);
        if(error == AskCtxErrorNone && (response[0] != instance->data->product_code ||
                                        response[1] != instance->data->fab_code)) {
            error = AskCtxErrorCommunication;
        }
    }
    if(error == AskCtxErrorNone) {
        instance->data->block_count = ask_ctx_features[instance->data->type].blocks_total;
        instance->state = AskCtxPollerStateRead;
    } else {
        instance->ask_ctx_event_data.error = error;
        instance->state = AskCtxPollerStateReadFailed;
    }

    return NfcCommandContinue;
}

static NfcCommand ask_ctx_poller_read_handler(AskCtxPoller* instance) {
    const uint8_t block = instance->current_block;
    if(block == instance->data->block_count) {
        instance->state = AskCtxPollerStateReadSuccess;
        return NfcCommandContinue;
    }

    const AskCtxError error = ask_ctx_poller_read_block(
        instance, instance->data->type, block, instance->data->blocks[block]);
    if(error == AskCtxErrorNone) {
        instance->current_block++;
    } else {
        instance->ask_ctx_event_data.error = error;
        instance->state = AskCtxPollerStateReadFailed;
    }

    return NfcCommandContinue;
}

static NfcCommand ask_ctx_poller_read_success_handler(AskCtxPoller* instance) {
    instance->ask_ctx_event.type = AskCtxPollerEventTypeReadSuccess;
    const NfcCommand command = instance->callback(instance->general_event, instance->context);
    instance->state = AskCtxPollerStateSelect;

    return command;
}

static NfcCommand ask_ctx_poller_read_failed_handler(AskCtxPoller* instance) {
    instance->ask_ctx_event.type = AskCtxPollerEventTypeReadFailed;
    const NfcCommand command = instance->callback(instance->general_event, instance->context);
    furi_delay_ms(100);
    instance->state = AskCtxPollerStateSelect;

    return command;
}

static const AskCtxPollerStateHandler ask_ctx_poller_state_handlers[AskCtxPollerStateNum] = {
    [AskCtxPollerStateSelect] = ask_ctx_poller_select_handler,
    [AskCtxPollerStateDetectType] = ask_ctx_poller_detect_type_handler,
    [AskCtxPollerStateRead] = ask_ctx_poller_read_handler,
    [AskCtxPollerStateReadSuccess] = ask_ctx_poller_read_success_handler,
    [AskCtxPollerStateReadFailed] = ask_ctx_poller_read_failed_handler,
};

static NfcCommand ask_ctx_poller_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    AskCtxPoller* instance = context;
    const NfcEvent* nfc_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    furi_assert(instance->state < AskCtxPollerStateNum);

    if(nfc_event->type == NfcEventTypePollerReady) {
        command = ask_ctx_poller_state_handlers[instance->state](instance);
    }

    return command;
}

static bool ask_ctx_poller_detect(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);

    const NfcEvent* nfc_event = event.event_data;
    if(nfc_event->type != NfcEventTypePollerReady) return false;

    AskCtxData data;
    return ask_ctx_poller_activate(context, &data) == AskCtxErrorNone;
}

const NfcPollerBase nfc_poller_ask_ctx = {
    .alloc = (NfcPollerAlloc)ask_ctx_poller_alloc,
    .free = (NfcPollerFree)ask_ctx_poller_free,
    .set_callback = (NfcPollerSetCallback)ask_ctx_poller_set_callback,
    .run = (NfcPollerRun)ask_ctx_poller_run,
    .detect = (NfcPollerDetect)ask_ctx_poller_detect,
    .get_data = (NfcPollerGetData)ask_ctx_poller_get_data,
};
