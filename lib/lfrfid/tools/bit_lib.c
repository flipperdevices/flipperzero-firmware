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
        uint8_t value = (data[position / 8] << (shift));
        value |= data[position / 8 + 1] >> (8 - shift);
        value = value >> (8 - length);
        return value;
    }
}

uint16_t bit_lib_get_bits_16(const uint8_t* data, size_t position, uint8_t length) {
    uint16_t value = 0;
    if(length <= 8) {
        value = bit_lib_get_bits(data, position, length);
    } else {
        value = bit_lib_get_bits(data, position, 8) << (length - 8);
        value |= bit_lib_get_bits(data, position + 8, length - 8);
    }
    return value;
}

uint32_t bit_lib_get_bits_32(const uint8_t* data, size_t position, uint8_t length) {
    uint32_t value = 0;
    if(length <= 8) {
        value = bit_lib_get_bits(data, position, length);
    } else if(length <= 16) {
        value = bit_lib_get_bits(data, position, 8) << (length - 8);
        value |= bit_lib_get_bits(data, position + 8, length - 8);
    } else if(length <= 24) {
        value = bit_lib_get_bits(data, position, 8) << (length - 8);
        value |= bit_lib_get_bits(data, position + 8, 8) << (length - 16);
        value |= bit_lib_get_bits(data, position + 16, length - 16);
    } else {
        value = bit_lib_get_bits(data, position, 8) << (length - 8);
        value |= bit_lib_get_bits(data, position + 8, 8) << (length - 16);
        value |= bit_lib_get_bits(data, position + 16, 8) << (length - 24);
        value |= bit_lib_get_bits(data, position + 24, length - 24);
    }

    return value;
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

bool bit_lib_test_parity(
    const uint8_t* bits,
    size_t position,
    uint8_t length,
    BitLibParity parity,
    uint8_t parity_length) {
    uint32_t parity_block;
    bool result = true;
    const size_t parity_blocks_count = length / parity_length;

    for(size_t i = 0; i < parity_blocks_count; ++i) {
        switch(parity) {
        case BitLibParityEven:
        case BitLibParityOdd:
            parity_block = bit_lib_get_bits_32(bits, position + i * parity_length, parity_length);
            if(!bit_lib_test_parity_u32(parity_block, parity)) {
                result = false;
            }
            break;
        case BitLibParityAlways0:
            if(bit_lib_get_bit(bits, position + i * parity_length + parity_length - 1)) {
                result = false;
            }
            break;
        case BitLibParityAlways1:
            if(!bit_lib_get_bit(bits, position + i * parity_length + parity_length - 1)) {
                result = false;
            }
            break;
        }

        if(!result) break;
    }
    return result;
}

size_t bit_lib_remove_bit_every_nth(uint8_t* data, size_t position, uint8_t length, uint8_t n) {
    size_t counter = 0;
    size_t result_counter = 0;
    uint8_t bit_buffer = 0;
    uint8_t bit_counter = 0;

    while(counter < length) {
        if((counter + 1) % n != 0) {
            bit_buffer = (bit_buffer << 1) | bit_lib_get_bit(data, position + counter);
            bit_counter++;
        }

        if(bit_counter == 8) {
            bit_lib_set_bits(data, position + result_counter, bit_buffer, 8);
            bit_counter = 0;
            bit_buffer = 0;
            result_counter += 8;
        }
        counter++;
    }

    if(bit_counter != 0) {
        bit_lib_set_bits(data, position + result_counter, bit_buffer, bit_counter);
        result_counter += bit_counter;
    }
    return result_counter;
}

void bit_lib_copy_bits(
    uint8_t* data,
    size_t position,
    size_t length,
    const uint8_t* source,
    size_t source_position) {
    for(size_t i = 0; i < length; ++i) {
        bit_lib_set_bit(data, position + i, bit_lib_get_bit(source, source_position + i));
    }
}

#include <stdio.h>
void bit_lib_reverse_bits(uint8_t* data, size_t position, uint8_t length) {
    size_t i = 0;
    size_t j = length - 1;

    while(i < j) {
        bool tmp = bit_lib_get_bit(data, position + i);
        bit_lib_set_bit(data, position + i, bit_lib_get_bit(data, position + j));
        bit_lib_set_bit(data, position + j, tmp);
        i++;
        j--;
    }
}

uint8_t bit_lib_get_bit_count(uint32_t data) {
#if defined __GNUC__
    return __builtin_popcountl(data);
#else
#error Please, implement popcount for non-GCC compilers
#endif
}