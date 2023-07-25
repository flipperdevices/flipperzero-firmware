#include "iso14443_3b_render.h"

void nfc_render_iso14443_3b_info(
    const Iso14443_3bData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        const char iso_type = FURI_BIT(data->protocol_info[1], 0) ? '4' : '3';
        furi_string_cat_printf(str, "ISO 14443-%c (NFC-B)\n", iso_type);
    }

    furi_string_cat_printf(str, "UID:");

    for(size_t i = 0; i < ISO14443_3B_UID_SIZE; i++) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "\nApp. data:");
        for(size_t i = 0; i < ISO14443_3B_APP_DATA_SIZE; ++i) {
            furi_string_cat_printf(str, " %02X", data->app_data[i]);
        }
        furi_string_cat_printf(str, "\nProtocol info:");
        for(size_t i = 0; i < ISO14443_3B_PROTOCOL_INFO_SIZE; ++i) {
            furi_string_cat_printf(str, " %02X", data->protocol_info[i]);
        }
    }
}
