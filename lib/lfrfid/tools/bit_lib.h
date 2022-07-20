#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BitLibParityEven,
    BitLibParityOdd,
    BitLibParityAlways0,
    BitLibParityAlways1,
} BitLibParity;

/** @brief Increment and wrap around a value.
 *  @param index value to increment
 *  @param length wrap-around range
 */
#define bit_lib_increment_index(index, length) (index = (((index) + 1) % (length)))

/** @brief Test if a bit is set.
 *  @param data value to test
 *  @param index bit index to test
 */
#define bit_lib_bit_is_set(data, index) ((data & (1 << (index))) != 0)

/** @brief Test if a bit is not set.
 *  @param data value to test
 *  @param index bit index to test
 */
#define bit_lib_bit_is_not_set(data, index) ((data & (1 << (index))) == 0)

/** @brief Push a bit into a byte array.
 *  @param data array to push bit into
 *  @param data_size array size
 *  @param bit bit to push
 */
void bit_lib_push_bit(uint8_t* data, size_t data_size, bool bit);

/** @brief Set a bit in a byte array.
 *  @param data array to set bit in
 *  @param position The position of the bit to set.
 *  @param bit bit value to set
 */
void bit_lib_set_bit(uint8_t* data, size_t position, bool bit);

/** @brief Set the bit at the given position to the given value.
 * @param data The data to set the bit in.
 * @param position The position of the bit to set.
 * @param byte The data to set the bit to.
 * @param length The length of the data.
 */
void bit_lib_set_bits(uint8_t* data, size_t position, uint8_t byte, uint8_t length);

/** @brief Get the bit of a byte.
 * @param data The byte to get the bits from.
 * @param position The position of the bit.
 * @return The bit.
 */
bool bit_lib_get_bit(const uint8_t* data, size_t position);

/**
 * @brief Get the bits of a byte.
 * @param data The byte to get the bits from.
 * @param position The position of the first bit.
 * @param length The length of the bits.
 * @return The bits.
 */
uint8_t bit_lib_get_bits(const uint8_t* data, size_t position, uint8_t length);

uint16_t bit_lib_get_bits_16(const uint8_t* data, size_t position, uint8_t length);

uint32_t bit_lib_get_bits_32(const uint8_t* data, size_t position, uint8_t length);

/**
 * @brief Test parity of given bits
 * @param bits Bits to test parity of
 * @param parity Parity to test against
 * @return true if parity is correct, false otherwise
 */
bool bit_lib_test_parity_u32(uint32_t bits, BitLibParity parity);

bool bit_lib_test_parity(
    const uint8_t* bits,
    size_t position,
    uint8_t length,
    BitLibParity parity,
    uint8_t parity_length);

size_t bit_lib_remove_bit_every_nth(uint8_t* data, size_t position, uint8_t length, uint8_t n);

void bit_lib_copy_bits(
    uint8_t* data,
    size_t position,
    size_t length,
    const uint8_t* source,
    size_t source_position);

#ifdef __cplusplus
}
#endif