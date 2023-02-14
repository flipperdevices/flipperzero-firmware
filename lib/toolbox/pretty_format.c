#include "pretty_format.h"

#include <core/check.h>
#include <core/core_defines.h>

void pretty_format_bytes_hex_canonical(
    FuriString* result,
    size_t num_places,
    const char* line_prefix,
    const uint8_t* data,
    size_t data_size) {
    furi_assert(data);
    for(size_t i = 0; i < data_size; i += num_places) {
        if(line_prefix) {
            furi_string_cat(result, line_prefix);
        }

        const size_t begin_idx = i;
        const size_t end_idx = MIN(i + num_places, data_size);

        for(size_t j = begin_idx; j < end_idx; j++) {
            furi_string_cat_printf(result, "%02X ", data[j]);
        }

        furi_string_push_back(result, '|');

        for(size_t j = begin_idx; j < end_idx; j++) {
            const char c = data[j];
            const char sep = ((j < end_idx - 1) ? ' ' : '\n');
            const char* fmt = ((j < data_size - 1) ? "%c%c" : "%c");
            furi_string_cat_printf(result, fmt, (c > 0x1f && c < 0x7f) ? c : '.', sep);
        }
    }
}
