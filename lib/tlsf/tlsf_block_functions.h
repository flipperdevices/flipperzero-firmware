/*
 * SPDX-FileCopyrightText: 2006-2016 Matthew Conte
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include "tlsf_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*
** block_header_t member functions.
*/

static inline __attribute__((always_inline)) size_t block_size(const block_header_t* block)
{
	return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}

static inline __attribute__((always_inline)) void block_set_size(block_header_t* block, size_t size)
{
	const size_t oldsize = block->size;
	block->size = size | (oldsize & (block_header_free_bit | block_header_prev_free_bit));
}

static inline __attribute__((always_inline)) int block_is_last(const block_header_t* block)
{
	return block_size(block) == 0;
}

static inline __attribute__((always_inline)) int block_is_free(const block_header_t* block)
{
	return tlsf_cast(int, block->size & block_header_free_bit);
}

static inline __attribute__((always_inline)) void block_set_free(block_header_t* block)
{
	block->size |= block_header_free_bit;
}

static inline __attribute__((always_inline)) void block_set_used(block_header_t* block)
{
	block->size &= ~block_header_free_bit;
}

static inline __attribute__((always_inline)) int block_is_prev_free(const block_header_t* block)
{
	return tlsf_cast(int, block->size & block_header_prev_free_bit);
}

static inline __attribute__((always_inline)) void block_set_prev_free(block_header_t* block)
{
	block->size |= block_header_prev_free_bit;
}

static inline __attribute__((always_inline)) void block_set_prev_used(block_header_t* block)
{
	block->size &= ~block_header_prev_free_bit;
}

static inline __attribute__((always_inline)) block_header_t* block_from_ptr(const void* ptr)
{
	return tlsf_cast(block_header_t*,
		tlsf_cast(unsigned char*, ptr) - block_start_offset);
}

static inline __attribute__((always_inline)) void* block_to_ptr(const block_header_t* block)
{
	return tlsf_cast(void*,
		tlsf_cast(unsigned char*, block) + block_start_offset);
}

/* Return location of next block after block of given size. */
static inline __attribute__((always_inline)) block_header_t* offset_to_block(const void* ptr, size_t size)
{
	return tlsf_cast(block_header_t*, tlsf_cast(tlsfptr_t, ptr) + size);
}

/* Return location of previous block. */
static inline __attribute__((always_inline)) block_header_t* block_prev(const block_header_t* block)
{
	tlsf_assert(block_is_prev_free(block) && "previous block must be free");
	return block->prev_phys_block;
}

/* Return location of next existing block. */
static inline __attribute__((always_inline)) block_header_t* block_next(const block_header_t* block)
{
	block_header_t* next = offset_to_block(block_to_ptr(block),
		block_size(block) - block_header_overhead);
	tlsf_assert(!block_is_last(block));
	return next;
}

/* Link a new block with its physical neighbor, return the neighbor. */
static inline __attribute__((always_inline)) block_header_t* block_link_next(block_header_t* block)
{
	block_header_t* next = block_next(block);
	next->prev_phys_block = block;
	return next;
}

static inline __attribute__((always_inline)) void block_mark_as_free(block_header_t* block)
{
	/* Link the block to the next block, first. */
	block_header_t* next = block_link_next(block);
	block_set_prev_free(next);
	block_set_free(block);
}

static inline __attribute__((always_inline)) void block_mark_as_used(block_header_t* block)
{
	block_header_t* next = block_next(block);
	block_set_prev_used(next);
	block_set_used(block);
}

#if defined(__cplusplus)
};
#endif
