#include "stream.h"
#include "stream_i.h"
#include <furi/check.h>

void stream_free(Stream* stream) {
    furi_assert(stream);
    stream->vtable->free(stream);
}

void stream_clean(Stream* stream) {
    furi_assert(stream);
    stream->vtable->clean(stream);
}

bool stream_eof(Stream* stream) {
    furi_assert(stream);
    return stream->vtable->eof(stream);
}

bool stream_seek(Stream* stream, int32_t offset, StreamOffset offset_type) {
    furi_assert(stream);
    return stream->vtable->seek(stream, offset, offset_type);
}

size_t stream_tell(Stream* stream) {
    furi_assert(stream);
    return stream->vtable->tell(stream);
}

size_t stream_size(Stream* stream) {
    furi_assert(stream);
    return stream->vtable->size(stream);
}

size_t stream_write(Stream* stream, const uint8_t* data, size_t size) {
    furi_assert(stream);
    return stream->vtable->write(stream, data, size);
}

size_t stream_read(Stream* stream, uint8_t* data, size_t size) {
    furi_assert(stream);
    return stream->vtable->read(stream, data, size);
}

int32_t stream_delete_and_insert(
    Stream* stream,
    size_t delete_size,
    StreamWriteCB write_callback,
    const void* ctx) {
    furi_assert(stream);
    return stream->vtable->delete_and_insert(stream, delete_size, write_callback, ctx);
}

/********************************** Some random helpers starts here **********************************/

bool stream_rewind(Stream* stream) {
    furi_assert(stream);
    return stream_seek(stream, 0, StreamOffsetFromStart);
}

size_t stream_write_char(Stream* stream, char c) {
    furi_assert(stream);
    return stream_write(stream, (uint8_t*)&c, 1);
}

size_t stream_write_string(Stream* stream, string_t string) {
    furi_assert(stream);
    return stream_write(stream, (uint8_t*)string_get_cstr(string), string_size(string));
}

size_t stream_write_cstring(Stream* stream, const char* string) {
    furi_assert(stream);
    return stream_write(stream, (uint8_t*)string, strlen(string));
}

size_t stream_write_format(Stream* stream, const char* format, ...) {
    furi_assert(stream);
    size_t size;
    va_list args;
    va_start(args, format);
    size = stream_write_vaformat(stream, format, args);
    va_end(args);
    return size;
}

size_t stream_write_vaformat(Stream* stream, const char* format, va_list args) {
    furi_assert(stream);
    string_t data;
    string_init_vprintf(data, format, args);
    size_t size = stream_write_string(stream, data);
    string_clear(data);

    return size;
}

typedef struct {
    const uint8_t* data;
    size_t size;
} StreamWriteData;

static size_t stream_write_struct(Stream* stream, const void* context) {
    furi_assert(stream);
    furi_assert(context);
    const StreamWriteData* write_data = context;
    return stream_write(stream, write_data->data, write_data->size);
}

size_t stream_insert(Stream* stream, const uint8_t* data, size_t size) {
    furi_assert(stream);
    size_t was_written = 0;
    StreamWriteData write_data = {.data = data, .size = size};
    was_written = stream_delete_and_insert(stream, 0, stream_write_struct, &write_data);
    return was_written;
}

size_t stream_insert_char(Stream* stream, char c) {
    furi_assert(stream);
    size_t was_written = 0;
    was_written = stream_delete_and_insert_char(stream, 0, c);
    return was_written;
}

size_t stream_insert_string(Stream* stream, string_t string) {
    furi_assert(stream);
    size_t was_written = stream_delete_and_insert_string(stream, 0, string);
    return was_written;
}

size_t stream_insert_cstring(Stream* stream, const char* string) {
    furi_assert(stream);
    size_t was_written = stream_delete_and_insert_cstring(stream, 0, string);
    return was_written;
}

size_t stream_insert_format(Stream* stream, const char* format, ...) {
    furi_assert(stream);
    va_list args;
    va_start(args, format);
    size_t was_written = stream_insert_vaformat(stream, format, args);
    va_end(args);

    return was_written;
}

size_t stream_insert_vaformat(Stream* stream, const char* format, va_list args) {
    furi_assert(stream);
    size_t was_written = stream_delete_and_insert_vaformat(stream, 0, format, args);
    return was_written;
}

int32_t stream_delete_and_insert_char(Stream* stream, size_t delete_size, char c) {
    furi_assert(stream);
    int32_t was_written;
    StreamWriteData write_data = {.data = (uint8_t*)&c, .size = 1};
    was_written = stream_delete_and_insert(stream, delete_size, stream_write_struct, &write_data);
    return was_written;
}

int32_t stream_delete_and_insert_string(Stream* stream, size_t delete_size, string_t string) {
    furi_assert(stream);
    int32_t was_written;
    StreamWriteData write_data = {
        .data = (uint8_t*)string_get_cstr(string), .size = string_size(string)};
    was_written = stream_delete_and_insert(stream, delete_size, stream_write_struct, &write_data);
    return was_written;
}

int32_t stream_delete_and_insert_cstring(Stream* stream, size_t delete_size, const char* string) {
    furi_assert(stream);
    int32_t was_written;
    StreamWriteData write_data = {.data = (uint8_t*)string, .size = strlen(string)};
    was_written = stream_delete_and_insert(stream, delete_size, stream_write_struct, &write_data);
    return was_written;
}

int32_t
    stream_delete_and_insert_format(Stream* stream, size_t delete_size, const char* format, ...) {
    furi_assert(stream);
    int32_t was_written;
    va_list args;
    va_start(args, format);
    was_written = stream_delete_and_insert_vaformat(stream, delete_size, format, args);
    va_end(args);

    return was_written;
}

int32_t stream_delete_and_insert_vaformat(
    Stream* stream,
    size_t delete_size,
    const char* format,
    va_list args) {
    furi_assert(stream);
    size_t was_written = 0;
    string_t data;
    string_init_vprintf(data, format, args);
    StreamWriteData write_data = {
        .data = (uint8_t*)string_get_cstr(data), .size = string_size(data)};
    was_written = stream_delete_and_insert(stream, 0, stream_write_struct, &write_data);
    string_clear(data);

    return was_written;
}

size_t stream_delete(Stream* stream, size_t size) {
    furi_assert(stream);
    size_t result = -stream_delete_and_insert(stream, size, NULL, NULL);
    return result;
}