#include "nfc_cli_command_legic.h"

#include "../helpers/nfc_cli_format.h"

#include <furi.h>
#include <toolbox/args.h>
#include <toolbox/strint.h>
#include <nfc/protocols/legic_prime/legic_prime_poller_sync.h>

#define TAG "LEGIC"

// Bytes 5-6 are the DCF.  On a real tag it can only ever be decreased, so a
// write there (e.g. turning new media into a data card) is irreversible.
#define NFC_CLI_LEGIC_DCF_FIRST (5U)
#define NFC_CLI_LEGIC_DCF_LAST  (6U)

typedef struct {
    Nfc* nfc;
    uint16_t offset;
    uint8_t data[LEGIC_PRIME_MEM_MAX];
    size_t size;
    bool danger;
} NfcCliLegicWrblContext;

typedef struct {
    Nfc* nfc;
    uint16_t offset;
    uint16_t length;
    LegicPrimeData card;
} NfcCliLegicRdblContext;

static const char* nfc_cli_legic_get_error(LegicPrimeError error) {
    switch(error) {
    case LegicPrimeErrorNone:
        return "none";
    case LegicPrimeErrorNotPresent:
        return "card not present";
    case LegicPrimeErrorWrongCrc:
        return "wrong CRC";
    case LegicPrimeErrorTimeout:
        return "timeout";
    case LegicPrimeErrorCommunication:
        return "communication error";
    case LegicPrimeErrorUnknownType:
        return "unknown card type";
    case LegicPrimeErrorNoAck:
        return "write not acknowledged";
    case LegicPrimeErrorReadOnly:
        return "address not writable (bytes 0-4, or past the end of the card)";
    case LegicPrimeErrorWrongCard:
        return "a card with another UID answered; the write was stopped";
    default:
        return "unknown error";
    }
}

static NfcCliActionContext* nfc_cli_legic_rdbl_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliLegicRdblContext* instance = malloc(sizeof(NfcCliLegicRdblContext));
    instance->nfc = nfc;
    instance->length = 16;
    return instance;
}

static void nfc_cli_legic_rdbl_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    free(ctx);
}

static void nfc_cli_legic_rdbl_execute(PipeSide* pipe, NfcCliActionContext* ctx) {
    furi_assert(pipe);
    NfcCliLegicRdblContext* instance = ctx;

    LegicPrimeError error = legic_prime_poller_sync_read(instance->nfc, &instance->card);
    if(error != LegicPrimeErrorNone) {
        printf(ANSI_FG_RED "Error: %s" ANSI_RESET, nfc_cli_legic_get_error(error));
        return;
    }
    if(instance->offset >= instance->card.data_size) {
        printf(
            ANSI_FG_RED "Error: offset past the end of the card (%u bytes)" ANSI_RESET,
            instance->card.data_size);
        return;
    }
    size_t length = instance->length;
    if(instance->offset + length > instance->card.data_size) {
        length = instance->card.data_size - instance->offset;
    }
    printf("\r\nOffset: %u ", instance->offset);
    nfc_cli_printf_array(&instance->card.data[instance->offset], length, "Data: ");
    printf("\r\n");
}

static bool nfc_cli_legic_rdbl_parse_offset(FuriString* value, NfcCliActionContext* output) {
    NfcCliLegicRdblContext* ctx = output;
    return strint_to_uint16(furi_string_get_cstr(value), NULL, &ctx->offset, 10) ==
           StrintParseNoError;
}

static bool nfc_cli_legic_rdbl_parse_length(FuriString* value, NfcCliActionContext* output) {
    NfcCliLegicRdblContext* ctx = output;
    return strint_to_uint16(furi_string_get_cstr(value), NULL, &ctx->length, 10) ==
               StrintParseNoError &&
           ctx->length > 0;
}

static NfcCliActionContext* nfc_cli_legic_wrbl_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliLegicWrblContext* instance = malloc(sizeof(NfcCliLegicWrblContext));
    instance->nfc = nfc;
    return instance;
}

static void nfc_cli_legic_wrbl_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    free(ctx);
}

