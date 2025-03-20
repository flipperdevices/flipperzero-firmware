#include "nfc_cli_command_apdu.h"
#include "../../nfc_cli_command_processor.h"
#include "../helpers/nfc_cli_format.h"

#include "protocol_handlers/iso14443_4a/nfc_cli_apdu_iso14443_4a.h"
#include "protocol_handlers/iso14443_4b/nfc_cli_apdu_iso14443_4b.h"
#include "protocol_handlers/iso15693_3/nfc_cli_apdu_iso15693_3.h"

#include <furi.h>
#include <nfc/nfc.h>
#include <nfc/nfc_poller.h>

#include <toolbox/args.h>
#include <m-array.h>
#include <m-algo.h>

#define TAG "APDU"

///TODO: Move this define into some common place
#define NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE (256)

typedef NfcCommand (
    *NfcCliApduProtocolHandler)(NfcGenericEvent event, NfcCliApduRequestResponse* instance);

static const char* raw_error_names[] = {
    [NfcCliApduErrorNone] = "None",
    [NfcCliApduErrorTimeout] = "Timeout",
    [NfcCliApduErrorProtocol] = "Internal protocol",
    [NfcCliApduErrorWrongCrc] = "Wrong CRC",
    [NfcCliApduErrorNotPresent] = "No card",
};

typedef enum {
    NfcCliProtocolRequestTypeNormalExecute,
    NfcCliProtocolRequestTypeAbort,
} NfcCliProtocolRequestType;

typedef struct {
    uint8_t* data;
    size_t size;
} NfcCliApduData;

static void SubmenuItem_init(NfcCliApduData* item) {
    item->size = 0;
    item->data = NULL;
}

static void SubmenuItem_init_set(NfcCliApduData* item, const NfcCliApduData* src) {
    item->data = malloc(src->size);
    item->size = src->size;
    memcpy(item->data, src->data, src->size);
}

static void SubmenuItem_set(NfcCliApduData* item, const NfcCliApduData* src) {
    if(item->data && item->size != src->size) realloc(item->data, src->size);
    item->size = src->size;
    memcpy(item->data, src->data, src->size);
}

static void SubmenuItem_clear(NfcCliApduData* item) {
    if(item->data) free(item->data);
    item->data = NULL;
    item->size = 0;
}

ARRAY_DEF(
    NfcCliApduItemArray,
    NfcCliApduData,
    (INIT(API_2(SubmenuItem_init)),
     SET(API_6(SubmenuItem_set)),
     INIT_SET(API_6(SubmenuItem_init_set)),
     CLEAR(API_2(SubmenuItem_clear))))

typedef struct {
    Nfc* nfc;
    bool auto_detect;
    NfcCliApduItemArray_t apdu;
    NfcCliApduRequestResponse data;
    FuriMutex* wait_mutex;
    FuriSemaphore* sem_done;
    FuriMessageQueue* input_queue;
} NfcCliApduContext;

static NfcCliActionContext* nfc_cli_apdu_alloc_ctx(Nfc* nfc) {
    furi_assert(nfc);
    NfcCliApduContext* instance = malloc(sizeof(NfcCliApduContext));
    instance->nfc = nfc;
    instance->data.protocol = NfcProtocolInvalid;
    instance->auto_detect = true;
    NfcCliApduItemArray_init(instance->apdu);
    instance->data.rx_buffer = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE);
    instance->data.tx_buffer = bit_buffer_alloc(NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE);

    instance->sem_done = furi_semaphore_alloc(1, 0);
    instance->input_queue = furi_message_queue_alloc(1, sizeof(NfcCliProtocolRequestType));
    return instance;
}

static void nfc_cli_apdu_free_ctx(NfcCliActionContext* action_ctx) {
    furi_assert(action_ctx);
    NfcCliApduContext* instance = action_ctx;

    instance->nfc = NULL;
    NfcCliApduItemArray_clear(instance->apdu);

    bit_buffer_free(instance->data.rx_buffer);
    bit_buffer_free(instance->data.tx_buffer);
    furi_semaphore_free(instance->sem_done);
    furi_message_queue_free(instance->input_queue);
    free(instance);
}

