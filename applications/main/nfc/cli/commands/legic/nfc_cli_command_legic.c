#include "nfc_cli_command_legic.h"

#include "../helpers/nfc_cli_format.h"

#include <furi.h>
#include <toolbox/args.h>
#include <toolbox/strint.h>
#include <nfc/nfc_poller.h>
#include <nfc/protocols/legic_prime/legic_prime_poller.h>
#include <nfc/protocols/legic_prime/legic_prime_poller_sync.h>

// rdbl and wrbl's read-back wait for the poller in steps this long, so
// Ctrl+C can end them.
#define NFC_CLI_LEGIC_POLL_MS (100)

typedef struct {
    Nfc* nfc;
    uint16_t offset;
    uint8_t* data;
    uint16_t size;
    bool danger;
} NfcCliLegicWrblContext;

typedef struct {
    Nfc* nfc;
    uint16_t offset;
    uint16_t length;
    LegicPrimeData card;
} NfcCliLegicRdblContext;

typedef struct {
    NfcPoller* poller;
    FuriSemaphore* done;
    uint16_t offset;
    uint16_t size;
    LegicPrimeData* card;
    LegicPrimePollerEventDataResult result;
} NfcCliLegicRead;

static const char* nfc_cli_legic_get_error(LegicPrimeError error) {
    switch(error) {
    case LegicPrimeErrorNone:
        return "none";
    case LegicPrimeErrorNotPresent:
        return "card not present";
    case LegicPrimeErrorWrongCrc:
        return "replies kept failing their CRC check";
    case LegicPrimeErrorTimeout:
        return "timeout";
    case LegicPrimeErrorCommunication:
        return "communication error";
    case LegicPrimeErrorUnknownType:
        return "unknown card type";
    case LegicPrimeErrorNoAck:
        return "write not acknowledged";
    case LegicPrimeErrorReadOnly:
        return "address out of range (bytes 0-4 are read-only, or past the end of the card)";
    case LegicPrimeErrorWrongCard:
        return "a card with another UID answered";
    case LegicPrimeErrorUidUnconfirmed:
        return "could not read the card UID reliably";
    default:
        return "unknown error";
    }
}

static NfcCliActionContext* nfc_cli_legic_rdbl_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliLegicRdblContext* instance = malloc(sizeof(NfcCliLegicRdblContext));
    instance->nfc = nfc;
    instance->length = 16;
    legic_prime_reset(&instance->card);
    return instance;
}

static void nfc_cli_legic_rdbl_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    free(ctx);
}

static NfcCommand nfc_cli_legic_read_callback(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolLegicPrime);
    NfcCliLegicRead* read = context;
    const LegicPrimePollerEvent* legic_event = event.event_data;

    if(legic_event->type == LegicPrimePollerEventTypeRequestMode) {
        legic_event->data->mode_request.mode = LegicPrimePollerModeRead;
        legic_event->data->mode_request.read_offset = read->offset;
        legic_event->data->mode_request.read_size = read->size;
        return NfcCommandContinue;
    }
    // Success or Failure: the read is over either way.
    if(legic_event->type == LegicPrimePollerEventTypeSuccess) {
        legic_prime_copy(read->card, (const LegicPrimeData*)nfc_poller_get_data(read->poller));
    }
    read->result = legic_event->data->result;
    furi_semaphore_release(read->done);
    return NfcCommandStop;
}

// Reads `size` bytes from `offset` as legic_prime_poller_sync_read_range()
// does (`card` is only filled on success), but on the generic poller, as nfc
// dump does, so Ctrl+C or a closed pipe can end a long read.  Returns false
// if it was aborted before the poller reported.
static bool nfc_cli_legic_read(
    Nfc* nfc,
    PipeSide* pipe,
    uint16_t offset,
    uint16_t size,
    LegicPrimeData* card,
    LegicPrimePollerEventDataResult* result) {
    NfcCliLegicRead read = {
        .done = furi_semaphore_alloc(1, 0),
        .offset = offset,
        .size = size,
        .card = card,
    };
    bool aborted = false;
    read.poller = nfc_poller_alloc(nfc, NfcProtocolLegicPrime);
    nfc_poller_start(read.poller, nfc_cli_legic_read_callback, &read);
    while(furi_semaphore_acquire(read.done, NFC_CLI_LEGIC_POLL_MS) != FuriStatusOk) {
        if(cli_is_pipe_broken_or_is_etx_next_char(pipe)) {
            aborted = true;
            break;
        }
    }
    // Returns once the poller's current session is done (at most ~2 s), so
    // the callback can't run after this.
    nfc_poller_stop(read.poller);
    nfc_poller_free(read.poller);
    // A result the poller reported before it stopped stands, even one that
    // came after the last timed-out wait: the Ctrl+C then came too late, and
    // rdbl shows the bytes or the error (wrbl's read-back its outcome)
    // without saying so.
    const bool reported = !aborted || furi_semaphore_acquire(read.done, 0) == FuriStatusOk;
    furi_semaphore_free(read.done);
    if(reported) *result = read.result;
    return reported;
}

