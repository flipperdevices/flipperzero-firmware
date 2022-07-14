#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define bit_lib_increment_index(index, length) (index = (((index) + 1) % (length)))

#define bit_lib_bit_is_set(data, index) ((data & (1 << (index))) == 1)

#define bit_lib_bit_is_not_set(data, index) ((data & (1 << (index))) == 0)

void bit_lib_push_bit(uint8_t* data, size_t data_size, bool bit);

void bit_lib_set_bit(uint8_t* data, size_t position, bool bit);

void bit_lib_set_bits(uint8_t* data, size_t position, uint8_t byte, uint8_t length);

bool bit_lib_get_bit(const uint8_t* data, size_t position);

uint8_t bit_lib_get_bits(const uint8_t* data, size_t position, uint8_t length);

#ifdef __cplusplus
}
#endif