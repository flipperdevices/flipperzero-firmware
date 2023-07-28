#include "iso15693_3_render.h"

void nfc_render_iso15693_3_info(
    const Iso15693_3Data* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat(str, "ISO15693-3 (NFC-V)\n");
    }

    furi_string_cat_printf(str, "UID:");
    for(size_t i = 0; i < ISO15693_3_UID_SIZE; i++) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }

    furi_string_push_back(str, '\n');

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "IC Reference: %u\n", data->ic_ref);
        furi_string_cat_printf(str, "Block count: %u\n", data->block_count);
        furi_string_cat_printf(str, "Block size: %u\n", data->block_size);

        furi_string_cat_printf(
            str, "Data (%lu bytes)\n", simple_array_get_count(data->block_data));

        uint8_t block_count = data->block_count;

        if(block_count > 32) {
            block_count = 32;
            furi_string_cat_printf(str, "(truncated to %u blocks)\n", block_count);
        }

        for(uint32_t i = 0; i < block_count; ++i) {
            for(uint32_t j = 0; j < data->block_size; j++) {
                const uint8_t byte =
                    *(uint8_t*)simple_array_cget(data->block_data, i * data->block_size + j);
                furi_string_cat_printf(str, " %02X", byte);
            }

            const uint8_t block_status =
                *(uint8_t*)simple_array_cget(data->security_status, i + 1);
            furi_string_cat_printf(str, " %s\n", (block_status & 0x01) ? "(lck)" : "");
        }
    }
}
