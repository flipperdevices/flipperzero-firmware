#include "nfc_transaction_formatter.h"
#include "nfc_history_formatter.h"
#include "../protocols/nfc_protocol_formatters.h"
#include <nfc/helpers/logger/transaction/nfc_transaction_data_type.h>

static const char* nfc_transaction_type_name[] = {
    [NfcTransactionTypeEmpty] = "HalNfc",
    [NfcTransactionTypeFlagsOnly] = "FlagsOnly",
    [NfcTransactionTypeRequest] = "Request",
    [NfcTransactionTypeResponse] = "Response",
    [NfcTransactionTypeRequestResponse] = "Full",
};

static const char* nfc_history_crc_status_message[] = {
    [NfcHistoryCrcNotSet] = "",
    [NfcHistoryCrcOk] = "OK",
    [NfcHistoryCrcBad] = "KO",
};

static void
    nfc_transaction_format_crc_status(const NfcFormatter* formatter, FuriString* crc_status_str) {
    furi_string_printf(
        crc_status_str, "%s", nfc_history_crc_status_message[formatter->crc_from_history]);
}

static void
    nfc_packet_format_without_crc(const uint8_t* data, const size_t data_size, FuriString* output) {
    for(size_t i = 0; i < data_size; i++) {
        furi_string_cat_printf(output, "%02X ", data[i]);
    }
}

static void
    nfc_packet_format(NfcHistoryCrcStatus crc_status, const NfcPacket* packet, FuriString* output) {
    size_t n = packet->data_size;

    if((packet->data_size >= 3) && (crc_status != NfcHistoryCrcNotSet)) {
        n = packet->data_size - 2;
    }

    nfc_packet_format_without_crc(packet->data, n, output);

    if((packet->data_size >= 3) && (crc_status != NfcHistoryCrcNotSet)) {
        const char* format = (crc_status == NfcHistoryCrcOk) ? "[ %02X %02X ]" : "![ %02X %02X ]";
        furi_string_cat_printf(output, format, packet->data[n], packet->data[n + 1]);
    }
}

static void nfc_packet_format_append_decrypted(
    const NfcHistoryCrcStatus crc_status,
    const NfcPacket* packet,
    FuriString* output) {
    do {
        furi_string_cat_printf(output, " = ");
        nfc_packet_format(crc_status, packet, output);
    } while(false);
}

static NfcPacket* nfc_packet_copy(const NfcPacket* packet) {
    furi_assert(packet);
    NfcPacket* copy = malloc(sizeof(NfcPacket));
    copy->data = malloc(packet->data_size);
    memcpy(copy->data, packet->data, packet->data_size);
    copy->data_size = packet->data_size;
    copy->time = packet->time;
    copy->flags = packet->flags;
    return copy;
}

static void nfc_packet_free(NfcPacket* packet) {
    if(packet) {
        if(packet->data) free(packet->data);
        free(packet);
    }
}

static void nfc_transaction_format_common(
    NfcFormatter* instance,
    const NfcTransaction* transaction,
    NfcTransactionType desired_type,
    NfcTransactionString* output) {
    do {
        const NfcTransactionHeader* header = &transaction->header;
        uint32_t time = (header->type == NfcTransactionTypeFlagsOnly ||
                         header->type == NfcTransactionTypeEmpty ||
                         header->type == NfcTransactionTypeResponse) ?
                            header->start_time :
                            transaction->request->time;

        if(desired_type != NfcTransactionTypeResponse ||
           header->type != NfcTransactionTypeRequestResponse) {
            furi_string_printf(output->id, "%lu", header->id);
            furi_string_printf(output->type, "%s", nfc_transaction_type_name[header->type]);
            furi_string_printf(output->src, "RDR");
        } else
            furi_string_printf(output->src, "TAG");

        furi_string_printf(output->time, "%lu", time);

        furi_string_reset(output->payload);
        if((header->type == NfcTransactionTypeRequest ||
            header->type == NfcTransactionTypeRequestResponse) &&
           (desired_type == NfcTransactionTypeRequest)) {
            nfc_packet_format(instance->crc_from_history, transaction->request, output->payload);

            if(instance->decrypt_context && instance->decrypted_transaction &&
               instance->decrypted_transaction->request)
                nfc_packet_format_append_decrypted(
                    instance->crc_from_history,
                    instance->decrypted_transaction->request,
                    output->payload);
        }

        if((header->type == NfcTransactionTypeResponse ||
            header->type == NfcTransactionTypeRequestResponse) &&
           (desired_type == NfcTransactionTypeResponse)) {
            nfc_packet_format(instance->crc_from_history, transaction->response, output->payload);

            if(instance->decrypt_context && instance->decrypted_transaction &&
               instance->decrypted_transaction->response)
                nfc_packet_format_append_decrypted(
                    instance->crc_from_history,
                    instance->decrypted_transaction->response,
                    output->payload);
        }

        nfc_transaction_format_crc_status(instance, output->crc_status);
    } while(false);
}

