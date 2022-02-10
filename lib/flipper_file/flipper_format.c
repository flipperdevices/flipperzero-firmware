#include <furi/check.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/string_stream.h>
#include "flipper_format.h"
#include "flipper_format_stream.h"

/* Private */
struct FlipperFormat {
    Stream* stream;
    bool strict_mode;
};

/********************************** Private **********************************/

static const char* flipper_format_filetype_key = "Filetype";
static const char* flipper_format_version_key = "Version";

/********************************** Public **********************************/

FlipperFormat* flipper_format_alloc_as_string() {
    FlipperFormat* flipper_format = malloc(sizeof(FlipperFormat));
    flipper_format->stream = string_stream_alloc();
    flipper_format->strict_mode = false;
    return flipper_format;
}

void flipper_format_free(FlipperFormat* flipper_format) {
    furi_assert(flipper_format);
    stream_free(flipper_format->stream);
    free(flipper_format);
}

bool flipper_format_rewind(FlipperFormat* flipper_format) {
    furi_assert(flipper_format);
    return stream_rewind(flipper_format->stream);
}

bool flipper_format_read_header(
    FlipperFormat* flipper_format,
    string_t filetype,
    uint32_t* version) {
    furi_assert(flipper_format);
    bool result = false;
    do {
        result = flipper_format_read_string(flipper_format, flipper_format_filetype_key, filetype);
        if(!result) break;
        result =
            flipper_format_read_uint32(flipper_format, flipper_format_version_key, version, 1);
        if(!result) break;
    } while(false);

    return result;
}

bool flipper_format_write_header(
    FlipperFormat* flipper_format,
    string_t filetype,
    const uint32_t version) {
    furi_assert(flipper_format);
    bool result = false;
    do {
        result =
            flipper_format_write_string(flipper_format, flipper_format_filetype_key, filetype);
        if(!result) break;
        result =
            flipper_format_write_uint32(flipper_format, flipper_format_version_key, &version, 1);
        if(!result) break;
    } while(false);

    return result;
}

bool flipper_format_write_header_cstr(
    FlipperFormat* flipper_format,
    const char* filetype,
    const uint32_t version) {
    furi_assert(flipper_format);
    bool result = false;
    do {
        result = flipper_format_write_string_cstr(
            flipper_format, flipper_format_filetype_key, filetype);
        if(!result) break;
        result =
            flipper_format_write_uint32(flipper_format, flipper_format_version_key, &version, 1);
        if(!result) break;
    } while(false);

    return result;
}

bool flipper_format_get_value_count(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t* count) {
    furi_assert(flipper_format);
    return flipper_format_stream_get_value_count(
        flipper_format->stream, key, count, flipper_format->strict_mode);
}

bool flipper_format_read_string(FlipperFormat* flipper_format, const char* key, string_t data) {
    furi_assert(flipper_format);
    return flipper_format_stream_read_value_line(
        flipper_format->stream, key, FlipperStreamValueStr, data, 1, flipper_format->strict_mode);
}

bool flipper_format_write_string(FlipperFormat* flipper_format, const char* key, string_t data) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueStr,
        .data = string_get_cstr(data),
        .data_size = 1,
    };
    bool result = flipper_format_stream_write_value_line(flipper_format->stream, &write_data);
    return result;
}

bool flipper_format_write_string_cstr(
    FlipperFormat* flipper_format,
    const char* key,
    const char* data) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueStr,
        .data = data,
        .data_size = 1,
    };
    bool result = flipper_format_stream_write_value_line(flipper_format->stream, &write_data);
    return result;
}

bool flipper_format_read_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_format);
    return flipper_format_stream_read_value_line(
        flipper_format->stream,
        key,
        FlipperStreamValueUint32,
        data,
        data_size,
        flipper_format->strict_mode);
}

bool flipper_format_write_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueUint32,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_write_value_line(flipper_format->stream, &write_data);
    return result;
}

bool flipper_format_read_int32(
    FlipperFormat* flipper_format,
    const char* key,
    int32_t* data,
    const uint16_t data_size) {
    return flipper_format_stream_read_value_line(
        flipper_format->stream,
        key,
        FlipperStreamValueInt32,
        data,
        data_size,
        flipper_format->strict_mode);
}

bool flipper_format_write_int32(
    FlipperFormat* flipper_format,
    const char* key,
    const int32_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueInt32,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_write_value_line(flipper_format->stream, &write_data);
    return result;
}

bool flipper_format_read_float(
    FlipperFormat* flipper_format,
    const char* key,
    float* data,
    const uint16_t data_size) {
    return flipper_format_stream_read_value_line(
        flipper_format->stream,
        key,
        FlipperStreamValueFloat,
        data,
        data_size,
        flipper_format->strict_mode);
}

bool flipper_format_write_float(
    FlipperFormat* flipper_format,
    const char* key,
    const float* data,
    const uint16_t data_size) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueFloat,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_write_value_line(flipper_format->stream, &write_data);
    return result;
}

bool flipper_format_read_hex(
    FlipperFormat* flipper_format,
    const char* key,
    uint8_t* data,
    const uint16_t data_size) {
    return flipper_format_stream_read_value_line(
        flipper_format->stream,
        key,
        FlipperStreamValueHex,
        data,
        data_size,
        flipper_format->strict_mode);
}

bool flipper_format_write_hex(
    FlipperFormat* flipper_format,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueHex,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_write_value_line(flipper_format->stream, &write_data);
    return result;
}

bool flipper_format_write_comment(FlipperFormat* flipper_format, string_t data) {
    furi_assert(flipper_format);
    return flipper_format_write_comment_cstr(flipper_format, string_get_cstr(data));
}

bool flipper_format_write_comment_cstr(FlipperFormat* flipper_format, const char* data) {
    furi_assert(flipper_format);
    return flipper_format_stream_write_comment_cstr(flipper_format->stream, data);
}

bool flipper_format_delete_key(FlipperFormat* flipper_format, const char* key) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueIgnore,
        .data = NULL,
        .data_size = 0,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}

bool flipper_format_update_string(FlipperFormat* flipper_format, const char* key, string_t data) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueStr,
        .data = data,
        .data_size = 1,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}

bool flipper_format_update_string_cstr(
    FlipperFormat* flipper_format,
    const char* key,
    const char* data) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueStr,
        .data = data,
        .data_size = 1,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}

bool flipper_format_update_uint32(
    FlipperFormat* flipper_format,
    const char* key,
    const uint32_t* data,
    const uint16_t data_size) {
    furi_assert(flipper_format);
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueUint32,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}

bool flipper_format_update_int32(
    FlipperFormat* flipper_format,
    const char* key,
    const int32_t* data,
    const uint16_t data_size) {
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueInt32,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}

bool flipper_format_update_float(
    FlipperFormat* flipper_format,
    const char* key,
    const float* data,
    const uint16_t data_size) {
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueFloat,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}

bool flipper_format_update_hex(
    FlipperFormat* flipper_format,
    const char* key,
    const uint8_t* data,
    const uint16_t data_size) {
    FlipperStreamWriteData write_data = {
        .key = key,
        .type = FlipperStreamValueHex,
        .data = data,
        .data_size = data_size,
    };
    bool result = flipper_format_stream_delete_key_and_write(
        flipper_format->stream, &write_data, flipper_format->strict_mode);
    return result;
}