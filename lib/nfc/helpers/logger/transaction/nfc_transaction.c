#include "nfc_transaction_i.h"
#include "nfc_transaction_data_type.h"
#include "../history/nfc_history_i.h"

#define TAG "NfcTransaction"

size_t nfc_transaction_get_size(uint8_t history_size_bytes) {
    return (history_size_bytes + sizeof(NfcTransaction));
}

NfcTransaction* nfc_transaction_alloc(
    uint32_t id,
    FuriHalNfcEvent event,
    uint32_t time,
    uint8_t history_size_bytes,
    uint8_t max_chain_count) {
    NfcTransaction* t = malloc(sizeof(NfcTransaction));
    t->header.type = NfcTransactionTypeEmpty;
    t->header.id = id;
    t->header.nfc_event = event;
    t->header.start_time = time;
    t->history = nfc_history_alloc(history_size_bytes, max_chain_count);
    return t;
}

void nfc_transaction_refresh(
    NfcTransaction* instance,
    uint32_t id,
    FuriHalNfcEvent event,
    uint32_t time) {
    furi_assert(instance);
    instance->header.id = id;
    instance->header.nfc_event = event;
    instance->header.start_time = time;
    instance->header.type = NfcTransactionTypeEmpty;
}

void nfc_transaction_free(NfcTransaction* instance) {
    furi_assert(instance);

    if(instance->request) {
        if(instance->request->data) free(instance->request->data);
        free(instance->request);
    }

    if(instance->response) {
        if(instance->response->data) free(instance->response->data);
        free(instance->response);
    }

    nfc_history_free(instance->history);
    free(instance);
}

static NfcPacket* nfc_transaction_get_packet(NfcTransaction* transaction, bool response) {
    furi_assert(transaction);

    NfcPacket* p;
    if(response) {
        if(!transaction->response) transaction->response = malloc(sizeof(NfcPacket));
        transaction->header.type = transaction->header.type == NfcTransactionTypeRequest ?
                                       NfcTransactionTypeRequestResponse :
                                       NfcTransactionTypeResponse;
        p = transaction->response;
    } else {
        if(!transaction->request) transaction->request = malloc(sizeof(NfcPacket));
        transaction->header.type = NfcTransactionTypeRequest;
        p = transaction->request;
    }
    return p;
}

void nfc_transaction_append(
    NfcTransaction* transaction,
    const uint32_t time,
    const uint8_t* data,
    const size_t data_size,
    bool response) {
    furi_assert(transaction);
    furi_assert(data);
    furi_assert(data_size > 0);

    NfcPacket* p = nfc_transaction_get_packet(transaction, response);
    p->time = time;
    p->data_size = data_size;
    p->data = malloc(data_size);
    memcpy(p->data, data, data_size);
}

void nfc_transaction_append_history(NfcTransaction* transaction, NfcHistoryItem* item) {
    furi_assert(transaction);
    furi_assert(item);

    if(transaction->header.type == NfcTransactionTypeEmpty) {
        transaction->header.type = NfcTransactionTypeFlagsOnly;
    }
    nfc_history_append(transaction->history, item);
}

NfcTransactionType nfc_transaction_get_type(const NfcTransaction* instance) {
    furi_assert(instance);
    return instance->header.type;
}

uint32_t nfc_transaction_get_id(const NfcTransaction* instance) {
    furi_assert(instance);
    return instance->header.id;
}

void nfc_transaction_complete(NfcTransaction* instance, uint32_t time) {
    furi_assert(instance);
    instance->header.end_time = time;
}

static bool nfc_transaction_save_packet(Stream* stream, NfcPacket* packet) {
    bool result = false;
    do {
        if(!packet) {
            FURI_LOG_W(TAG, "Attempt to save empty packet");
            break;
        }

        result = true;

        size_t bytes_to_write = sizeof(uint32_t) * 2 + sizeof(size_t);
        result &= stream_write(stream, (uint8_t*)&packet->time, bytes_to_write) == bytes_to_write;

        bytes_to_write = packet->data_size;
        result &= stream_write(stream, packet->data, bytes_to_write) == bytes_to_write;
    } while(false);

    return result;
}

bool nfc_transaction_save(Stream* stream, const NfcTransaction* transaction) {
    furi_assert(stream);
    furi_assert(transaction);

    bool result = false;
    do {
        size_t bytes_to_write = sizeof(NfcTransactionHeader);
        if(stream_write(stream, (uint8_t*)&(transaction->header), bytes_to_write) !=
           bytes_to_write) {
            FURI_LOG_E(TAG, "Failed to save transaction: %lu", transaction->header.id);
            break;
        }

        if(transaction->header.type == NfcTransactionTypeRequest ||
           transaction->header.type == NfcTransactionTypeRequestResponse) {
            if(!nfc_transaction_save_packet(stream, transaction->request)) break;
        }

        if(transaction->header.type == NfcTransactionTypeResponse ||
           transaction->header.type == NfcTransactionTypeRequestResponse)
            if(!nfc_transaction_save_packet(stream, transaction->response)) break;

        if(!nfc_history_save(stream, transaction->history)) break;
        result = true;
    } while(false);

    return result;
}

bool nfc_transaction_read(Stream* stream, NfcTransaction** transaction_ptr) {
    furi_assert(stream);
    furi_assert(transaction_ptr);

    bool result = false;
    NfcTransaction* transaction = malloc(sizeof(NfcTransaction));
    do {
        size_t bytes_read =
            stream_read(stream, (uint8_t*)transaction, sizeof(NfcTransactionHeader));
        if(bytes_read != sizeof(NfcTransactionHeader)) break;

        if(transaction->header.type == NfcTransactionTypeRequest ||
           transaction->header.type == NfcTransactionTypeRequestResponse) {
            transaction->request = malloc(sizeof(NfcPacket));
            stream_read(stream, (uint8_t*)transaction->request, sizeof(uint32_t) * 3);

            transaction->request->data = malloc(transaction->request->data_size);
            stream_read(stream, transaction->request->data, transaction->request->data_size);
        }

        if(transaction->header.type == NfcTransactionTypeResponse ||
           transaction->header.type == NfcTransactionTypeRequestResponse) {
            transaction->response = malloc(sizeof(NfcPacket));
            stream_read(stream, (uint8_t*)transaction->response, sizeof(uint32_t) * 3);

            transaction->response->data = malloc(transaction->response->data_size);
            stream_read(stream, transaction->response->data, transaction->response->data_size);
        }

        if(!nfc_history_load(stream, &transaction->history)) break;

        result = true;
        *transaction_ptr = transaction;
    } while(false);

    if(!result) nfc_transaction_free(transaction);

    return result;
}
