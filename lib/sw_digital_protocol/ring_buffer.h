#pragma once
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RingBuffer RingBuffer;

RingBuffer* ring_buffer_alloc(uint32_t size);
void ring_buffer_free(RingBuffer* ring_buffer);
void ring_buffer_reset(RingBuffer* ring_buffer);
void ring_buffer_write_byte(RingBuffer* ring_buffer, uint8_t data);
bool ring_buffer_write(RingBuffer* ring_buffer, uint8_t* data, size_t size);
bool ring_buffer_read_byte(RingBuffer* ring_buffer, uint8_t* data);
size_t ring_buffer_read(RingBuffer* ring_buffer, uint8_t* data, size_t size);
size_t ring_buffer_spaces_available(RingBuffer* ring_buffer);
size_t ring_buffer_available(RingBuffer* ring_buffer);

#ifdef __cplusplus
}
#endif