inline static void nfc_transaction_format_request(
    NfcFormatter* instance,
    const NfcTransaction* transaction,
    NfcTransactionString* output) {
    furi_assert(output);
    furi_assert(transaction);

    nfc_transaction_format_common(instance, transaction, NfcTransactionTypeRequest, output);
    nfc_histroy_format_annotation(
        instance,
        transaction->request,
        transaction->history,
        transaction->header.nfc_event,
        output->annotation);
}

inline static void nfc_transaction_format_response(
    NfcFormatter* instance,
    const NfcTransaction* transaction,
    NfcTransactionString* output) {
    furi_assert(output);
    furi_assert(transaction);

    nfc_transaction_format_common(instance, transaction, NfcTransactionTypeResponse, output);
}

static bool nfc_transaction_filter_check(
    const NfcLoggerTransactionFilter transaction_filter,
    const NfcTransactionType type) {
    bool result = true;
    if(transaction_filter == NfcLoggerTransactionFilterPayloadOnly &&
       (type != NfcTransactionTypeRequestResponse && type != NfcTransactionTypeRequest &&
        type != NfcTransactionTypeResponse)) {
        result = false;
    }
    return result;
}

static bool nfc_transaction_decrypt_context_alloc(
    NfcFormatter* instance,
    const NfcTransaction* transaction) {
    bool result = false;
    do {
        const NfcHistoryItem* item =
            nfc_history_get_item(transaction->history, instance->protocol);

        if(!item) break;
        instance->decrypt_context = malloc(item->base.data_block_size);
        memcpy(instance->decrypt_context, &item->data, item->base.data_block_size);

        instance->decrypted_transaction = malloc(sizeof(NfcTransactionDecryptedData));
        if(transaction->request)
            instance->decrypted_transaction->request = nfc_packet_copy(transaction->request);
        if(transaction->response)
            instance->decrypted_transaction->response = nfc_packet_copy(transaction->response);

        result = true;
    } while(false);
    return result;
}

static void nfc_transaction_decrypt_context_free(NfcFormatter* instance) {
    if(instance->decrypted_transaction) {
        nfc_packet_free(instance->decrypted_transaction->request);
        nfc_packet_free(instance->decrypted_transaction->response);

        free(instance->decrypted_transaction);
        instance->decrypted_transaction = NULL;
    }

    if(instance->decrypt_context) {
        free(instance->decrypt_context);
        instance->decrypt_context = NULL;
    }
}

static void nfc_transaction_decrypt(NfcFormatter* instance, const NfcTransaction* transaction) {
    furi_assert(instance);
    furi_assert(transaction);

    do {
        if(transaction->header.type == NfcTransactionTypeEmpty ||
           transaction->header.type == NfcTransactionTypeFlagsOnly)
            break;

        if(!nfc_transaction_decrypt_context_alloc(instance, transaction)) break;

        NfcTransactionDecryptCallback decrypt_callback =
            nfc_protocol_formatter_get_decrypt_callback(instance->protocol, instance->mode);

        if(transaction->request) {
            if(!decrypt_callback(
                   transaction->request,
                   instance->decrypt_context,
                   instance->decrypted_transaction->request)) {
                nfc_packet_free(instance->decrypted_transaction->request);
                instance->decrypted_transaction->request = NULL;
            }
        }

        if(transaction->response) {
            if(!decrypt_callback(
                   transaction->response,
                   instance->decrypt_context,
                   instance->decrypted_transaction->response)) {
                nfc_packet_free(instance->decrypted_transaction->response);
                instance->decrypted_transaction->response = NULL;
            }
        }

    } while(false);
}

void nfc_transaction_format(
    NfcFormatter* instance,
    const NfcTransaction* transaction,
    FuriString* output) {
    furi_assert(instance);
    furi_assert(transaction);
    furi_assert(output);
    NfcTransactionString* tr_str = nfc_transaction_string_alloc();

    do {
        const NfcLoggerTransactionFilter filter = instance->config->transaction_filter;
        const NfcTransactionType type = transaction->header.type;
        if(!nfc_transaction_filter_check(filter, type)) {
            break;
        }

        Table* table = instance->table;
        const uint8_t count = instance->table_columns_cnt;

        if(nfc_protocol_formatter_support_decryption(instance->protocol, instance->mode)) {
            nfc_transaction_decrypt(instance, transaction);
        }

        if(type != NfcTransactionTypeResponse) {
            nfc_transaction_string_reset(tr_str);
            nfc_transaction_format_request(instance, transaction, tr_str);
            table_printf_row_array(table, output, (FuriString**)tr_str, count);
        }

        if(type == NfcTransactionTypeResponse || type == NfcTransactionTypeRequestResponse) {
            nfc_transaction_string_reset(tr_str);
            nfc_transaction_format_response(instance, transaction, tr_str);
            table_printf_row_array(table, output, (FuriString**)tr_str, count);
        }
    } while(false);
    nfc_transaction_string_free(tr_str);
    nfc_transaction_decrypt_context_free(instance);
}
