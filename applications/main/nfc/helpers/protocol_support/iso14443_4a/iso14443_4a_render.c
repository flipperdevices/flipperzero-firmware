#include "iso14443_4a_render.h"

#include "../iso14443_3a/iso14443_3a_render.h"

void nfc_render_iso14443_4a_info(
    const Iso14443_4aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_iso14443_3a_info(data->iso14443_3a_data, format_type, str);
    // TODO: Add RATS info?
}
