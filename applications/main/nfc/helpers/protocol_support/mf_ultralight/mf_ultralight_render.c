#include "mf_ultralight_render.h"

#include "../iso14443_3a/iso14443_3a_render.h"

void nfc_render_mf_ultralight_info(
    const MfUltralightData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_iso14443_3a_info(data->iso14443_3a_data, format_type, str);

    furi_string_cat_printf(str, "\nPages Read: %u/%u", data->pages_read, data->pages_total);
    if(data->pages_read != data->pages_total) {
        furi_string_cat_printf(str, "\nPassword-protected pages!");
    }

    //TODO: Something else?
}
