#include "felica_render.h"

static void nfc_render_felica_blocks_count(const FelicaData* data, FuriString* str) {
    furi_string_cat_printf(str, "\nBlocks Read: %u/%u", data->blocks_read, data->blocks_total);
    if(data->blocks_read != data->blocks_total) {
        furi_string_cat_printf(str, "\nAuth-protected blocks!");
    }
}

void nfc_render_felica_info(
    const FelicaData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    furi_string_cat_printf(str, "IDm:");

    for(size_t i = 0; i < FELICA_IDM_SIZE; i++) {
        furi_string_cat_printf(str, " %02X", data->idm.data[i]);
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "\nPMm:");
        for(size_t i = 0; i < FELICA_PMM_SIZE; ++i) {
            furi_string_cat_printf(str, " %02X", data->pmm.data[i]);
        }

        nfc_render_felica_blocks_count(data, str);

        furi_string_cat_printf(str, "\nDump:");

        size_t index = 0;
        for(size_t i = 0; i < data->blocks_total; i++) {
            furi_string_cat_printf(str, "\nSF1=%02X, ", data->data.dump[index++]);
            furi_string_cat_printf(str, "SF2=%02X\n", data->data.dump[index++]);
            for(size_t j = 0; j < FELICA_DATA_BLOCK_SIZE; j++) {
                if((j != 0) && (j % 8 == 0)) furi_string_cat_printf(str, "\n");
                furi_string_cat_printf(str, "%02X ", data->data.dump[index++]);
            }
        }
    }
}
