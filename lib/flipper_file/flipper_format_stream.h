#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <toolbox/stream/stream.h>
#include <mlib/m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FlipperStreamValueIgnore,
    FlipperStreamValueStr,
    FlipperStreamValueHex,
    FlipperStreamValueFloat,
    FlipperStreamValueInt32,
    FlipperStreamValueUint32,
} FlipperStreamValue;

typedef struct {
    const char* key;
    FlipperStreamValue type;
    const void* data;
    size_t data_size;
} FlipperStreamWriteData;

bool flipper_format_stream_write(Stream* stream, const void* data, size_t data_size);

bool flipper_format_stream_write_key(Stream* stream, const char* key);

bool flipper_format_stream_write_eol(Stream* stream);

bool flipper_format_stream_read_valid_key(Stream* stream, string_t key);

bool flipper_format_stream_seek_to_key(Stream* stream, const char* key, bool strict_mode);

bool flipper_format_stream_read_value(Stream* stream, string_t value, bool* last);

bool flipper_format_stream_read_line(Stream* stream, string_t str_result);

bool flipper_format_stream_seek_to_next_line(Stream* stream);

bool flipper_format_stream_write_value_line(Stream* stream, FlipperStreamWriteData* write_data);

bool flipper_format_stream_read_value_line(
    Stream* stream,
    const char* key,
    FlipperStreamValue type,
    void* _data,
    size_t data_size,
    bool strict_mode);

bool flipper_format_stream_get_value_count(
    Stream* stream,
    const char* key,
    uint32_t* count,
    bool strict_mode);

bool flipper_format_stream_delete_key_and_write(
    Stream* stream,
    FlipperStreamWriteData* write_data,
    bool strict_mode);

bool flipper_format_stream_write_comment_cstr(Stream* stream, const char* data);

#ifdef __cplusplus
}
#endif