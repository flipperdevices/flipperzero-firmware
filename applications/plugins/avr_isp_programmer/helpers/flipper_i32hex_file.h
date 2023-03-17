#pragma once

#include <furi_hal.h>

typedef struct FlipperI32HexFile FlipperI32HexFile;

FlipperI32HexFile* flipper_i32hex_file_open_write(const char* name, uint32_t start_addr);
FlipperI32HexFile* flipper_i32hex_file_open_read(const char* name);
void flipper_i32hex_file_close(FlipperI32HexFile* instance);
void flipper_i32hex_file_bin_to_i32hex_add_data(
    FlipperI32HexFile* instance,
    uint8_t* data,
    uint32_t data_size);
void flipper_i32hex_file_bin_to_i32hex_add_end_line(FlipperI32HexFile* instance);
const char* flipper_i32hex_file_get_string(FlipperI32HexFile* instance);
void flipper_i32hex_file_bin_to_i32hex_set_addr(FlipperI32HexFile* instance, uint32_t addr);
bool flipper_i32hex_file_check(FlipperI32HexFile* instance);

