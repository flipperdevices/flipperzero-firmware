#include "felica_seac_render.h"

void nfc_render_felica_seac_info(
    const FelicaSeacData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    furi_check(data);
    furi_check(str);

    furi_string_cat(str, (format_type == NfcProtocolFormatTypeFull) ? "IDm:\n" : "IDm:");
    for(size_t i = 0; i < FELICA_SEAC_IDM_SIZE; i++) {
        furi_string_cat_printf(
            str, (format_type == NfcProtocolFormatTypeFull) ? "%02X " : " %02X", data->idm[i]);
    }

    furi_string_cat(str, "\nCTX:");
    for(size_t i = 0; i < FELICA_SEAC_CONTEXT_SIZE; i++) {
        furi_string_cat_printf(str, " %02X", data->context[i]);
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat(str, "\nReading card contents requires authentication.");
    }
}
