#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include "../app/app.h"

typedef struct {
    char* data;
    size_t size;
    size_t capacity;
} DynamicBuffer;


void dynamic_buffer_init(DynamicBuffer* buffer, size_t initial_capacity);
bool dynamic_buffer_grow(DynamicBuffer* buffer, size_t min_capacity);
bool dynamic_buffer_append(DynamicBuffer* buffer, const char* src, size_t src_size);
void dynamic_buffer_free(DynamicBuffer* buffer);

#endif // DYNAMIC_BUFFER_H