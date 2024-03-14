#include "uhf_buffer.h"
#include <stdlib.h>
#include <string.h>

Buffer* uhf_buffer_alloc(size_t initial_capacity) {
    Buffer* buf = (Buffer*)malloc(sizeof(Buffer));
    buf->data = (uint8_t*)malloc(sizeof(uint8_t) * initial_capacity);
    if(!buf->data) {
        free(buf);
        return NULL;
    }
    buf->size = 0;
    buf->capacity = initial_capacity;
    return buf;
}

bool uhf_buffer_append_single(Buffer* buf, uint8_t data) {
    if(buf->closed) return false;
    if(buf->size + 1 > buf->capacity) {
        size_t new_capacity = buf->capacity * 2;
        uint8_t* new_data = (uint8_t*)realloc(buf->data, sizeof(uint8_t) * new_capacity);
        if(!new_data) return false;
        buf->data = new_data;
        buf->capacity = new_capacity;
    }
    buf->data[buf->size++] = data;
    return true;
}

bool uhf_buffer_append(Buffer* buf, uint8_t* data, size_t data_size) {
    if(buf->closed) return false;
    if(buf->size + data_size > buf->capacity) {
        size_t new_capacity = buf->capacity * 2;
        uint8_t* new_data = (uint8_t*)realloc(buf->data, new_capacity);
        if(!new_data) return false;

        buf->data = new_data;
        buf->capacity = new_capacity;
    }

    memcpy((void*)&buf->data[buf->size], data, data_size);
    buf->size += data_size;
    return true;
}

uint8_t* uhf_buffer_get_data(Buffer* buf) {
    return buf->data;
}

size_t uhf_buffer_get_size(Buffer* buf) {
    return buf->size;
}

bool uhf_is_buffer_closed(Buffer* buf) {
    return buf->closed;
}

void uhf_buffer_close(Buffer* buf) {
    buf->closed = true;
}

void uhf_buffer_reset(Buffer* buf) {
    for(size_t i = 0; i < MAX_BUFFER_SIZE; i++) {
        buf->data[i] = 0;
    }
    buf->size = 0;
    buf->closed = false;
}

void uhf_buffer_free(Buffer* buf) {
    free(buf->data);
    free(buf);
}