static void nfc_cli_legic_wrbl_execute(PipeSide* pipe, NfcCliActionContext* ctx) {
    furi_assert(pipe);
    NfcCliLegicWrblContext* instance = ctx;

    if(!instance->danger && instance->offset <= NFC_CLI_LEGIC_DCF_LAST &&
       instance->offset + instance->size > NFC_CLI_LEGIC_DCF_FIRST) {
        printf(ANSI_FG_RED "Bytes 5-6 are the DCF: changing it is irreversible on a real card.\r\n"
                           "Add --danger to write them anyway." ANSI_RESET);
        return;
    }

    LegicPrimeError error = legic_prime_poller_sync_write(
        instance->nfc, instance->offset, instance->data, instance->size);

    if(error == LegicPrimeErrorNone) {
        printf(ANSI_FG_BR_GREEN "\r\nSuccess\r\n" ANSI_RESET);
        printf("Offset: %u ", instance->offset);
        nfc_cli_printf_array(instance->data, instance->size, "Data: ");
        printf("\r\n");
    } else {
        printf(ANSI_FG_RED "Error: %s" ANSI_RESET, nfc_cli_legic_get_error(error));
    }
}

static bool nfc_cli_legic_wrbl_parse_offset(FuriString* value, NfcCliActionContext* output) {
    NfcCliLegicWrblContext* ctx = output;
    StrintParseError err = strint_to_uint16(furi_string_get_cstr(value), NULL, &ctx->offset, 10);
    return err == StrintParseNoError;
}

static bool nfc_cli_legic_wrbl_parse_data(FuriString* value, void* output) {
    NfcCliLegicWrblContext* ctx = output;
    const size_t len = furi_string_size(value);
    if(len == 0 || len % 2 != 0 || len / 2 > sizeof(ctx->data)) return false;
    ctx->size = len / 2;
    return args_read_hex_bytes(value, ctx->data, ctx->size);
}

static const NfcCliKeyDescriptor rdbl_action_keys[] = {
    {
        .short_name = "o",
        .long_name = "offset",
        .features = {.required = true, .parameter = true},
        .description = "first byte to read",
        .parse = nfc_cli_legic_rdbl_parse_offset,
    },
    {
        .short_name = "l",
        .long_name = "length",
        .features = {.required = false, .parameter = true},
        .description = "number of bytes (default 16)",
        .parse = nfc_cli_legic_rdbl_parse_length,
    },
};

//legic rdbl -o 0 -l 32
static const NfcCliActionDescriptor rdbl_action = {
    .name = "rdbl",
    .description = "Read bytes from a LEGIC Prime card",
    .alloc = nfc_cli_legic_rdbl_alloc_ctx,
    .free = nfc_cli_legic_rdbl_free_ctx,
    .execute = nfc_cli_legic_rdbl_execute,
    .key_count = COUNT_OF(rdbl_action_keys),
    .keys = rdbl_action_keys,
};

static bool nfc_cli_legic_wrbl_parse_danger(FuriString* value, void* output) {
    UNUSED(value);
    NfcCliLegicWrblContext* ctx = output;
    ctx->danger = true;
    return true;
}

static const NfcCliKeyDescriptor wrbl_action_keys[] = {
    {
        .short_name = "o",
        .long_name = "offset",
        .features = {.required = true, .parameter = true},
        .description = "first byte to write (5 or more)",
        .parse = nfc_cli_legic_wrbl_parse_offset,
    },
    {
        .short_name = "d",
        .long_name = "data",
        .features = {.required = true, .parameter = true},
        .description = "bytes to write, hex",
        .parse = nfc_cli_legic_wrbl_parse_data,
    },
    {
        .long_name = "danger",
        .features = {.required = false, .parameter = false},
        .description = "allow writing the DCF (bytes 5-6), which is irreversible",
        .parse = nfc_cli_legic_wrbl_parse_danger,
    },
};

//legic wrbl -o 30 -d DEADBEEF
static const NfcCliActionDescriptor wrbl_action = {
    .name = "wrbl",
    .description = "Write bytes to a LEGIC Prime card",
    .alloc = nfc_cli_legic_wrbl_alloc_ctx,
    .free = nfc_cli_legic_wrbl_free_ctx,
    .execute = nfc_cli_legic_wrbl_execute,
    .key_count = COUNT_OF(wrbl_action_keys),
    .keys = wrbl_action_keys,
};

static const NfcCliActionDescriptor* legic_actions[] = {
    &rdbl_action,
    &wrbl_action,
};

//Command descriptor
ADD_NFC_CLI_COMMAND(legic, "LEGIC Prime specific commands", legic_actions);
