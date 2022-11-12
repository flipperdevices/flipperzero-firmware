#pragma once
#include <furi.h>
#include <flipper_format_i.h>
#include <flipper_format_stream_i.h>
#include <subghz/types.h>

bool load_file_shapshup(Storage* storage, FlipperFormat* fff_data, const char* file_path);
bool read_int32_shapshup(
    Stream* stream,
    int32_t* _data,
    const uint16_t data_size);