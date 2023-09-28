#include "iso14443_3a_render.h"

void nfc_render_iso14443_3a_info(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        const char iso_type = iso14443_3a_supports_iso14443_4(data) ? '4' : '3';
        furi_string_cat_printf(str, "ISO 14443-%c (NFC-A)\n", iso_type);
    }

    nfc_render_iso14443_3a_brief(data, str);

    if(format_type == NfcProtocolFormatTypeFull ||
       format_type == NfcProtocolFormatTypeShortExtra) {
        nfc_render_iso14443_3a_extra(data, str);
    }
}

void nfc_render_iso14443_3a_brief(const Iso14443_3aData* data, FuriString* str) {
    furi_string_cat_printf(str, "UID:");

    for(size_t i = 0; i < data->uid_len; i++) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }
}

void nfc_render_iso14443_3a_extra(const Iso14443_3aData* data, FuriString* str) {
    furi_string_cat_printf(str, "\nATQA: %02X %02X ", data->atqa[1], data->atqa[0]);
    furi_string_cat_printf(str, "\nSAK: %02X", data->sak);
}
