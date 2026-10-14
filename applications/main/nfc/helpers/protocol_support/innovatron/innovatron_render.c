#include "innovatron_render.h"

static void nfc_render_innovatron_bytes(
    FuriString* str,
    const char* label,
    const uint8_t* data,
    size_t data_size) {
    furi_string_cat_printf(str, "%s", label);
    for(size_t i = 0; i < data_size; i++) {
        furi_string_cat_printf(str, " %02X", data[i]);
    }
}

void nfc_render_innovatron_info(
    const InnovatronData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_innovatron_bytes(str, "DIV:", data->div, INNOVATRON_DIV_SIZE);

    if(data->atr_len) {
        furi_string_cat_printf(str, "\n");
        nfc_render_innovatron_bytes(str, "ATR:", data->atr, data->atr_len);
    } else {
        furi_string_cat_printf(str, "\nATR: n/a");
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "\nVerLog: %02X\nConfig: %02X", data->verlog, data->config);
    }
}
