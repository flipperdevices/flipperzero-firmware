#include "pretty_format.h"

void pretty_format_bytes_hex(
    FuriString* result,
    size_t row_length,
    const uint8_t* data,
    size_t data_size) {
    for(size_t i = 0; i < data_size; ++i) {
        const char* format = (i < (data_size - 1)) ? "%02X%c" : "%02X";
        const char separator = ((i + 1) % row_length) ? ' ' : '\n';

        furi_string_cat_printf(result, format, data[i], separator);
    }
}

void pretty_format_bytes_ascii(
    FuriString* result,
    size_t row_length,
    const uint8_t* data,
    size_t data_size) {
    for(size_t i = 0; i < data_size; ++i) {
        const char c = data[i];
        const char* format = ((i + 1) % row_length) ? "%c" : "%c%c";

        furi_string_cat_printf(result, format, (c > 0x1f && c < 0x7f) ? c : '.', '\n');
    }
}