static inline void nfc_cli_apdu_print_result(const NfcCliApduContext* instance) {
    nfc_cli_printf_array(
        bit_buffer_get_data(instance->data.tx_buffer),
        bit_buffer_get_size_bytes(instance->data.tx_buffer),
        "\r\nTx: ");

    if(instance->data.result != NfcCliApduErrorNone)
        printf("\r\nError: \"%s\"\r\n", raw_error_names[instance->data.result]);

    size_t rx_size = bit_buffer_get_size_bytes(instance->data.rx_buffer);
    if(rx_size > 0) {
        nfc_cli_printf_array(
            bit_buffer_get_data(instance->data.rx_buffer),
            bit_buffer_get_size_bytes(instance->data.rx_buffer),
            "\r\nRx: ");
        printf("\r\n");
    }
}

static NfcProtocol nfc_cli_apdu_protocol_autodetect(Nfc* nfc) {
    const NfcProtocol supported_protocols[] = {
        NfcProtocolIso14443_4a,
        NfcProtocolIso14443_4b,
        NfcProtocolIso15693_3,
    };

    const char* supported_names[] = {"Iso14443_4a", "Iso14443_4b", "Iso15693_3"};

    NfcProtocol protocol = NfcProtocolInvalid;
    for(uint8_t i = 0; i < COUNT_OF(supported_protocols); i++) {
        NfcPoller* poller = nfc_poller_alloc(nfc, supported_protocols[i]);
        bool is_detected = nfc_poller_detect(poller);
        nfc_poller_free(poller);
        if(is_detected) {
            protocol = supported_protocols[i];
            printf("Detected tag: %s\r\n", supported_names[i]);
            break;
        }
    }
    return protocol;
}

static NfcCliApduProtocolHandler nfc_cli_apdu_poller_get_handler(NfcProtocol protocol) {
    if(protocol == NfcProtocolIso14443_4a)
        return nfc_cli_apdu_iso14443_4a_handler;
    else if(protocol == NfcProtocolIso14443_4b)
        return nfc_cli_apdu_iso14443_4b_handler;
    else if(protocol == NfcProtocolIso15693_3)
        return nfc_cli_apdu_iso15693_3_handler;
    else
        return NULL;
}

static NfcCommand nfc_cli_apdu_poller_callback(NfcGenericEvent event, void* context) {
    NfcCliApduContext* instance = context;

    FURI_LOG_D(TAG, "Poller callback");
    NfcCliProtocolRequestType request_type = NfcCliProtocolRequestTypeAbort;
    furi_message_queue_get(instance->input_queue, &request_type, FuriWaitForever);

    NfcCommand command = NfcCommandStop;
    if(request_type == NfcCliProtocolRequestTypeAbort) {
        FURI_LOG_D(TAG, "Aborting poller callback");
        command = NfcCommandStop;
    } else {
        NfcCliApduProtocolHandler handler =
            nfc_cli_apdu_poller_get_handler(instance->data.protocol);
        if(handler) command = handler(event, &instance->data);
    }
    furi_semaphore_release(instance->sem_done);
    return command;
}

