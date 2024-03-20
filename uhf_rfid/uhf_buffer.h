#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE 200

typedef struct Buffer {
    uint8_t* data;
    size_t size;
    size_t capacity;
    bool closed;
} Buffer;

Buffer* uhf_buffer_alloc(size_t inital_capacity);
bool uhf_buffer_append_single(Buffer* buf, uint8_t value);
bool uhf_buffer_append(Buffer* buf, uint8_t* data, size_t size);

uint8_t* uhf_buffer_get_data(Buffer* buf);
size_t uhf_buffer_get_size(Buffer* buf);
bool uhf_is_buffer_closed(Buffer* buf);
void uhf_buffer_close(Buffer* buf);
void uhf_buffer_reset(Buffer* buf);
void uhf_buffer_free(Buffer* buf);