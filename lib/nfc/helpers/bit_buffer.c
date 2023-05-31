#include "bit_buffer.h"

#include <furi.h>

#define BITS_IN_BYTE (8)

struct BitBuffer {
    uint8_t* data;
    uint8_t* parity;
    size_t capacity_bytes;
    size_t data_size_bits;
};

BitBuffer* bit_buffer_alloc(size_t size_bytes) {
    furi_assert(size_bytes);

    BitBuffer* buf = malloc(sizeof(BitBuffer));

    buf->data = malloc(size_bytes);
    buf->parity = malloc((size_bytes + BITS_IN_BYTE - 1) / BITS_IN_BYTE);
    buf->capacity_bytes = size_bytes;
    buf->data_size_bits = 0;

    return buf;
}

void bit_buffer_free(BitBuffer* buf) {
    furi_assert(buf);

    free(buf->data);
    free(buf->parity);
    free(buf);
}

void bit_buffer_reset(BitBuffer* buf) {
    furi_assert(buf);

    memset(buf->data, 0, buf->capacity_bytes);
    memset(buf->parity, 0, (buf->capacity_bytes + BITS_IN_BYTE - 1) / BITS_IN_BYTE);

    buf->data_size_bits = 0;
}

void bit_buffer_prepend_cat(BitBuffer* buf, const BitBuffer* other, const uint8_t* prefix, size_t prefix_size) {
    furi_assert(buf->capacity_bytes >= (other->capacity_bytes + prefix_size));

    memcpy(buf->data, prefix, prefix_size);
    memcpy(buf->data + prefix_size, other->data, other->data_size_bits / BITS_IN_BYTE);

    buf->data_size_bits = other->data_size_bits + prefix_size * BITS_IN_BYTE;
}

uint8_t* bit_buffer_get_data(BitBuffer* buf) {
    furi_assert(buf);

    return buf->data;
}
