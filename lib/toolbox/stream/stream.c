#include "stream.h"
#include "stream_i.h"

void stream_free(Stream* stream) {
    stream->vtable->free(stream);
}

void stream_clean(Stream* stream) {
    stream->vtable->clean(stream);
}

bool stream_eof(Stream* stream) {
    return stream->vtable->eof(stream);
}

bool stream_seek(Stream* stream, int32_t offset, StreamOffset offset_type) {
    return stream->vtable->seek(stream, offset, offset_type);
}

size_t stream_tell(Stream* stream) {
    return stream->vtable->tell(stream);
}

size_t stream_size(Stream* stream) {
    return stream->vtable->size(stream);
}

size_t stream_write(Stream* stream, const uint8_t* data, size_t size) {
    return stream->vtable->write(stream, data, size);
}

size_t stream_read(Stream* stream, uint8_t* data, size_t size) {
    return stream->vtable->read(stream, data, size);
}

int32_t stream_delete_and_insert(
    Stream* stream,
    size_t delete_size,
    StreamWriteCB write_callback,
    const void* ctx) {
    return stream->vtable->delete_and_insert(stream, delete_size, write_callback, ctx);
}

/****** Some random helpers starts here ******/

void stream_rewind(Stream* stream) {
    stream_seek(stream, 0, StreamOffsetFromStart);
}

size_t stream_write_char(Stream* stream, char c) {
    return stream_write(stream, &c, 1);
}

size_t stream_write_string(Stream* stream, string_t string) {
    return stream_write(stream, string_get_cstr(string), string_size(string));
}

size_t stream_write_cstring(Stream* stream, const char* string) {
    return stream_write(stream, string, strlen(string));
}

size_t stream_write_format(Stream* stream, const char* format, ...) {
    size_t size;

    va_list args;
    va_start(args, format);
    size = stream_write_vaformat(stream, format, args);
    va_end(args);

    return size;
}

size_t stream_write_vaformat(Stream* stream, const char* format, va_list args) {
    string_t data;
    string_init_vprintf(data, format, args);
    size_t size = stream_write_string(stream, data);
    string_clear(data);

    return size;
}

typedef struct {
    const char* data;
    size_t size;
} StreamWriteData;

static size_t stream_write_struct(Stream* stream, const void* context) {
    const StreamWriteData* write_data = context;
    return stream_write(stream, write_data->data, write_data->size);
}

size_t stream_insert(Stream* stream, const char* data, size_t size) {
    size_t was_written = 0;

    StreamWriteData write_data = {.data = data, .size = size};
    was_written = stream_delete_and_insert(stream, 0, stream_write_struct, &write_data);

    return was_written;
}

size_t stream_delete(Stream* stream, size_t size) {
    size_t result = -stream_delete_and_insert(stream, size, NULL, NULL);
    return result;
}