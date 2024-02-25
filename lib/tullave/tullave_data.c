#include "tullave_data.h"

const SimpleArrayConfig simple_array_config_tllave_history = {
    .init = NULL,
    .copy = NULL,
    .reset = NULL,
    .type_size = sizeof(TuLlaveTransaction),
};

void tullave_data_format_bytes(FuriString* str, const uint8_t* const data, size_t size) {
    for(size_t i = 0; i < size; i++) {
        furi_string_cat_printf(str, " %02X", data[i]);
    }
}

void tullave_data_get_uid_str(TuLlaveData* data, FuriString* dst_str) {
    furi_assert(data);
    uint8_t* uid = data->iso_data->iso14443_3a_data->uid;
    size_t uid_len = data->iso_data->iso14443_3a_data->uid_len;
    tullave_data_format_bytes(dst_str, uid, uid_len);
}

void tullave_data_set_iso_data(TuLlaveData* data, const Iso14443_4aData* iso_data) {
    furi_assert(data);
    furi_assert(iso_data);
    iso14443_4a_copy(data->iso_data, iso_data);
}

uint64_t tullave_data_uint8_to_balance(const uint8_t* src, size_t length) {
    uint64_t result = 0;
    for(size_t i = 0; i < length; i++) {
        result |= ((uint64_t)src[i]) << (8 * (length - i - 1));
    }
    return result;
}

void tullave_data_uint8_to_time(const uint8_t* src, size_t length, TuLlaveTime* time) {
    furi_assert(length >= TULLAVE_TIME_BYTES_LEN);
    furi_assert(time);

    // This string is only used as helper, to parse visual representation of hexa into decimal.
    FuriString* aux = furi_string_alloc();
    uint16_t i16_aux = 0;
    uint8_t i8_aux = 0;

    // Year
    furi_string_printf(aux, "%x%x", src[0], src[1]);
    sscanf(furi_string_get_cstr(aux), "%hi", &i16_aux);
    time->year = i16_aux;

    // Month
    furi_string_printf(aux, "%x", src[2]);
    sscanf(furi_string_get_cstr(aux), "%hhi", &i8_aux);
    time->mon = i8_aux;

    // Day
    furi_string_printf(aux, "%x", src[3]);
    sscanf(furi_string_get_cstr(aux), "%hhi", &i8_aux);
    time->mday = i8_aux;

    // Hour
    furi_string_printf(aux, "%x", src[4]);
    sscanf(furi_string_get_cstr(aux), "%hhi", &i8_aux);
    time->hour = i8_aux;

    // Minutes
    furi_string_printf(aux, "%x", src[5]);
    sscanf(furi_string_get_cstr(aux), "%hhi", &i8_aux);
    time->min = i8_aux;

    // Seconds
    furi_string_printf(aux, "%x", src[6]);
    sscanf(furi_string_get_cstr(aux), "%hhi", &i8_aux);
    time->sec = i8_aux;

    furi_string_free(aux);
}

void tullave_data_format_time(TuLlaveTime* time, FuriString* time_str) {
    furi_string_cat_printf(
        time_str,
        "%02d-%02d-%04d %02d:%02d:%02d",
        time->mday,
        time->mon,
        time->year,
        time->hour,
        time->min,
        time->sec);
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
