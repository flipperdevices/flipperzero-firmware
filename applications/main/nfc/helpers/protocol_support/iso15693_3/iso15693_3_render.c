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

    if(format_type != NfcProtocolFormatTypeFull) return;

    furi_string_push_back(str, '\n');

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
        furi_string_cat_printf(str, "DSFID: %02X\n", data->system_info.ic_ref);
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_AFI) {
        furi_string_cat_printf(str, "AFI: %02X\n", data->system_info.ic_ref);
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_IC_REF) {
        furi_string_cat_printf(str, "IC Reference: %02X\n", data->system_info.ic_ref);
    }

    if(data->system_info.flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
        furi_string_cat_printf(str, "Block count: %u\n", data->system_info.block_count);
        furi_string_cat_printf(str, "Block size: %u\n", data->system_info.block_size);

        furi_string_cat_printf(
            str, "Data (%lu bytes)\n", simple_array_get_count(data->block_data));

        uint32_t block_count = data->system_info.block_count;

        if(block_count > 32) {
            block_count = 32;
            furi_string_cat_printf(str, "(truncated to %lu blocks)\n", block_count);
        }

        for(uint32_t i = 0; i < block_count; ++i) {
            furi_string_cat(str, "\e*");

            for(uint32_t j = 0; j < data->system_info.block_size; j++) {
                const uint8_t byte = *(uint8_t*)simple_array_cget(
                    data->block_data, i * data->system_info.block_size + j);
                furi_string_cat_printf(str, "%02X ", byte);
            }

            const uint8_t security = *(uint8_t*)simple_array_cget(data->security_status, i + 1);
            furi_string_cat_printf(str, " %s\n", (security & 0x01) ? "(lck)" : "");
        }
    }
}
