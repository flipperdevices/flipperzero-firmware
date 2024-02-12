#include "tullave_data.h"

void tullave_iso14443_4a_format_bytes(FuriString* str, const uint8_t* const data, size_t size) {
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

void tullave_data_free(TuLlaveData* tullave_info) {
    furi_assert(tullave_info);
    furi_string_free(tullave_info->nfc_uid);
    furi_string_free(tullave_info->card_number);
    tullave_info = NULL;
}

TuLlaveData* tullave_data_alloc() {
    TuLlaveData* buffer = malloc(sizeof(TuLlaveData));
    buffer->nfc_uid = furi_string_alloc_set_str("");
    buffer->card_number = furi_string_alloc_set_str("");
    buffer->balance = 0;
    buffer->data_collected = false;
    return buffer;
}
