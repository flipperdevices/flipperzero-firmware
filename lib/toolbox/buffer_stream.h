#pragma once
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Buffer Buffer;

uint8_t* buffer_get_data(Buffer* buffer);

size_t buffer_get_size(Buffer* buffer);

void buffer_reset(Buffer* buffer);

typedef struct BufferStream BufferStream;

BufferStream* buffer_stream_alloc(size_t buffer_size, size_t buffers_count);

void buffer_stream_free(BufferStream* buffer_stream);

bool buffer_stream_send_from_isr(
    BufferStream* buffer_stream,
    const uint8_t* data,
    size_t size,
    BaseType_t* const task_woken);

Buffer* buffer_stream_receive(BufferStream* buffer_stream, TickType_t timeout);

size_t buffer_stream_get_overrun_count(BufferStream* buffer_stream);

void buffer_stream_reset(BufferStream* buffer_stream);

#ifdef __cplusplus
}
#endif
