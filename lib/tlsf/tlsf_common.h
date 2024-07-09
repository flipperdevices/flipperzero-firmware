/*
 * SPDX-FileCopyrightText: 2006-2016 Matthew Conte
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include <stddef.h>
#include <assert.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
** Constants definition for poisoning.
** These defines are used as 3rd argument of tlsf_poison_fill_region() for readability purposes.
*/
#define POISONING_AFTER_FREE true
#define POISONING_AFTER_MALLOC !POISONING_AFTER_FREE

/*
** Cast and min/max macros.
*/
#define tlsf_cast(t, exp)	((t) (exp))
#define tlsf_min(a, b)		((a) < (b) ? (a) : (b))
#define tlsf_max(a, b)		((a) > (b) ? (a) : (b))

/*
** Set assert macro, if it has not been provided by the user.
*/
#if !defined (tlsf_assert)
#define tlsf_assert assert
#endif

enum tlsf_config
{
	/* All allocation sizes and addresses are aligned to 4 bytes. */
	ALIGN_SIZE_LOG2 = 2,
	ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),
};

/*
** Data structures and associated constants.
*/

/* A type used for casting when doing pointer arithmetic. */
typedef ptrdiff_t tlsfptr_t;

typedef struct block_header_t
{
	/* Points to the previous physical block. */
	struct block_header_t* prev_phys_block;

	/* The size of this block, excluding the block header. */
	size_t size;

	/* Next and previous free blocks. */
	struct block_header_t* next_free;
	struct block_header_t* prev_free;
} block_header_t;

/*
** Since block sizes are always at least a multiple of 4, the two least
** significant bits of the size field are used to store the block status:
** - bit 0: whether block is busy or free
** - bit 1: whether previous block is busy or free
*/
static const size_t block_header_free_bit = 1 << 0;
static const size_t block_header_prev_free_bit = 1 << 1;

/*
** The size of the block header exposed to used blocks is the size field.
** The prev_phys_block field is stored *inside* the previous free block.
*/
static const size_t block_header_overhead = sizeof(size_t);

/* User data starts directly after the size field in a used block. */
static const size_t block_start_offset =
	offsetof(block_header_t, size) + sizeof(size_t);

/*
** A free block must be large enough to store its header minus the size of
** the prev_phys_block field, and no larger than the number of addressable
** bits for FL_INDEX.
*/
static const size_t block_size_min = 
	sizeof(block_header_t) - sizeof(block_header_t*);

/* The TLSF control structure. */
typedef struct control_t
{
    /* Empty lists point at this block to indicate they are free. */
    block_header_t block_null;

    /* Local parameter for the pool. Given the maximum
	 * value of each field, all the following parameters
	 * can fit on 4 bytes when using bitfields
	 */
    unsigned int fl_index_count : 5; // 5 cumulated bits
    unsigned int fl_index_shift : 3; // 8 cumulated bits
    unsigned int fl_index_max : 6; // 14 cumulated bits
    unsigned int sl_index_count : 6; // 20 cumulated bits

	/* log2 of number of linear subdivisions of block sizes. Larger
	** values require more memory in the control structure. Values of
	** 4 or 5 are typical.
	*/
    unsigned int sl_index_count_log2 : 3; // 23 cumulated bits
    unsigned int small_block_size : 8; // 31 cumulated bits

	/* size of the metadata ( size of control block,
	 * sl_bitmap and blocks )
	 */
    size_t size;

    /* Bitmaps for free lists. */
    unsigned int fl_bitmap;
    unsigned int *sl_bitmap;

    /* Head of free lists. */
    block_header_t** blocks;
} control_t;

#if defined(__cplusplus)
};
#endif