static void nfc_cli_apdu_execute(PipeSide* pipe, void* context) {
    UNUSED(pipe);
    furi_assert(context);
    NfcCliApduContext* instance = context;

    if(instance->auto_detect) {
        instance->data.protocol = nfc_cli_apdu_protocol_autodetect(instance->nfc);
    }

    if(instance->data.protocol != NfcProtocolInvalid) {
        NfcPoller* poller = nfc_poller_alloc(instance->nfc, instance->data.protocol);

        NfcCliProtocolRequestType request_type = NfcCliProtocolRequestTypeNormalExecute;
        nfc_poller_start(poller, nfc_cli_apdu_poller_callback, instance);

        NfcCliApduItemArray_it_t it;
        for(NfcCliApduItemArray_it(it, instance->apdu); !NfcCliApduItemArray_end_p(it);
            NfcCliApduItemArray_next(it)) {
            const NfcCliApduData* item = NfcCliApduItemArray_cref(it);
            bit_buffer_copy_bytes(instance->data.tx_buffer, item->data, item->size);
            bit_buffer_reset(instance->data.rx_buffer);

            furi_message_queue_put(instance->input_queue, &request_type, FuriWaitForever);
            furi_semaphore_acquire(instance->sem_done, FuriWaitForever);
            nfc_cli_apdu_print_result(instance);
            if(instance->data.result != NfcCliApduErrorNone) break;
        }

        request_type = NfcCliProtocolRequestTypeAbort;
        furi_message_queue_put(instance->input_queue, &request_type, FuriWaitForever);
        nfc_poller_stop(poller);
        nfc_poller_free(poller);
    }
}

static bool nfc_cli_apdu_parse_protocol(FuriString* value, void* output) {
    NfcCliApduContext* ctx = output;
    ctx->auto_detect = false;
    NfcProtocol new_protocol = NfcProtocolInvalid;

    ///TODO: Move parse protocol to separate helper function and reuse it here and in raw command
    ///An array of valid protocols must be provided for this function as an input parameter
    bool result = true;
    if(furi_string_equal_str(value, "4a")) {
        new_protocol = NfcProtocolIso14443_4a;
    } else if(furi_string_equal_str(value, "4b")) {
        new_protocol = NfcProtocolIso14443_4b;
    } else if(furi_string_equal_str(value, "15")) {
        new_protocol = NfcProtocolIso15693_3;
    } else {
        result = false;
        new_protocol = NfcProtocolInvalid;
    }

    if(result) {
        ctx->data.protocol = new_protocol;
    }
    return result;
}

static bool nfc_cli_raw_parse_data(FuriString* value, void* output) {
    NfcCliApduContext* ctx = output;

    bool result = false;
    FuriString* word = furi_string_alloc();

    while(args_read_string_and_trim(value, word)) {
        size_t len = furi_string_size(word);
        if(len % 2 != 0) break;

        size_t data_length = len / 2;
        NfcCliApduData* item = NfcCliApduItemArray_push_new(ctx->apdu);
        item->size = data_length;
        item->data = malloc(data_length);
        result = args_read_hex_bytes(word, item->data, item->size);
    }
    furi_string_free(word);

    return result;
}

const NfcCliKeyDescriptor apdu_keys[] = {
    {
        .long_name = "protocol",
        .short_name = "p",
        .description = "Set protocol (4a, 4b, 15) directly, otherwise autodetected",
        .features = {.parameter = true, .required = false},
        .parse = nfc_cli_apdu_parse_protocol,
    },
    {
        .long_name = "data",
        .short_name = "d",
        .features = {.parameter = true, .multivalue = true, .required = true},
        .parse = nfc_cli_raw_parse_data,
    },
};

const NfcCliActionDescriptor apdu_action = {
    .name = "apdu",
    .alloc = nfc_cli_apdu_alloc_ctx,
    .free = nfc_cli_apdu_free_ctx,
    .execute = nfc_cli_apdu_execute,
    .key_count = COUNT_OF(apdu_keys),
    .keys = apdu_keys,
};

const NfcCliActionDescriptor* apdu_actions_collection[] = {&apdu_action};

//Command descriptor
ADD_NFC_CLI_COMMAND(apdu, apdu_actions_collection);
/* const NfcCliCommandDescriptor apdu_cmd = {
    .name = "apdu",
    .action_count = 1,
    .actions = apdu_actions_collection,
}; */

//Command usage: apdu <protocol> <data>
//Command examples:
//apdu 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
//apdu 4a 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
//apdu 4b 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
//apdu 15 00a404000e325041592e5359532e444446303100 00A4040008A000000333010102
