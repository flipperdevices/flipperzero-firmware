#include "iso14443_3a_render.h"

void nfc_render_iso14443_3a_info(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        const char iso_type = FURI_BIT(data->sak, 5) ? '4' : '3';
        furi_string_cat_printf(str, "ISO 14443-%c (NFC-A)\n", iso_type);
    }

    furi_string_cat_printf(str, "UID:");

    for(size_t i = 0; i < data->uid_len; i++) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "\nATQA: %02X %02X ", data->atqa[1], data->atqa[0]);
        furi_string_cat_printf(str, " SAK: %02X", data->sak);
    }
}
