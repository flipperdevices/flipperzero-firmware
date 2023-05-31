#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BitBuffer BitBuffer;

BitBuffer* bit_buffer_alloc(size_t size_bytes);

void bit_buffer_free(BitBuffer* buf);

void bit_buffer_reset(BitBuffer* buf);

void bit_buffer_prepend_cat(BitBuffer* buf, const BitBuffer* other, const uint8_t* prefix, size_t prefix_size);


// Temporary (?) direct access methods

uint8_t* bit_buffer_get_data(BitBuffer* buf);

#ifdef __cplusplus
}
#endif
