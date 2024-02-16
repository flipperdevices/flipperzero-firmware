#include "tullave_data.h"

const SimpleArrayConfig simple_array_config_tllave_history = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(uint8_t),
};

void tullave_data_format_bytes(FuriString* str, const uint8_t* const data, size_t size) {
    for(size_t i = 0; i < size; i++) {
        furi_string_cat_printf(str, " %02X", data[i]);
    }
}

uint64_t uint8_to_integer_big_endian(const uint8_t* src, size_t length) {
    uint64_t result = 0;
    for(size_t i = 0; i < length; i++) {
        result |= ((uint64_t)src[i]) << (8 * (length - i - 1));
    }
    return result;
}

void tullave_data_free(TuLlaveData* buffer) {
    furi_assert(buffer);
    furi_string_free(buffer->card_number);
    simple_array_free(buffer->transaction_history);
    iso14443_4a_free(buffer->iso_data);
    free(buffer);
}

TuLlaveData* tullave_data_alloc() {
    TuLlaveData* buffer = malloc(sizeof(TuLlaveData));
    buffer->card_number = furi_string_alloc_set_str("");
    buffer->balance = 0;
    buffer->transaction_history = simple_array_alloc(&simple_array_config_tllave_history);
    buffer->iso_data = iso14443_4a_alloc();
    return buffer;
}
