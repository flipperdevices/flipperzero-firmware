#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE 128

typedef struct Buffer {
    uint8_t* data;
    size_t size;
    size_t capacity;
    bool closed;
} Buffer;

Buffer* buffer_alloc(size_t inital_capacity);
bool buffer_append_single(Buffer* buf, uint8_t value);
bool buffer_append(Buffer* buf, uint8_t* data, size_t size);
uint8_t* buffer_get_data(Buffer* buf);
size_t buffer_get_size(Buffer* buf);
void buffer_close(Buffer* buf);
void buffer_reset(Buffer* buf);
void buffer_free(Buffer* buf);