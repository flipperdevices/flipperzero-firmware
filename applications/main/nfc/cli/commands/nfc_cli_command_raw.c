#include "nfc_cli_command_raw.h"
#include "../nfc_cli_command_processor.h"
#include <toolbox/args.h>
#include <nfc/nfc_poller.h>
#include <nfc/helpers/iso14443_crc.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <furi_hal_nfc.h>

///TODO: use this in parsing of arg_protocol
/* const char* protocol_key_valid_values[] =
    {"14a", "14b", "15", "felica", "iso14a", "iso14b", "iso15"}; */

#define NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE (256)

#define ISO14443_3A_FDT_LISTEN_FC (1172)

typedef enum {
    NfcCliProtocolRequestTypeNormalExecute,
    //NfcCliProtocolRequestTypeFrameExchange,
    NfcCliProtocolRequestTypeAbort,
} NfcCliProtocolRequestType;

/* typedef struct {
    Nfc* nfc;
    NfcGenericInstance* poller;
    NfcCliPollerError error;
    union {
        NfcCliProtocolRequestFrameExchangeData frame_exchange;
        FuriString* activation_info;
    };
} NfcCliProtocolRequestData; */

/* typedef struct {
    bool select;
    bool keep_field;
    bool append_crc;
    const uint8_t* data;
    size_t data_length;
} NfcCliProtocolRequestData; */

typedef enum {
    NfcPollerStateStopped,
    NfcPollerStateStarted,
} NfcPollerState;

typedef struct {
    Nfc* nfc;
    bool select;
    bool keep_field;
    bool append_crc;
    NfcProtocol protocol;

    BitBuffer* rx_buffer;
    BitBuffer* tx_buffer;

    NfcPoller* poller;
    NfcPollerState poller_state;

    NfcCliProtocolRequestType request_type;
    FuriMessageQueue* input_queue;
    FuriSemaphore* sem_done;

    FuriString* result_str;
} NfcCliRawCmdContext;

/* static void nfc_cli_poller_context_free(NfcCliPollerContext* instance) {
    furi_assert(instance);

    furi_string_free(instance->formatted_data);
    bit_buffer_free(instance->rx_buffer);
    furi_message_queue_free(instance->worker_queue);
    furi_message_queue_free(instance->user_queue);
    nfc_poller_free(instance->poller);
}
 */
static NfcCliActionContext* nfc_cli_raw_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliRawCmdContext* instance = malloc(sizeof(NfcCliRawCmdContext));
    instance->nfc = nfc;

    instance->protocol = NfcProtocolInvalid;

    //instance->poller = nfc_poller_alloc(instance->nfc, protocol);
    // instance->user_queue = furi_message_queue_alloc(8, sizeof(NfcCliUserMessage));
    // instance->worker_queue = furi_message_queue_alloc(8, sizeof(NfcCliWorkerMessage));
    instance->rx_buffer = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE);
    instance->tx_buffer = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE);

    instance->input_queue = furi_message_queue_alloc(5, sizeof(NfcCliProtocolRequestType));
    instance->sem_done = furi_semaphore_alloc(1, 0);
    instance->result_str = furi_string_alloc();
    return instance;
}

static void nfc_cli_raw_abort_nfc_thread(NfcCliRawCmdContext* instance) {
    if(instance->poller_state == NfcPollerStateStarted) {
        instance->request_type = NfcCliProtocolRequestTypeAbort;
        furi_message_queue_put(instance->input_queue, &instance->request_type, FuriWaitForever);
        furi_semaphore_acquire(instance->sem_done, FuriWaitForever);
        instance->poller_state = NfcPollerStateStopped;
    }
    if(instance->poller) nfc_poller_stop(instance->poller);
}

static void nfc_cli_raw_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    NfcCliRawCmdContext* instance = ctx;

    nfc_cli_raw_abort_nfc_thread(instance);
    if(instance->poller) nfc_poller_free(instance->poller);

    furi_message_queue_free(instance->input_queue);
    furi_semaphore_free(instance->sem_done);

    furi_string_free(instance->result_str);
    bit_buffer_free(instance->rx_buffer);
    bit_buffer_free(instance->tx_buffer);
    instance->nfc = NULL;
    free(instance);
}

static bool nfc_cli_raw_can_reuse_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    NfcCliRawCmdContext* instance = ctx;
    bool result = instance->keep_field;
    instance->keep_field = false;
    instance->append_crc = false;
    instance->select = false;
    return result;
}

