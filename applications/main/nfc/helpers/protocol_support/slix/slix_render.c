#include "slix_render.h"

void nfc_render_slix_info(const SlixData* data, NfcProtocolFormatType format_type, FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "%s\n", slix_get_device_name(data, NfcDeviceNameTypeFull));
    }

    const Iso15693_3Data* iso15693_3_data = slix_get_base_data(data);

    furi_string_cat_printf(str, "UID:");
    for(size_t i = 0; i < ISO15693_3_UID_SIZE; i++) {
        furi_string_cat_printf(str, " %02X", iso15693_3_data->uid[i]);
    }

    if(format_type != NfcProtocolFormatTypeFull) return;

    furi_string_push_back(str, '\n');
    // TODO: Implement the rendering
    furi_string_cat(str, "(Work in progress)");
}
