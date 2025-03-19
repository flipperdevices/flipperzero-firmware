#include "nfc_cli_format.h"

void nfc_cli_format_array(
    const uint8_t* data,
    const size_t data_size,
    const char* header,
    FuriString* output) {
    furi_assert(data);
    furi_assert(data_size > 0);
    furi_assert(header);
    furi_assert(output);

    furi_string_cat_printf(output, "%s", header);
    for(size_t i = 0; i < data_size; i++) {
        furi_string_cat_printf(output, "%02X ", data[i]);
    }
}

void nfc_cli_printf_array(const uint8_t* data, const size_t data_size, const char* header) {
    furi_assert(data);
    furi_assert(data_size > 0);
    furi_assert(header);

    printf("%s", header);
    for(size_t i = 0; i < data_size; i++) {
        printf("%02X ", data[i]);
    }
}