static NfcCommand nfc_cli_raw_poller_callback(NfcGenericEventEx event, void* context) {
    NfcEvent* nfc_event = event.parent_event_data;
    NfcCliRawCmdContext* instance = context;

    Iso14443_3aPoller* poller = event.poller;

    NfcCommand command = NfcCommandContinue;
    NfcCliProtocolRequestType request_type = NfcCliProtocolRequestTypeAbort;

    if(nfc_event->type == NfcEventTypePollerReady) {
        FURI_LOG_D("RAW", "Poller callback");
        furi_message_queue_get(instance->input_queue, &request_type, FuriWaitForever);

        if(request_type == NfcCliProtocolRequestTypeAbort) {
            command = NfcCommandStop;
        } else {
            if(instance->select) {
                Iso14443_3aData iso3_data = {};
                FURI_LOG_D("RAW", "Activating...");

                Iso14443_3aError error = iso14443_3a_poller_activate(poller, &iso3_data);
                if(error == Iso14443_3aErrorNone) {
                    FURI_LOG_D("RAW", "Activate OK");

                    furi_string_printf(instance->result_str, "UID:");
                    for(size_t i = 0; i < iso3_data.uid_len; i++) {
                        furi_string_cat_printf(instance->result_str, " %02X", iso3_data.uid[i]);
                    }
                    furi_string_cat_printf(
                        instance->result_str,
                        " ATQA: %02X%02X SAK: %02X",
                        iso3_data.atqa[0],
                        iso3_data.atqa[1],
                        iso3_data.sak);
                }
            }

            if(bit_buffer_get_size_bytes(instance->tx_buffer) > 0) {
                FURI_LOG_D(TAG, "Tx");
                if(instance->append_crc) {
                    FURI_LOG_D("RAW", "Tx CRC");
                    iso14443_crc_append(Iso14443CrcTypeA, instance->tx_buffer);
                }

                bit_buffer_reset(instance->rx_buffer);

                NfcError error = nfc_poller_trx(
                    instance->nfc,
                    instance->tx_buffer,
                    instance->rx_buffer,
                    ISO14443_3A_FDT_LISTEN_FC);

                if(error == NfcErrorNone) {
                    FURI_LOG_D("RAW", "Tx OK");
                } else {
                    FURI_LOG_D("RAW", "Tx Error");
                }
            }
            command = instance->keep_field ? NfcCommandContinue : NfcCommandStop;
        }
    }
    furi_semaphore_release(instance->sem_done);
    if(command == NfcCommandStop) {
        FURI_LOG_D("RAW", "Aborting poller callback");
        instance->poller_state = NfcPollerStateStopped;
    }
    return command;
}

static void nfc_cli_raw_execute(PipeSide* pipe, void* context) {
    UNUSED(pipe);
    furi_assert(context);
    NfcCliRawCmdContext* instance = context;

    printf("Raw execute\r\n");
    printf(
        "Context:\r\nk = %d\r\nc = %d\r\ns = %d\r\n",
        instance->keep_field,
        instance->append_crc,
        instance->select);
    printf("Protocol: %d\r\n", instance->protocol);
    size_t data_length = bit_buffer_get_size_bytes(instance->tx_buffer);
    printf("Data length: %d\r\nData:", data_length);
    for(size_t i = 0; i < data_length; i++)
        printf("%02X ", bit_buffer_get_byte(instance->tx_buffer, i));

    furi_string_reset(instance->result_str);

    if(instance->poller_state == NfcPollerStateStopped) {
        if(instance->poller == NULL)
            instance->poller = nfc_poller_alloc(instance->nfc, instance->protocol);

        nfc_poller_start_ex(instance->poller, nfc_cli_raw_poller_callback, instance);
        instance->poller_state = NfcPollerStateStarted;
    }

    instance->request_type = NfcCliProtocolRequestTypeNormalExecute;
    furi_message_queue_put(instance->input_queue, &instance->request_type, FuriWaitForever);
    furi_semaphore_acquire(instance->sem_done, FuriWaitForever);

    printf("%s", furi_string_get_cstr(instance->result_str));
    size_t rx_size = bit_buffer_get_size_bytes(instance->rx_buffer);
    if(rx_size > 0) {
        printf("\r\nTx:");
        const uint8_t* tx_data = bit_buffer_get_data(instance->rx_buffer);
        for(size_t i = 0; i < data_length; i++) {
            printf(" %02X", tx_data[i]);
        }

        printf("\r\nRx:");
        const uint8_t* rx_data = bit_buffer_get_data(instance->rx_buffer);
        for(size_t i = 0; i < rx_size; i++) {
            printf(" %02X", rx_data[i]);
        }
    }
}

