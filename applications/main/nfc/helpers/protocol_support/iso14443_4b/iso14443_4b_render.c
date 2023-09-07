#include "iso14443_4b_render.h"

#include "../iso14443_3b/iso14443_3b_render.h"

void nfc_render_iso14443_4b_info(
    const Iso14443_4bData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_iso14443_3b_info(data->iso14443_3b_data, format_type, str);
    // TODO: Add RATS info?
}
