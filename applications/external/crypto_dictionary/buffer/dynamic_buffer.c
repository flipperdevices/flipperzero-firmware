#include "dynamic_buffer.h"

void dynamic_buffer_init(DynamicBuffer* buffer, size_t initial_capacity) {
    buffer->data = malloc(initial_capacity);
    buffer->size = 0;
    buffer->capacity = buffer->data ? initial_capacity : 0;
}

bool dynamic_buffer_grow(DynamicBuffer* buffer, size_t min_capacity) {
    size_t new_capacity = buffer->capacity > 0 ? buffer->capacity : 64;
    while(new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    char* new_data = realloc(buffer->data, new_capacity);
    if(!new_data) return false;

    buffer->data = new_data;
    buffer->capacity = new_capacity;
    return true;
}

bool dynamic_buffer_append(DynamicBuffer* buffer, const char* src, size_t src_size) {
    if(buffer->size + src_size > buffer->capacity) {
        if(!dynamic_buffer_grow(buffer, buffer->size + src_size)) {
            return false;
        }
    }
    memcpy(buffer->data + buffer->size, src, src_size);
    buffer->size += src_size;
    return true;
}

void dynamic_buffer_free(DynamicBuffer* buffer) {
    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
}