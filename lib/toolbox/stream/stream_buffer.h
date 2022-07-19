#pragma once

#include "stream.h"

typedef struct StreamBuffer StreamBuffer;

StreamBuffer* stream_buffer_alloc();
void stream_buffer_free(StreamBuffer* buffer);

void stream_buffer_reset(StreamBuffer* buffer);

bool stream_buffer_eof(StreamBuffer* buffer);
size_t stream_buffer_size(StreamBuffer* buffer);

size_t stream_buffer_fill(StreamBuffer* buffer, Stream* stream);
size_t stream_buffer_read(StreamBuffer* buffer, uint8_t* data, size_t size);
int32_t stream_buffer_seek(StreamBuffer* buffer, int32_t offset);
