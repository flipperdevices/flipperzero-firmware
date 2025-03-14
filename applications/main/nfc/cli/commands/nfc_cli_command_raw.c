#include "nfc_cli_command_raw.h"
#include "../nfc_cli_command_processor.h"
#include <toolbox/args.h>

///TODO: use this in parsing of arg_protocol
/* const char* protocol_key_valid_values[] =
    {"14a", "14b", "15", "felica", "iso14a", "iso14b", "iso15"}; */
typedef struct {
    Nfc* nfc;
    bool select;
    bool keep_field;
    bool append_crc;
    NfcProtocol protocol;
    uint8_t* data;
    size_t data_length;
} NfcCliRawCmdContext;

static NfcCliActionContext* nfc_cli_raw_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliRawCmdContext* instance = malloc(sizeof(NfcCliRawCmdContext));
    instance->nfc = nfc;
    instance->protocol = NfcProtocolInvalid;
    return instance;
}

static void nfc_cli_raw_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    NfcCliRawCmdContext* instance = ctx;
    instance->nfc = NULL;
    free(instance);
}

static void nfc_cli_raw_execute(PipeSide* pipe, void* context) {
    UNUSED(pipe);
    NfcCliRawCmdContext* instance = context;
    UNUSED(instance);
    printf("Raw execute\r\n");
    printf(
        "Context:\r\nk = %d\r\nc = %d\r\ns = %d\r\n",
        instance->keep_field,
        instance->append_crc,
        instance->select);
    printf("Protocol: %d\r\n", instance->protocol);
    printf("Data length: %d\r\nData:", instance->data_length);
    for(size_t i = 0; i < instance->data_length; i++)
        printf("%02X ", instance->data[i]);
}

static bool nfc_cli_raw_parse_protocol(FuriString* value, void* output) {
    bool result = false;
    do {
        NfcCliRawCmdContext* ctx = output;
        result = true;
        if(furi_string_equal_str(value, "14a") || furi_string_equal_str(value, "iso14a")) {
            ctx->protocol = NfcProtocolIso14443_3a;
        } else if(furi_string_equal_str(value, "14b") || furi_string_equal_str(value, "iso14b")) {
            ctx->protocol = NfcProtocolIso14443_3b;
        } else if(furi_string_equal_str(value, "15")) {
            ctx->protocol = NfcProtocolIso15693_3;
        } else if(furi_string_equal_str(value, "felica")) {
            ctx->protocol = NfcProtocolFelica;
        } else {
            result = false;
            ctx->protocol = NfcProtocolInvalid;
        }
    } while(false);
    return result;
}

static bool nfc_cli_raw_parse_data(FuriString* value, void* output) {
    NfcCliRawCmdContext* ctx = output;

    bool result = false;

    do {
        size_t len = furi_string_size(value);
        if(len % 2 != 0) break;

        ctx->data_length = len / 2;
        ctx->data = malloc(ctx->data_length);
        if(!args_read_hex_bytes(value, ctx->data, ctx->data_length)) break;

        result = true;
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
};

const NfcCliActionDescriptor* raw_actions_collection[] = {&raw_action};
//Command descriptor

//ADD_NFC_CLI_COMMAND(raw, raw_actions_collection);

//static void nfc_cli_command_raw_callback(PipeSide* pipe, FuriString* args, void* context);
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

//ADD_NFC_CLI_COMMAND(raw, raw_actions_collection);

//Command usage: raw <protocol> [keys] <data>
//Command examples:
//raw iso14a -sc 3000
//raw iso14a 3000
//raw iso14a 3000 -sc
