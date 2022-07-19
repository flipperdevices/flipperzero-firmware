#include "stream_buffer.h"

#include <furi/common_defines.h>

#define STREAM_BUFFER_MAX_SIZE 512U

struct StreamBuffer {
    uint8_t data[STREAM_BUFFER_MAX_SIZE];
    size_t data_size;
    size_t offset;
};

StreamBuffer* stream_buffer_alloc() {
    StreamBuffer* buffer = malloc(sizeof(StreamBuffer));
    buffer->data_size = 0;
    buffer->offset = 0;
    return buffer;
}
void stream_buffer_free(StreamBuffer* buffer) {
    furi_assert(buffer);
    buffer->data_size = 0;
    buffer->offset = 0;
    free(buffer);
}

void stream_buffer_reset(StreamBuffer* buffer) {
    buffer->data_size = 0;
    buffer->offset = 0;
}

bool stream_buffer_eof(StreamBuffer* buffer) {
    furi_assert(buffer->data_size >= buffer->offset);
    return buffer->data_size == buffer->offset;
}

size_t stream_buffer_size(StreamBuffer* buffer) {
    return buffer->data_size;
}

size_t stream_buffer_fill(StreamBuffer* buffer, Stream* stream) {
    const size_t size_read = stream_read(stream, buffer->data, STREAM_BUFFER_MAX_SIZE);
    buffer->data_size = size_read;
    buffer->offset = 0;
    return size_read;
}

size_t stream_buffer_read(StreamBuffer* buffer, uint8_t* data, size_t size) {
    furi_assert(buffer->data_size >= buffer->offset);
    const size_t size_read = MIN(size, buffer->data_size - buffer->offset);
    if(size_read > 0) {
        memcpy(data, buffer->data + buffer->offset, size_read);
        buffer->offset += size_read;
    }
    return size_read;
}

int32_t stream_buffer_seek(StreamBuffer* buffer, int32_t offset) {
    int32_t actual_offset = 0;

    if(offset > 0) {
        actual_offset = MIN(buffer->data_size - buffer->offset, (size_t)offset);
    } else if(offset < 0) {
        actual_offset = -MIN(buffer->offset, (size_t)abs(offset));
    }

    buffer->offset += actual_offset;
    return actual_offset;
}
