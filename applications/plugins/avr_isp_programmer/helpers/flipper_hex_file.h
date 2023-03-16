#pragma once

#include <furi_hal.h>

typedef struct FlipperHexFile FlipperHexFile;

FlipperHexFile* flipper_hex_file_open(const char* name, uint32_t start_addr);
void flipper_hex_file_close(FlipperHexFile* instance);
void flipper_hex_file_bin_to_hex_add_data(FlipperHexFile* instance, uint8_t* data, uint32_t data_size);
void flipper_hex_file_bin_to_hex_add_end_line(FlipperHexFile* instance);
const char* flipper_hex_file_get_string(FlipperHexFile* instance);