static void nfc_cli_legic_rdbl_execute(PipeSide* pipe, NfcCliActionContext* ctx) {
    furi_assert(pipe);
    NfcCliLegicRdblContext* instance = ctx;

    printf("Press Ctrl+C to abort\r\n");
    LegicPrimePollerEventDataResult result;
    if(!nfc_cli_legic_read(
           instance->nfc, pipe, instance->offset, instance->length, &instance->card, &result)) {
        printf(ANSI_FG_RED "Error: aborted" ANSI_RESET);
        return;
    }
    const LegicPrimeError error = result.error;
    const uint16_t confirmed = result.bytes_done;
    const uint16_t total = result.bytes_total;
    if(error != LegicPrimeErrorNone) {
        // Bytes that are not confirmed (two agreeing reads) aren't shown.
        printf(
            ANSI_FG_RED "Error: %s" ANSI_RESET,
            error == LegicPrimeErrorReadOnly ? "offset past the end of the card" :
                                               nfc_cli_legic_get_error(error));
        if(total) printf("\r\nConfirmed %u of %u bytes", (unsigned)confirmed, (unsigned)total);
        return;
    }
    // On success the range starts at the offset, inside the card, and
    // `total` is its length clamped to the card; the guard only keeps a
    // broken invariant from printing past the card.
    if(instance->offset + total > legic_prime_get_mem_size(instance->card.type)) return;
    printf("Offset: %u ", instance->offset);
    nfc_cli_printf_array(&instance->card.data[instance->offset], total, "Data: ");
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
           ctx->length > 0 && ctx->length <= LEGIC_PRIME_MEM_MAX;
}

static NfcCliActionContext* nfc_cli_legic_wrbl_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliLegicWrblContext* instance = malloc(sizeof(NfcCliLegicWrblContext));
    instance->nfc = nfc;
    return instance;
}

static void nfc_cli_legic_wrbl_free_ctx(NfcCliActionContext* ctx) {
    furi_assert(ctx);
    NfcCliLegicWrblContext* instance = ctx;
    free(instance->data);
    free(instance);
}

// The tag acknowledges a WRITE with one unencrypted bit: read the bytes back
// to tell whether the card holds them.
// The write is reported before this runs, so the read can be aborted.
static void nfc_cli_legic_wrbl_verify(
    Nfc* nfc,
    PipeSide* pipe,
    uint16_t offset,
    const uint8_t* data,
    uint16_t size) {
    LegicPrimeData* card = legic_prime_alloc();
    LegicPrimePollerEventDataResult result;
    printf("Reading the bytes back, press Ctrl+C to abort\r\n");
    if(!nfc_cli_legic_read(nfc, pipe, offset, size, card, &result)) {
        printf(ANSI_FG_RED "Written (acknowledged), verify read aborted\r\n" ANSI_RESET);
    } else if(result.error != LegicPrimeErrorNone) {
        printf(
            ANSI_FG_RED "Written (acknowledged), but the verify read failed: %s\r\n" ANSI_RESET,
            nfc_cli_legic_get_error(result.error));
    } else if(result.bytes_total < size) {
        printf(
            ANSI_FG_RED
            "Written (acknowledged), but the verify read found a %u-byte card\r\n" ANSI_RESET,
            legic_prime_get_mem_size(card->type));
    } else {
        size_t i = 0;
        while(i < size && card->data[offset + i] == data[i]) {
            i++;
        }
        if(i == size) {
            printf(ANSI_FG_BR_GREEN "Verified\r\n" ANSI_RESET);
        } else {
            printf(
                ANSI_FG_RED "Verify mismatch at offset %u: wrote %02X, read %02X\r\n" ANSI_RESET,
                (unsigned)(offset + i),
                data[i],
                card->data[offset + i]);
        }
    }
    legic_prime_free(card);
}

