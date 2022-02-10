#pragma once
#include <stdint.h>
#include <mlib/m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlipperFormat FlipperFormat;

FlipperFormat* flipper_format_alloc_as_string();

void flipper_format_free(FlipperFormat* flipper_format);

bool flipper_format_rewind(FlipperFormat* flipper_format);

bool flipper_format_read_header(
    FlipperFormat* flipper_format,
    string_t filetype,
    uint32_t* version);

bool flipper_format_write_header(
    FlipperFormat* flipper_format,
    string_t filetype,
    const uint32_t version);

bool flipper_format_write_header_cstr(
    FlipperFormat* flipper_format,
    const char* filetype,
    const uint32_t version);

bool flipper_format_get_value_count(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t* count);

bool flipper_format_read_string(FlipperFormat* flipper_format, const char* key, string_t data);

bool flipper_format_write_string(FlipperFormat* flipper_format, const char* key, string_t data);

bool flipper_format_write_string_cstr(
    FlipperFormat* flipper_format,
    const char* key,
    const char* data);

bool flipper_format_read_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t* data,
    const uint16_t data_size);

bool flipper_format_write_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size);

bool flipper_format_read_int32(
    FlipperFormat* flipper_format,
    const char* key,
    int32_t* data,
    const uint16_t data_size);

bool flipper_format_write_int32(
    FlipperFormat* flipper_format,
    const char* key,
    const int32_t* data,
    const uint16_t data_size);

bool flipper_format_read_float(
    FlipperFormat* flipper_format,
    const char* key,
    float* data,
    const uint16_t data_size);

bool flipper_format_write_float(
    FlipperFormat* flipper_format,
    const char* key,
    const float* data,
    const uint16_t data_size);

bool flipper_format_read_hex(
    FlipperFormat* flipper_format,
    const char* key,
    uint8_t* data,
    const uint16_t data_size);

bool flipper_format_write_hex(
    FlipperFormat* flipper_format,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size);

bool flipper_format_write_comment(FlipperFormat* flipper_format, string_t data);

bool flipper_format_write_comment_cstr(FlipperFormat* flipper_format, const char* data);

//

bool flipper_format_delete_key(FlipperFormat* flipper_format, const char* key);

bool flipper_format_update_string(FlipperFormat* flipper_format, const char* key, string_t data);

bool flipper_format_update_string_cstr(
    FlipperFormat* flipper_format,
    const char* key,
    const char* data);

bool flipper_format_update_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size);

bool flipper_format_update_int32(
    FlipperFormat* flipper_format,
    const char* key,
    const int32_t* data,
    const uint16_t data_size);

bool flipper_format_update_float(
    FlipperFormat* flipper_format,
    const char* key,
    const float* data,
    const uint16_t data_size);

bool flipper_format_update_hex(
    FlipperFormat* flipper_format,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size);

#ifdef __cplusplus
}
#endif