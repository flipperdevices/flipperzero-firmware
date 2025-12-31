#include "nfc_history_i.h"
#include "nfc_history_chain_data_type.h"

#define TAG "NfcHistory"

typedef struct {
    NfcHistoryItemBase base;
    uint8_t data[];
} FURI_PACKED NfcHistoryItemInternal;

NfcHistory* nfc_history_alloc(uint8_t history_size_bytes, uint8_t max_chain_count) {
    NfcHistory* history = malloc(history_size_bytes);
    history->base.history_size_bytes = history_size_bytes;
    history->base.max_chain_count = max_chain_count;
    return history;
}

void nfc_history_free(NfcHistory* instance) {
    furi_assert(instance);

    free(instance);
}

void nfc_history_append(NfcHistory* instance, const NfcHistoryItem* item) {
    furi_assert(instance);
    furi_assert(item);
    NfcHistoryBase* base = &instance->base;
    do {
        if(base->chain_count == base->max_chain_count) {
            FURI_LOG_W(TAG, "History is full");
            break;
        }

        if(base->chain_count == 0) base->chain_count += 1;

        uint8_t current_chain_index = base->chain_count - 1;
        if(instance->chains[current_chain_index].length == base->max_chain_count) {
            base->chain_count += 1;
            current_chain_index += 1;
        }

        NfcHistoryChain* chain = &instance->chains[current_chain_index];
        NfcHistoryItemInternal* chain_item = (NfcHistoryItemInternal*)&chain->items[chain->length];
        chain_item->base = item->base;
        memcpy(chain_item->data, item->data, item->base.data_block_size);

        chain->length++;
    } while(false);
}

static inline bool nfc_history_chain_save(Stream* stream, const NfcHistoryChain* chain) {
    bool result = false;

    do {
        if(stream_write(stream, &chain->length, sizeof(chain->length)) != sizeof(chain->length)) {
            FURI_LOG_E(TAG, "Unable to save history chain");
            break;
        }

        result = true;
        for(size_t i = 0; i < chain->length; i++) {
            const NfcHistoryItemInternal* item = (NfcHistoryItemInternal*)&chain->items[i];

            result &= stream_write(stream, (uint8_t*)&item->base, sizeof(NfcHistoryItemBase)) ==
                      sizeof(NfcHistoryItemBase);

            result &= stream_write(stream, item->data, item->base.data_block_size) ==
                      item->base.data_block_size;

            if(!result) {
                FURI_LOG_E(TAG, "Unable to save history item");
                break;
            }
        }
    } while(false);
    return result;
}

bool nfc_history_save(Stream* stream, const NfcHistory* instance) {
    furi_assert(stream);
    furi_assert(instance);

    bool result = false;
    do {
        size_t bytes_to_write = sizeof(NfcHistoryBase);
        if(stream_write(stream, (uint8_t*)&instance->base, bytes_to_write) != bytes_to_write) {
            FURI_LOG_E(TAG, "Unable to save history");
            break;
        }

        for(uint8_t i = 0; i < instance->base.chain_count; i++) {
            nfc_history_chain_save(stream, &instance->chains[i]);
        }

        result = true;
    } while(false);
    return result;
}

static inline bool nfc_history_item_load(Stream* stream, NfcHistoryItemInternal* item) {
    bool result = false;
    do {
        size_t bytes_to_read = sizeof(NfcHistoryItemBase);
        size_t read = stream_read(stream, (uint8_t*)&item->base, bytes_to_read);
        if(bytes_to_read != read) break;

        bytes_to_read = item->base.data_block_size;
        read = stream_read(stream, item->data, bytes_to_read);
        if(bytes_to_read != read) break;

        result = true;
    } while(false);
    return result;
}

static inline bool nfc_history_chain_load(Stream* stream, NfcHistoryChain* chain) {
    bool result = false;
    do {
        size_t bytes_to_read = sizeof(chain->length);
        size_t read = stream_read(stream, &chain->length, bytes_to_read);
        if(bytes_to_read != read) break;

        for(size_t i = 0; i < chain->length; i++) {
            result = nfc_history_item_load(stream, (NfcHistoryItemInternal*)&chain->items[i]);
            if(!result) break;
        }
    } while(false);
    return result;
}

bool nfc_history_load(Stream* stream, NfcHistory** instance_ptr) {
    furi_assert(stream);
    furi_assert(instance_ptr);
    bool result = false;
    do {
        NfcHistoryBase base;
        size_t read = stream_read(stream, (uint8_t*)&base, sizeof(NfcHistoryBase));
        if(read != sizeof(NfcHistoryBase)) {
            FURI_LOG_W(TAG, "Unable to read history from file");
            break;
        }
        NfcHistory* history = nfc_history_alloc(base.history_size_bytes, base.max_chain_count);
        history->base = base;

        result = true;
        for(size_t i = 0; i < history->base.chain_count; i++) {
            result = nfc_history_chain_load(stream, &history->chains[i]);
            if(!result) {
                FURI_LOG_W(TAG, "Unable to read chain from file");
                break;
            }
        }

        if(result) {
            *instance_ptr = history;
        } else {
            nfc_history_free(history);
        }
    } while(false);
    return result;
}