static bool nfc_cli_raw_parse_protocol(FuriString* value, void* output) {
    NfcCliRawCmdContext* ctx = output;
    NfcProtocol new_protocol = NfcProtocolInvalid;

    bool result = true;
    if(furi_string_equal_str(value, "14a") || furi_string_equal_str(value, "iso14a")) {
        new_protocol = NfcProtocolIso14443_3a;
    } else if(furi_string_equal_str(value, "14b") || furi_string_equal_str(value, "iso14b")) {
        new_protocol = NfcProtocolIso14443_3b;
    } else if(furi_string_equal_str(value, "15")) {
        new_protocol = NfcProtocolIso15693_3;
    } else if(furi_string_equal_str(value, "felica")) {
        new_protocol = NfcProtocolFelica;
    } else {
        result = false;
        new_protocol = NfcProtocolInvalid;
    }

    if(result && ctx->protocol != NfcProtocolInvalid && ctx->protocol != new_protocol) {
        printf("Unable to reuse");
        result = false;
    }

    if(result) {
        ctx->protocol = new_protocol;
    }
    return result;
}

static bool nfc_cli_raw_parse_data(FuriString* value, void* output) {
    NfcCliRawCmdContext* ctx = output;

    bool result = false;
    do {
        size_t len = furi_string_size(value);
        if(len % 2 != 0) break;

        size_t data_length = len / 2;
        uint8_t* data = malloc(data_length);

        if(args_read_hex_bytes(value, data, data_length)) {
            bit_buffer_reset(ctx->tx_buffer);
            bit_buffer_copy_bytes(ctx->tx_buffer, data, data_length);
            result = true;
        }

        free(data);
    } while(false);

    return result;
}

static bool nfc_cli_raw_parse_select(FuriString* value, void* output) {
    UNUSED(value);
    NfcCliRawCmdContext* ctx = output;
    ctx->select = true;
    return true;
}

static bool nfc_cli_raw_parse_crc(FuriString* value, void* output) {
    UNUSED(value);
    NfcCliRawCmdContext* ctx = output;
    ctx->append_crc = true;
    return true;
}

static bool nfc_cli_raw_parse_keep(FuriString* value, void* output) {
    UNUSED(value);
    NfcCliRawCmdContext* ctx = output;
    ctx->keep_field = true;
    return true;
}

const NfcCliKeyDescriptor raw_action_keys[] = {
    {
        .long_name = NULL,
        .short_name = "k",
        .description = "keep signal field ON after receive",
        .parse = nfc_cli_raw_parse_keep,
    },
    {
        .long_name = NULL,
        .short_name = "c",
        .description = "calculate and append CRC",
        .parse = nfc_cli_raw_parse_crc,
    },
    {
        .long_name = NULL,
        .short_name = "s",
        .description = "Select on FieldOn",
        .parse = nfc_cli_raw_parse_select,
    },
    {
        .long_name = "protocol",
        .short_name = "p",
        .features = {.parameter = true, .required = true},
        .parse = nfc_cli_raw_parse_protocol,
    },
    {
        .long_name = "data",
        .short_name = "d",
        .features = {.parameter = true, .required = true},
        .parse = nfc_cli_raw_parse_data,
    },
};

const NfcCliActionDescriptor raw_action = {
    .name = "raw",
    .key_count = COUNT_OF(raw_action_keys),
    .keys = raw_action_keys,
    .execute = nfc_cli_raw_execute,
    .alloc = nfc_cli_raw_alloc_ctx,
    .free = nfc_cli_raw_free_ctx,
    .can_reuse = nfc_cli_raw_can_reuse_ctx,
};

const NfcCliActionDescriptor* raw_actions_collection[] = {&raw_action};
//Command descriptor

ADD_NFC_CLI_COMMAND(raw, raw_actions_collection);

/* const NfcCliCommandDescriptor raw_cmd = {
    .name = "raw",
    .action_count = COUNT_OF(raw_actions_collection),
    .actions = raw_actions_collection,
    .callback = nfc_cli_command_raw_callback,
} */
;
/* 
static void nfc_cli_command_raw_callback(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    nfc_cli_command_process(&raw_cmd, args, context);
} */

//Command usage: raw <protocol> [keys] <data>
//Command examples:
//raw iso14a -sc 3000
//raw iso14a 3000
//raw iso14a 3000 -sc
