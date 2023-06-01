#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BitBuffer BitBuffer;

// Construction, deletion, reset
BitBuffer* bit_buffer_alloc(size_t capacity_bytes);

void bit_buffer_free(BitBuffer* buf);

void bit_buffer_reset(BitBuffer* buf);

// Copy
void bit_buffer_copy(BitBuffer* buf, const BitBuffer* other);

void bit_buffer_copy_right(BitBuffer* buf, const BitBuffer* other, size_t start);

void bit_buffer_copy_prepend_bytes(
    BitBuffer* buf,
    const BitBuffer* other,
    const uint8_t* prefix,
    size_t prefix_size_bytes);

void bit_buffer_copy_append_bytes(
    BitBuffer* buf,
    const BitBuffer* other,
    const uint8_t* suffix,
    size_t suffix_size_bytes);

void bit_buffer_copy_bytes(BitBuffer* buf, const uint8_t* data, size_t size_bytes);

void bit_buffer_write_bytes(const BitBuffer* buf, void* dest, size_t max_size_bytes);

// Tests
bool bit_buffer_has_partial_byte(const BitBuffer* buf);

bool bit_buffer_starts_with_byte(const BitBuffer* buf, uint8_t byte);

// Getters
size_t bit_buffer_get_capacity_bytes(const BitBuffer* buf);

size_t bit_buffer_get_size(const BitBuffer* buf);

size_t bit_buffer_get_size_bytes(const BitBuffer* buf);

uint8_t bit_buffer_get_byte(const BitBuffer* buf, size_t index);

uint8_t* bit_buffer_get_data(const BitBuffer* buf);

// Setters
void bit_buffer_set_byte(BitBuffer* buf, uint8_t byte, size_t index);

// Modification
void bit_buffer_append(BitBuffer* buf, const BitBuffer* other);

void bit_buffer_append_right(BitBuffer* buf, const BitBuffer* other, size_t start_index);

// TODO: Remove these temporary methods
void bit_buffer_set_size(BitBuffer* buf, size_t new_size);

#ifdef __cplusplus
}
#endif
