#include "bit_lib.h"
#include <furi/check.h>

void bit_lib_push_bit(uint8_t* data, size_t data_size, bool bit) {
    size_t last_index = data_size - 1;

    for(size_t i = 0; i < last_index; ++i) {
        data[i] = (data[i] << 1) | ((data[i + 1] >> 7) & 1);
    }
    data[last_index] = (data[last_index] << 1) | bit;
}

void bit_lib_set_bit(uint8_t* data, size_t position, bool bit) {
    if(bit) {
        data[position / 8] |= 1UL << (7 - (position % 8));
    } else {
        data[position / 8] &= ~(1UL << (7 - (position % 8)));
    }
}

void bit_lib_set_bits(uint8_t* data, size_t position, uint8_t byte, uint8_t length) {
    furi_check(length <= 8);
    furi_check(length > 0);

    for(uint8_t i = 0; i < length; ++i) {
        uint8_t shift = (length - 1) - i;
        bit_lib_set_bit(data, position + i, (byte >> shift) & 1);
    }
}

bool bit_lib_get_bit(const uint8_t* data, size_t position) {
    return (data[position / 8] >> (7 - (position % 8))) & 1;
}

uint8_t bit_lib_get_bits(const uint8_t* data, size_t position, uint8_t length) {
    uint8_t shift = position % 8;
    if(shift == 0) {
        return data[position / 8] >> (8 - length);
    } else {
        // TODO fix read out of bounds
        return ((data[position / 8] << (shift)) | (data[position / 8 + 1] >> (8 - shift))) >>
               (8 - length);
    }
}

bool bit_lib_test_parity_u32(uint32_t bits, BitLibParity parity) {
#if !defined __GNUC__
#error Please, implement parity test for non-GCC compilers
#else
    switch(parity) {
    case BitLibParityEven:
        return __builtin_parity(bits);
    case BitLibParityOdd:
        return !__builtin_parity(bits);
    default:
        furi_crash("Unknown parity");
    }
#endif
}