static void nfc_cli_legic_wrbl_execute(PipeSide* pipe, NfcCliActionContext* ctx) {
    furi_assert(pipe);
    NfcCliLegicWrblContext* instance = ctx;

    // The poller makes the same check, but only after a handshake: without a
    // card the user would see "card not present" instead of the address
    // error.  It comes before the DCF check, as --danger can't make bytes 0-4
    // or a range past 1024 writable.
    // The card's own size is only known after the handshake; the poller
    // checks the range against it then.
    if(!legic_prime_is_writable(LEGIC_PRIME_MEM_MAX, instance->offset, instance->size)) {
        printf(
            ANSI_FG_RED "Error: %s\r\n" ANSI_RESET,
            nfc_cli_legic_get_error(LegicPrimeErrorReadOnly));
        return;
    }

    // The DCF can only ever be decreased on a real tag, so a write there
    // (e.g. turning new media into a data card) is irreversible.
    if(!instance->danger && legic_prime_is_dcf_write(instance->offset, instance->size)) {
        printf(ANSI_FG_RED "Bytes 5-6 are the DCF: changing it is irreversible on a real card.\r\n"
                           "Add --danger to write them anyway." ANSI_RESET);
        return;
    }

    // The write stays on the blocking sync call: its progress is only
    // reported in the poller's result, which a stopped poller never sends.
    // A Ctrl+C typed during it waits in the pipe and aborts the read-back
    // when one runs (bytes written, no card swap); otherwise the shell gets it.
    printf("Writing: Ctrl+C can't stop the write, only the read-back after it\r\n");
    uint16_t written = 0;
    bool unacked = false;
    LegicPrimeError error = legic_prime_poller_sync_write(
        instance->nfc, instance->offset, instance->data, instance->size, &written, &unacked);

    if(error == LegicPrimeErrorNone) {
        printf(ANSI_FG_BR_GREEN "Success\r\n" ANSI_RESET);
    } else {
        printf(ANSI_FG_RED "Error: %s\r\n" ANSI_RESET, nfc_cli_legic_get_error(error));
    }
    // Bytes go last to first: the ones written are the end of the range.
    const size_t end = instance->offset + instance->size;
    printf("Wrote %u of %u bytes, last to first", (unsigned)written, (unsigned)instance->size);
    if(written) printf(" (offsets %u-%u)", (unsigned)(end - written), (unsigned)(end - 1));
    printf("\r\n");
    if(unacked) {
        printf(
            "The byte at offset %u may have been written without an ACK\r\n",
            (unsigned)(end - written - 1));
    }
    if(written) {
        printf("Offset: %u ", (unsigned)(end - written));
        nfc_cli_printf_array(&instance->data[instance->size - written], written, "Data: ");
        printf("\r\n");
    }
    // The bytes the tag acknowledged, after a failure too.  Not after a swap:
    // the card on the antenna now is not the one they were written to.
    if(written && error != LegicPrimeErrorWrongCard) {
        nfc_cli_legic_wrbl_verify(
            instance->nfc,
            pipe,
            (uint16_t)(end - written),
            &instance->data[instance->size - written],
            written);
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
    if(len == 0 || len % 2 != 0 || len / 2 > LEGIC_PRIME_MEM_MAX) return false;
    ctx->size = len / 2;
    free(ctx->data);
    ctx->data = malloc(ctx->size);
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
        .description = "number of bytes (default 16, at most 1024)",
        .parse = nfc_cli_legic_rdbl_parse_length,
    },
};

//legic rdbl -o 0 -l 32
static const NfcCliActionDescriptor rdbl_action = {
    .name = "rdbl",
    .description = "Read bytes from a LEGIC Prime card, shown once all are confirmed",
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
        .description = "first byte to write (5 to 1023)",
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
    .description = "Write bytes to a LEGIC Prime card and read them back",
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
