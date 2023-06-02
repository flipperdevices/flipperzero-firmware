#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BitBuffer BitBuffer;

// Construction, deletion, reset

/**
 * Allocate a BitBuffer instance.
 *
 * @param [in] capacity_bytes maximum buffer capacity, in bytes
 * @return pointer to the allocated BitBuffer instance
 */
BitBuffer* bit_buffer_alloc(size_t capacity_bytes);

/**
 * Delete a BitBuffer instance.
 *
 * @param [in,out] buf pointer to a BitBuffer instance
 */
void bit_buffer_free(BitBuffer* buf);

/**
 * Clear all data from a BitBuffer instance.
 *
 * @param [in,out] buf pointer to a BitBuffer instance
 */
void bit_buffer_reset(BitBuffer* buf);

// Copy and write

/**
 * Copy another BitBuffer instance's contents to this one, replacing
 * all of the original data.
 * The destination capacity must be no less than the source data size.
 *
 * @param [in,out] buf pointer to a BitBuffer instance to copy into
 * @param [in] other pointer to a BitBuffer instance to copy from
 * @note
 */
void bit_buffer_copy(BitBuffer* buf, const BitBuffer* other);

/**
 * Copy all BitBuffer instance's contents to this one, starting from start_index,
 * replacing all of the original data.
 * The destination capacity must be no less than the source data size
 * counting from start_index.
 *
 * @param [in,out] buf pointer to a BitBuffer instance to copy into
 * @param [in] other pointer to a BitBuffer instance to copy from
 * @param [in] start_index index to begin copying source data from
 */
void bit_buffer_copy_right(BitBuffer* buf, const BitBuffer* other, size_t start_index);

/**
 * Copy a byte array to a BitBuffer instance, replacing all of the original data.
 * The destination capacity must be no less than the source data size.
 *
 * @param [in,out] buf pointer to a BitBuffer instance to copy into
 * @param [in] data pointer to the byte array to be copied
 * @param [in] size_bytes size of the data to be copied, in bytes
 */
void bit_buffer_copy_bytes(BitBuffer* buf, const uint8_t* data, size_t size_bytes);

/**
 * Write a BitBuffer instance's contents to an arbitrary memory location.
 * The destination memory must be allocated. Additionally, the instance
 * must contain at least the requested amount of data (for easier underflow detection).
 *
 * @param [in] buf pointer to a BitBuffer instance to write from
 * @param [out] dest pointer to the destination memory location
 * @param [in] size_bytes size of the data to be written, in bytes
 */
void bit_buffer_write_bytes(const BitBuffer* buf, void* dest, size_t size_bytes);

// Checks

/**
 * Check whether a BitBuffer instance contains a partial byte (i.e. the bit count
 * is not divisible by 8).
 *
 * @param [in] buf pointer to a BitBuffer instance to be checked
 * @return true if the instance contains a partial byte, false otherwise
 */
bool bit_buffer_has_partial_byte(const BitBuffer* buf);

/**
 * Check whether a BitBuffer instance's contents start with the designated byte.
 *
 * @param [in] buf pointer to a BitBuffer instance to be checked
 * @param [in] byte byte value to be checked against
 * @return true if data starts with designated byte, false otherwise
 */
bool bit_buffer_starts_with_byte(const BitBuffer* buf, uint8_t byte);

// Getters

/**
 * Get a BitBuffer instance's capacity (i.e. the maximum possible amount of data), in bytes.
 *
 * @param [in] buf pointer to a BitBuffer instance to be queried
 * @return capacity, in bytes
 */
size_t bit_buffer_get_capacity_bytes(const BitBuffer* buf);

/**
 * Get a BitBuffer instance's data size (i.e. the amount of stored data), in bits.
 * Might be not divisible by 8 (see bit_buffer_is_partial_byte).
 *
 * @param [in] buf pointer to a BitBuffer instance to be queried
 * @return data size, in bits.
 */
size_t bit_buffer_get_size(const BitBuffer* buf);

/**
 * Get a BitBuffer instance's data size (i.e. the amount of stored data), in bytes.
 * If a partial byte is present, it is also counted.
 *
 * @param [in] buf pointer to a BitBuffer instance to be queried
 * @return data size, in bytes.
 */
size_t bit_buffer_get_size_bytes(const BitBuffer* buf);

/**
 * Get a byte value at a specified index in a BitBuffer instance.
 * The index must be valid (i.e. less than the instance's data size in bytes).
 *
 * @param [in] buf pointer to a BitBuffer instance to be queried
 * @param [in] index index of the byte in question
 */
uint8_t bit_buffer_get_byte(const BitBuffer* buf, size_t index);

/**
 * Get the pointer to a BitBuffer instance's underlying data. Use sparingly.
 * TODO: Make it return const uint8_t* after all compatibility problems
 * will have been resolved.
 *
 * @param [in] buf pointer to a BitBuffer instance to be queried
 * @return pointer to the underlying data
 */
uint8_t* bit_buffer_get_data(const BitBuffer* buf);

// Setters

/**
 * Set byte value at a specified index in a BitBuffer instance.
 * The index must be valid (i.e. less than the instance's data size in bytes).
 *
 * @param [in,out] buf pointer to a BitBuffer instance to be modified
 * @param [in] byte byte value to be set at index
 * @param [in] index index of the byte in question
 */
void bit_buffer_set_byte(BitBuffer* buf, uint8_t byte, size_t index);

// Modification

/**
 * Append all BitBuffer's instance contents to this one. The destination capacity
 * must be no less than its original data size plus source data size.
 *
 * @param [in,out] buf pointer to a BitBuffer instance to be appended to
 * @param [in] other pointer to a BitBuffer instance to be appended
 */
void bit_buffer_append(BitBuffer* buf, const BitBuffer* other);

/**
 * Append a BitBuffer's instance contents to this one, starting from start_index.
 * The destination capacity must be no less than the source data size
 * counting from start_index.
 *
 * @param [in,out] buf pointer to a BitBuffer instance to be appended to
 * @param [in] other pointer to a BitBuffer instance to be appended
 * @param [in] start_index index to begin copying source data from
 */
void bit_buffer_append_right(BitBuffer* buf, const BitBuffer* other, size_t start_index);

/**
 * Override the BitBuffer instance's data size and replace it with the new value, in bits.
 * @warning This method exists for compatibility purposes only. It will be removed
 * once the other APIs will allow for it.
 *
 * @param [in,out] buf pointer to a BitBuffer instance to be modified
 * @param [in] new_size the new size of the buffer, in bits
 */
void bit_buffer_set_size(BitBuffer* buf, size_t new_size);

#ifdef __cplusplus
}
#endif
