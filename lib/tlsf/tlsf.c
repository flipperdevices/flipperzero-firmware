/*
 * SPDX-FileCopyrightText: 2006-2016 Matthew Conte
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include "tlsf.h"
#include "tlsf_common.h"
#include "tlsf_block_functions.h"

#if defined(__cplusplus)
#define tlsf_decl inline
#else
#define tlsf_decl static inline __attribute__((always_inline))
#endif

/*
** Architecture-specific bit manipulation routines.
**
** TLSF achieves O(1) cost for malloc and free operations by limiting
** the search for a free block to a free list of guaranteed size
** adequate to fulfill the request, combined with efficient free list
** queries using bitmasks and architecture-specific bit-manipulation
** routines.
**
** Most modern processors provide instructions to count leading zeroes
** in a word, find the lowest and highest set bit, etc. These
** specific implementations will be used when available, falling back
** to a reasonably efficient generic implementation.
**
** NOTE: TLSF spec relies on ffs/fls returning value 0..31.
** ffs/fls return 1-32 by default, returning 0 for error.
*/

/*
** Detect whether or not we are building for a 32- or 64-bit (LP/LLP)
** architecture. There is no reliable portable method at compile-time.
*/
#if defined (__alpha__) || defined (__ia64__) || defined (__x86_64__) \
	|| defined (_WIN64) || defined (__LP64__) || defined (__LLP64__)
#define TLSF_64BIT
#endif

/*
** gcc 3.4 and above have builtin support, specialized for architecture.
** Some compilers masquerade as gcc; patchlevel test filters them out.
*/
#if defined (__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)) \
	&& defined (__GNUC_PATCHLEVEL__)

#if defined (__SNC__)
/* SNC for Playstation 3. */

tlsf_decl int tlsf_ffs(unsigned int word)
{
	const unsigned int reverse = word & (~word + 1);
	const int bit = 32 - __builtin_clz(reverse);
	return bit - 1;
}

#else

tlsf_decl int tlsf_ffs(unsigned int word)
{
	return __builtin_ffs(word) - 1;
}

#endif

tlsf_decl int tlsf_fls(unsigned int word)
{
	const int bit = word ? 32 - __builtin_clz(word) : 0;
	return bit - 1;
}

#elif defined (_MSC_VER) && (_MSC_VER >= 1400) && (defined (_M_IX86) || defined (_M_X64))
/* Microsoft Visual C++ support on x86/X64 architectures. */

#include <intrin.h>

#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)

tlsf_decl int tlsf_fls(unsigned int word)
{
	unsigned long index;
	return _BitScanReverse(&index, word) ? index : -1;
}

tlsf_decl int tlsf_ffs(unsigned int word)
{
	unsigned long index;
	return _BitScanForward(&index, word) ? index : -1;
}

#elif defined (_MSC_VER) && defined (_M_PPC)
/* Microsoft Visual C++ support on PowerPC architectures. */

#include <ppcintrinsics.h>

tlsf_decl int tlsf_fls(unsigned int word)
{
	const int bit = 32 - _CountLeadingZeros(word);
	return bit - 1;
}

tlsf_decl int tlsf_ffs(unsigned int word)
{
	const unsigned int reverse = word & (~word + 1);
	const int bit = 32 - _CountLeadingZeros(reverse);
	return bit - 1;
}

#elif defined (__ARMCC_VERSION)
/* RealView Compilation Tools for ARM */

tlsf_decl int tlsf_ffs(unsigned int word)
{
	const unsigned int reverse = word & (~word + 1);
	const int bit = 32 - __clz(reverse);
	return bit - 1;
}

tlsf_decl int tlsf_fls(unsigned int word)
{
	const int bit = word ? 32 - __clz(word) : 0;
	return bit - 1;
}

#elif defined (__ghs__)
/* Green Hills support for PowerPC */

#include <ppc_ghs.h>

tlsf_decl int tlsf_ffs(unsigned int word)
{
	const unsigned int reverse = word & (~word + 1);
	const int bit = 32 - __CLZ32(reverse);
	return bit - 1;
}

tlsf_decl int tlsf_fls(unsigned int word)
{
	const int bit = word ? 32 - __CLZ32(word) : 0;
	return bit - 1;
}

#else
/* Fall back to generic implementation. */

tlsf_decl int tlsf_fls_generic(unsigned int word)
{
	int bit = 32;

	if (!word) bit -= 1;
	if (!(word & 0xffff0000)) { word <<= 16; bit -= 16; }
	if (!(word & 0xff000000)) { word <<= 8; bit -= 8; }
	if (!(word & 0xf0000000)) { word <<= 4; bit -= 4; }
	if (!(word & 0xc0000000)) { word <<= 2; bit -= 2; }
	if (!(word & 0x80000000)) { word <<= 1; bit -= 1; }

	return bit;
}

/* Implement ffs in terms of fls. */
tlsf_decl int tlsf_ffs(unsigned int word)
{
	return tlsf_fls_generic(word & (~word + 1)) - 1;
}

tlsf_decl int tlsf_fls(unsigned int word)
{
	return tlsf_fls_generic(word) - 1;
}

#endif

/* Possibly 64-bit version of tlsf_fls. */
#if defined (TLSF_64BIT)
tlsf_decl int tlsf_fls_sizet(size_t size)
{
	int high = (int)(size >> 32);
	int bits = 0;
	if (high)
	{
		bits = 32 + tlsf_fls(high);
	}
	else
	{
		bits = tlsf_fls((int)size & 0xffffffff);

	}
	return bits;
}
#else
#define tlsf_fls_sizet tlsf_fls
#endif

#undef tlsf_decl

/*
** Static assertion mechanism.
*/

#define _tlsf_glue2(x, y) x ## y
#define _tlsf_glue(x, y) _tlsf_glue2(x, y)
#define tlsf_static_assert(exp) \
	typedef char _tlsf_glue(static_assert, __LINE__) [(exp) ? 1 : -1]

/* This code has been tested on 32- and 64-bit (LP/LLP) architectures. */
tlsf_static_assert(sizeof(int) * CHAR_BIT == 32);
tlsf_static_assert(sizeof(size_t) * CHAR_BIT >= 32);
tlsf_static_assert(sizeof(size_t) * CHAR_BIT <= 64);

static inline __attribute__((always_inline)) size_t align_up(size_t x, size_t align)
{
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return (x + (align - 1)) & ~(align - 1);
}

static inline __attribute__((always_inline)) size_t align_down(size_t x, size_t align)
{
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return x - (x & (align - 1));
}

static inline __attribute__((always_inline)) void* align_ptr(const void* ptr, size_t align)
{
	const tlsfptr_t aligned =
		(tlsf_cast(tlsfptr_t, ptr) + (align - 1)) & ~(align - 1);
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return tlsf_cast(void*, aligned);
}

/*
** Adjust an allocation size to be aligned to word size, and no smaller
** than internal minimum.
*/
static inline __attribute__((always_inline)) size_t adjust_request_size(tlsf_t tlsf, size_t size, size_t align)
{
	size_t adjust = 0;
	if (size)
	{
		const size_t aligned = align_up(size, align);

		/* aligned sized must not exceed block_size_max or we'll go out of bounds on sl_bitmap */
		if (aligned < tlsf_block_size_max(tlsf)) 
		{
			adjust = tlsf_max(aligned, block_size_min);
		}
	}
	return adjust;
}

/*
** TLSF utility functions. In most cases, these are direct translations of
** the documentation found in the white paper.
*/

static inline __attribute__((always_inline)) void mapping_insert(control_t* control, size_t size, int* fli, int* sli)
{
	int fl, sl;
	if (size < control->small_block_size)
	{
		/* Store small blocks in first list. */
		fl = 0;
		sl = tlsf_cast(int, size) / (control->small_block_size / control->sl_index_count);
	}
	else
	{
		fl = tlsf_fls_sizet(size);
		sl = tlsf_cast(int, size >> (fl - control->sl_index_count_log2)) ^ (1 << control->sl_index_count_log2);
		fl -= (control->fl_index_shift - 1);
	}
	*fli = fl;
	*sli = sl;
}

/* This version rounds up to the next block size (for allocations) */
static inline __attribute__((always_inline)) void mapping_search(control_t* control, size_t size, int* fli, int* sli)
{
	if (size >= control->small_block_size)
	{
		const size_t round = (1 << (tlsf_fls_sizet(size) - control->sl_index_count_log2)) - 1;
		size += round;
	}
	mapping_insert(control, size, fli, sli);
}

static inline __attribute__((always_inline)) block_header_t* search_suitable_block(control_t* control, int* fli, int* sli)
{
	int fl = *fli;
	int sl = *sli;

	/*
	** First, search for a block in the list associated with the given
	** fl/sl index.
	*/
	unsigned int sl_map = control->sl_bitmap[fl] & (~0U << sl);
	if (!sl_map)
	{
		/* No block exists. Search in the next largest first-level list. */
		const unsigned int fl_map = control->fl_bitmap & (~0U << (fl + 1));
		if (!fl_map)
		{
			/* No free blocks available, memory has been exhausted. */
			return 0;
		}

		fl = tlsf_ffs(fl_map);
		*fli = fl;
		sl_map = control->sl_bitmap[fl];
	}
	tlsf_assert(sl_map && "internal error - second level bitmap is null");
	sl = tlsf_ffs(sl_map);
	*sli = sl;

	/* Return the first block in the free list. */
	return control->blocks[fl * control->sl_index_count + sl];
}

/* Remove a free block from the free list.*/
static inline __attribute__((always_inline)) void remove_free_block(control_t* control, block_header_t* block, int fl, int sl)
{
	block_header_t* prev = block->prev_free;
	block_header_t* next = block->next_free;
	tlsf_assert(prev && "prev_free field can not be null");
	tlsf_assert(next && "next_free field can not be null");
	next->prev_free = prev;
	prev->next_free = next;

	/* If this block is the head of the free list, set new head. */
	if (control->blocks[fl * control->sl_index_count + sl] == block)
	{
		control->blocks[fl * control->sl_index_count + sl] = next;

		/* If the new head is null, clear the bitmap. */
		if (next == &control->block_null)
		{
			control->sl_bitmap[fl] &= ~(1U << sl);

			/* If the second bitmap is now empty, clear the fl bitmap. */
			if (!control->sl_bitmap[fl])
			{
				control->fl_bitmap &= ~(1U << fl);
			}
		}
	}
}

/* Insert a free block into the free block list. */
static inline __attribute__((always_inline)) void insert_free_block(control_t* control, block_header_t* block, int fl, int sl)
{
	block_header_t* current = control->blocks[fl * control->sl_index_count + sl];
	tlsf_assert(current && "free list cannot have a null entry");
	tlsf_assert(block && "cannot insert a null entry into the free list");
	block->next_free = current;
	block->prev_free = &control->block_null;
	current->prev_free = block;

	tlsf_assert(block_to_ptr(block) == align_ptr(block_to_ptr(block), ALIGN_SIZE)
		&& "block not aligned properly");
	/*
	** Insert the new block at the head of the list, and mark the first-
	** and second-level bitmaps appropriately.
	*/
	control->blocks[fl * control->sl_index_count + sl] = block;
	control->fl_bitmap |= (1U << fl);
	control->sl_bitmap[fl] |= (1U << sl);
}

/* Remove a given block from the free list. */
static inline __attribute__((always_inline)) void block_remove(control_t* control, block_header_t* block)
{
	int fl, sl;
	mapping_insert(control, block_size(block), &fl, &sl);
	remove_free_block(control, block, fl, sl);
}

/* Insert a given block into the free list. */
static inline __attribute__((always_inline)) void block_insert(control_t* control, block_header_t* block)
{
	int fl, sl;
	mapping_insert(control, block_size(block), &fl, &sl);
	insert_free_block(control, block, fl, sl);
}

static inline __attribute__((always_inline)) int block_can_split(block_header_t* block, size_t size)
{
	return block_size(block) >= sizeof(block_header_t) + size;
}

/* Split a block into two, the second of which is free. */
static inline __attribute__((always_inline)) block_header_t* block_split(block_header_t* block, size_t size)
{
	/* Calculate the amount of space left in the remaining block.
	 * REMINDER: remaining pointer's first field is `prev_phys_block` but this field is part of the
	 * previous physical block. */
	block_header_t* remaining =
		offset_to_block(block_to_ptr(block), size - block_header_overhead);

	/* `size` passed as an argument is the first block's new size, thus, the remaining block's size
	 * is `block_size(block) - size`. However, the block's data must be precedeed by the data size.
	 * This field is NOT part of the size, so it has to be substracted from the calculation. */
	const size_t remain_size = block_size(block) - (size + block_header_overhead);

	tlsf_assert(block_to_ptr(remaining) == align_ptr(block_to_ptr(remaining), ALIGN_SIZE)
		&& "remaining block not aligned properly");

	tlsf_assert(block_size(block) == remain_size + size + block_header_overhead);
	block_set_size(remaining, remain_size);
	tlsf_assert(block_size(remaining) >= block_size_min && "block split with invalid size");

	block_set_size(block, size);
	block_mark_as_free(remaining);

	/**
	 * Here is the final outcome of this function:
	 *
	 * block             remaining (block_ptr + size - BHO)
	 * +                                +
	 * |                                |
	 * v                                v
	 * +----------------------------------------------------------------------+
	 * |0000|    |xxxxxxxxxxxxxxxxxxxxxx|xxxx|    |###########################|
	 * |0000|    |xxxxxxxxxxxxxxxxxxxxxx|xxxx|    |###########################|
	 * |0000|    |xxxxxxxxxxxxxxxxxxxxxx|xxxx|    |###########################|
	 * |0000|    |xxxxxxxxxxxxxxxxxxxxxx|xxxx|    |###########################|
	 * +----------------------------------------------------------------------+
	 *      |    |                           |    |
	 *      +    +<------------------------->+    +<------------------------->
	 *       BHO    `size` (argument) bytes   BHO      `remain_size` bytes
	 *
	 * Where BHO = block_header_overhead,
	 * 0: part of the memory owned by a `block`'s previous neighbour,
	 * x: part of the memory owned by `block`.
	 * #: part of the memory owned by `remaining`.
	 */

	return remaining;
}

/* Absorb a free block's storage into an adjacent previous free block. */
static inline __attribute__((always_inline)) block_header_t* block_absorb(block_header_t* prev, block_header_t* block)
{
	tlsf_assert(!block_is_last(prev) && "previous block can't be last");
	/* Note: Leaves flags untouched. */
	prev->size += block_size(block) + block_header_overhead;
	block_link_next(prev);

	if (block_absorb_post_hook != NULL)
	{
		block_absorb_post_hook(block, sizeof(block_header_t), POISONING_AFTER_FREE);
	}

	return prev;
}

/* Merge a just-freed block with an adjacent previous free block. */
static inline __attribute__((always_inline)) block_header_t* block_merge_prev(control_t* control, block_header_t* block)
{
	if (block_is_prev_free(block))
	{
		block_header_t* prev = block_prev(block);
		tlsf_assert(prev && "prev physical block can't be null");
		tlsf_assert(block_is_free(prev) && "prev block is not free though marked as such");
		block_remove(control, prev);
		block = block_absorb(prev, block);
	}

	return block;
}

/* Merge a just-freed block with an adjacent free block. */
static inline __attribute__((always_inline)) block_header_t* block_merge_next(control_t* control, block_header_t* block)
{
	block_header_t* next = block_next(block);
	tlsf_assert(next && "next physical block can't be null");

	if (block_is_free(next))
	{
		tlsf_assert(!block_is_last(block) && "previous block can't be last");
		block_remove(control, next);
		block = block_absorb(block, next);
	}

	return block;
}

/* Trim any trailing block space off the end of a block, return to pool. */
static inline __attribute__((always_inline)) void block_trim_free(control_t* control, block_header_t* block, size_t size)
{
	tlsf_assert(block_is_free(block) && "block must be free");
	if (block_can_split(block, size))
	{
		block_header_t* remaining_block = block_split(block, size);
		block_link_next(block);
		block_set_prev_free(remaining_block);
		block_insert(control, remaining_block);
	}
}

/* Trim any trailing block space off the end of a used block, return to pool. */
static inline __attribute__((always_inline)) void block_trim_used(control_t* control, block_header_t* block, size_t size)
{
	tlsf_assert(!block_is_free(block) && "block must be used");
	if (block_can_split(block, size))
	{
		/* If the next block is free, we must coalesce. */
		block_header_t* remaining_block = block_split(block, size);
		block_set_prev_used(remaining_block);

		remaining_block = block_merge_next(control, remaining_block);
		block_insert(control, remaining_block);
	}
}

static inline __attribute__((always_inline)) block_header_t* block_trim_free_leading(control_t* control, block_header_t* block, size_t size)
{
	block_header_t* remaining_block = block;
	if (block_can_split(block, size))
	{
		/* We want to split `block` in two: the first block will be freed and the
		 * second block will be returned. */
		remaining_block = block_split(block, size - block_header_overhead);

		/* `remaining_block` is the second block, mark its predecessor (first
		 * block) as free. */
		block_set_prev_free(remaining_block);

		block_link_next(block);

		/* Put back the first block into the free memory list. */
		block_insert(control, block);
	}

	return remaining_block;
}

static inline __attribute__((always_inline)) block_header_t* block_locate_free(control_t* control, size_t size)
{
	int fl = 0, sl = 0;
	block_header_t* block = 0;

	if (size)
	{
		mapping_search(control, size, &fl, &sl);
		
		/*
		** mapping_search can futz with the size, so for excessively large sizes it can sometimes wind up 
		** with indices that are off the end of the block array.
		** So, we protect against that here, since this is the only callsite of mapping_search.
		** Note that we don't need to check sl, since it comes from a modulo operation that guarantees it's always in range.
		*/
		if (fl < control->fl_index_count)
		{
			block = search_suitable_block(control, &fl, &sl);
		}
	}

	if (block)
	{
		tlsf_assert(block_size(block) >= size);
		remove_free_block(control, block, fl, sl);
	}

	return block;
}

static inline __attribute__((always_inline)) void* block_prepare_used(control_t* control, block_header_t* block, size_t size)
{
	void* p = 0;
	if (block)
	{
		tlsf_assert(size && "size must be non-zero");
		block_trim_free(control, block, size);
		block_mark_as_used(block);
		p = block_to_ptr(block);
	}
	return p;
}

/* Clear structure and point all empty lists at the null block. */
static control_t* control_construct(control_t* control, size_t bytes)
{
	// check that the requested size can at least hold the control_t. This will allow us 
	// to fill in the field of control_t necessary to determine the final size of 
	// the metadata overhead and check that the requested size can hold
	// this data and at least a block of minimum size
	if (bytes < sizeof(control_t))
	{
		return NULL;
	}

	/* Find the closest power of two for first layer */
	control->fl_index_max = 32 - __builtin_clz(bytes);

	/* Adapt second layer to the pool */
	if (bytes <= 16 * 1024) control->sl_index_count_log2 = 3;
	else if (bytes <= 256 * 1024) control->sl_index_count_log2 = 4;
	else control->sl_index_count_log2 = 5;

	control->fl_index_shift = (control->sl_index_count_log2 + ALIGN_SIZE_LOG2);
	control->sl_index_count = 1 << control->sl_index_count_log2;
	control->fl_index_count = control->fl_index_max - control->fl_index_shift + 1;
	control->small_block_size = 1 << control->fl_index_shift;
	
	// the total size fo the metadata overhead is the size of the control_t
	// added to the size of the sl_bitmaps and the size of blocks
	control->size = sizeof(control_t) + (sizeof(*control->sl_bitmap) * control->fl_index_count) +
										(sizeof(*control->blocks) * (control->fl_index_count * control->sl_index_count));

	// check that the requested size can hold the whole control structure and
	// a small block at least
	if (bytes < control->size + block_size_min)
	{
		return NULL;
	}

	control->block_null.next_free = &control->block_null;
	control->block_null.prev_free = &control->block_null;

	control->fl_bitmap = 0;
	control->sl_bitmap = align_ptr(control + 1, sizeof(*control->sl_bitmap));
	control->blocks = align_ptr(control->sl_bitmap + control->fl_index_count, sizeof(*control->blocks));


	/* SL_INDEX_COUNT must be <= number of bits in sl_bitmap's storage type. */
	tlsf_assert(sizeof(unsigned int) * CHAR_BIT >= control->sl_index_count
		&& "CHAR_BIT less than sl_index_count");

	/* Ensure we've properly tuned our sizes. */
	tlsf_assert(ALIGN_SIZE == control->small_block_size / control->sl_index_count); //ALIGN_SIZE does not match");

	for (int i = 0; i < control->fl_index_count; ++i)
	{
		control->sl_bitmap[i] = 0;
		for (int j = 0; j < control->sl_index_count; ++j)
		{
			control->blocks[i * control->sl_index_count + j] = &control->block_null;
		}
	}

	return control;
}

/*
** Debugging utilities.
*/

typedef struct integrity_t
{
	int prev_status;
	int status;
} integrity_t;

#define tlsf_insist(x) { if (!(x)) { status--; } }

static bool integrity_walker(void* ptr, size_t size, int used, void* user)
{
	block_header_t* block = block_from_ptr(ptr);
	integrity_t* integ = tlsf_cast(integrity_t*, user);
	const int this_prev_status = block_is_prev_free(block) ? 1 : 0;
	const int this_status = block_is_free(block) ? 1 : 0;
	const size_t this_block_size = block_size(block);

	int status = 0;
	tlsf_insist(integ->prev_status == this_prev_status && "prev status incorrect");
	tlsf_insist(size == this_block_size && "block size incorrect");

	if (tlsf_check_hook != NULL)
	{
		/* block_size(block) returns the size of the usable memory when the block is allocated.
		 * As the block under test is free, we need to subtract to the block size the next_free
		 * and prev_free fields of the block header as they are not a part of the usable memory
		 * when the block is free. In addition, we also need to subtract the size of prev_phys_block
		 * as this field is in fact part of the current free block and not part of the next (allocated)
		 * block. Check the comments in block_split function for more details.
		 */
		const size_t actual_free_block_size = used ? this_block_size : 
													 this_block_size - offsetof(block_header_t, next_free)- block_header_overhead;
		
		void* ptr_block = used ? (void*)block + block_start_offset :
								 (void*)block + sizeof(block_header_t);

		tlsf_insist(tlsf_check_hook(ptr_block, actual_free_block_size, !used));
	}

	integ->prev_status = this_status;
	integ->status += status;

	return true;
}


int tlsf_check(tlsf_t tlsf)
{
	int i, j;

	control_t* control = tlsf_cast(control_t*, tlsf);
	int status = 0;

	/* Check that the free lists and bitmaps are accurate. */
	for (i = 0; i < control->fl_index_count; ++i)
	{
		for (j = 0; j < control->sl_index_count; ++j)
		{
			const int fl_map = control->fl_bitmap & (1U << i);
			const int sl_list = control->sl_bitmap[i];
			const int sl_map = sl_list & (1U << j);
			const block_header_t* block = control->blocks[i * control->sl_index_count + j];

			/* Check that first- and second-level lists agree. */
			if (!fl_map)
			{
				tlsf_insist(!sl_map && "second-level map must be null");
			}

			if (!sl_map)
			{
				tlsf_insist(block == &control->block_null && "block list must be null");
				continue;
			}

			/* Check that there is at least one free block. */
			tlsf_insist(sl_list && "no free blocks in second-level map");
			tlsf_insist(block != &control->block_null && "block should not be null");

			while (block != &control->block_null)
			{
				int fli, sli;
				const bool is_block_free = block_is_free(block);
				tlsf_insist(is_block_free && "block should be free");
				tlsf_insist(!block_is_prev_free(block) && "blocks should have coalesced");
				tlsf_insist(!block_is_free(block_next(block)) && "blocks should have coalesced");
				tlsf_insist(block_is_prev_free(block_next(block)) && "block should be free");
				tlsf_insist(block_size(block) >= block_size_min && "block not minimum size");

				mapping_insert(control, block_size(block), &fli, &sli);
				tlsf_insist(fli == i && sli == j && "block size indexed in wrong list");

				block = block->next_free;
			}
		}
	}

	return status;
}

#undef tlsf_insist

static bool default_walker(void* ptr, size_t size, int used, void* user)
{
	(void)user;
	printf("\t%p %s size: %x (%p)\n", ptr, used ? "used" : "free", (unsigned int)size, block_from_ptr(ptr));
	return true;
}

void tlsf_walk_pool(pool_t pool, tlsf_walker walker, void* user)
{
	tlsf_walker pool_walker = walker ? walker : default_walker;
	block_header_t* block =
		offset_to_block(pool, -(int)block_header_overhead);

	bool ret_val = true;
	while (block && !block_is_last(block) && ret_val == true)
	{
		ret_val = pool_walker(
			block_to_ptr(block),
			block_size(block),
			!block_is_free(block),
			user);

		if (ret_val == true) {
			block = block_next(block);
		}
	}
}

size_t tlsf_block_size(void* ptr)
{
	size_t size = 0;
	if (ptr)
	{
		const block_header_t* block = block_from_ptr(ptr);
		size = block_size(block);
	}
	return size;
}

int tlsf_check_pool(pool_t pool)
{
	/* Check that the blocks are physically correct. */
	integrity_t integ = { 0, 0 };
	tlsf_walk_pool(pool, integrity_walker, &integ);

	return integ.status;
}

size_t tlsf_fit_size(tlsf_t tlsf, size_t size)
{
	if (size == 0 || tlsf == NULL) {
		return 0;
	}

	control_t* control = tlsf_cast(control_t*, tlsf);
	if (size < control->small_block_size) {
		return adjust_request_size(tlsf, size, ALIGN_SIZE);
	}

	/* because it's GoodFit, allocable size is one range lower */
	size_t sl_interval;
	sl_interval = (1 << (32 - __builtin_clz(size) - 1)) / control->sl_index_count;
	return size & ~(sl_interval - 1);
}

/*
** Size of the TLSF structures in a given memory block passed to
** tlsf_create, equal to the size of a control_t
*/
size_t tlsf_size(tlsf_t tlsf)
{
	if (tlsf == NULL)
	{
		return 0;
	}
	control_t* control = tlsf_cast(control_t*, tlsf);
	return control->size;
}

size_t tlsf_align_size(void)
{
	return ALIGN_SIZE;
}

size_t tlsf_block_size_min(void)
{
	return block_size_min;
}

size_t tlsf_block_size_max(tlsf_t tlsf)
{
	if (tlsf == NULL)
	{
		return 0;
	}
	control_t* control = tlsf_cast(control_t*, tlsf);
	return tlsf_cast(size_t, 1) << control->fl_index_max;
}

/*
** Overhead of the TLSF structures in a given memory block passed to
** tlsf_add_pool, equal to the overhead of a free block and the
** sentinel block.
*/
size_t tlsf_pool_overhead(void)
{
	return 2 * block_header_overhead;
}

size_t tlsf_alloc_overhead(void)
{
	return block_header_overhead;
}

pool_t tlsf_add_pool(tlsf_t tlsf, void* mem, size_t bytes)
{
	block_header_t* block;
	block_header_t* next;

	const size_t pool_overhead = tlsf_pool_overhead();
	const size_t pool_bytes = align_down(bytes - pool_overhead, ALIGN_SIZE);

	if (((ptrdiff_t)mem % ALIGN_SIZE) != 0)
	{
		printf("tlsf_add_pool: Memory must be aligned by %u bytes.\n",
			(unsigned int)ALIGN_SIZE);
		return 0;
	}

	if (pool_bytes < block_size_min || pool_bytes > tlsf_block_size_max(tlsf))
	{
#if defined (TLSF_64BIT)
		printf("tlsf_add_pool: Memory size must be between 0x%x and 0x%x00 bytes.\n", 
			(unsigned int)(pool_overhead + block_size_min),
			(unsigned int)((pool_overhead + tlsf_block_size_max(tlsf)) / 256));
#else
		printf("tlsf_add_pool: Memory size must be between %u and %u bytes.\n", 
			(unsigned int)(pool_overhead + block_size_min),
			(unsigned int)(pool_overhead + tlsf_block_size_max(tlsf)));
#endif
		return 0;
	}

	/*
	** Create the main free block. Offset the start of the block slightly
	** so that the prev_phys_block field falls outside of the pool -
	** it will never be used.
	*/
	block = offset_to_block(mem, -(tlsfptr_t)block_header_overhead);
	block_set_size(block, pool_bytes);
	block_set_free(block);
	block_set_prev_used(block);
	block_insert(tlsf_cast(control_t*, tlsf), block);

	/* Split the block to create a zero-size sentinel block. */
	next = block_link_next(block);
	block_set_size(next, 0);
	block_set_used(next);
	block_set_prev_free(next);

	return mem;
}

void tlsf_remove_pool(tlsf_t tlsf, pool_t pool)
{
	control_t* control = tlsf_cast(control_t*, tlsf);
	block_header_t* block = offset_to_block(pool, -(int)block_header_overhead);

	int fl = 0, sl = 0;

	tlsf_assert(block_is_free(block) && "block should be free");
	tlsf_assert(!block_is_free(block_next(block)) && "next block should not be free");
	tlsf_assert(block_size(block_next(block)) == 0 && "next block size should be zero");

	mapping_insert(control, block_size(block), &fl, &sl);
	remove_free_block(control, block, fl, sl);
}

/*
** TLSF main interface.
*/

#if _DEBUG
int test_ffs_fls()
{
	/* Verify ffs/fls work properly. */
	int rv = 0;
	rv += (tlsf_ffs(0) == -1) ? 0 : 0x1;
	rv += (tlsf_fls(0) == -1) ? 0 : 0x2;
	rv += (tlsf_ffs(1) == 0) ? 0 : 0x4;
	rv += (tlsf_fls(1) == 0) ? 0 : 0x8;
	rv += (tlsf_ffs(0x80000000) == 31) ? 0 : 0x10;
	rv += (tlsf_ffs(0x80008000) == 15) ? 0 : 0x20;
	rv += (tlsf_fls(0x80000008) == 31) ? 0 : 0x40;
	rv += (tlsf_fls(0x7FFFFFFF) == 30) ? 0 : 0x80;

#if defined (TLSF_64BIT)
	rv += (tlsf_fls_sizet(0x80000000) == 31) ? 0 : 0x100;
	rv += (tlsf_fls_sizet(0x100000000) == 32) ? 0 : 0x200;
	rv += (tlsf_fls_sizet(0xffffffffffffffff) == 63) ? 0 : 0x400;
#endif

	if (rv)
	{
		printf("test_ffs_fls: %x ffs/fls tests failed.\n", rv);
	}
	return rv;
}
#endif

tlsf_t tlsf_create(void* mem, size_t max_bytes)
{
#if _DEBUG
	if (test_ffs_fls())
	{
		return NULL;
	}
#endif

	if (mem == NULL)
	{
		return NULL;
	}

	if (((tlsfptr_t)mem % ALIGN_SIZE) != 0)
	{
		printf("tlsf_create: Memory must be aligned to %u bytes.\n",
			(unsigned int)ALIGN_SIZE);
		return NULL;
	}

	control_t* control_ptr = control_construct(tlsf_cast(control_t*, mem), max_bytes);
	return tlsf_cast(tlsf_t, control_ptr);
}

tlsf_t tlsf_create_with_pool(void* mem, size_t pool_bytes, size_t max_bytes)
{
	tlsf_t tlsf = tlsf_create(mem, max_bytes ? max_bytes : pool_bytes);
	if (tlsf != NULL)
	{
		tlsf_add_pool(tlsf, (char*)mem + tlsf_size(tlsf), pool_bytes - tlsf_size(tlsf));
	}
	return tlsf;
}

void tlsf_destroy(tlsf_t tlsf)
{
	/* Nothing to do. */
	(void)tlsf;
}

pool_t tlsf_get_pool(tlsf_t tlsf)
{
	return tlsf_cast(pool_t, (char*)tlsf + tlsf_size(tlsf));
}

void* tlsf_malloc(tlsf_t tlsf, size_t size)
{
	control_t* control = tlsf_cast(control_t*, tlsf);
	const size_t adjust = adjust_request_size(tlsf, size, ALIGN_SIZE);
	block_header_t* block = block_locate_free(control, adjust);
	return block_prepare_used(control, block, adjust);
}

void* tlsf_malloc_aligned8(tlsf_t tlsf, size_t size){
	return tlsf_memalign(tlsf, 8, size);
}

/**
 * @brief Allocate memory of at least `size` bytes at a given address in the pool.
 *
 * @param tlsf TLSF structure to allocate memory from.
 * @param size Minimum size, in bytes, of the memory to allocate
 * @param address address at which the allocation must be done
 *
 * @return pointer to free memory or NULL in case of incapacity to perform the malloc
 */
void* tlsf_malloc_addr(tlsf_t tlsf, size_t size, void *address)
{
	control_t* control = tlsf_cast(control_t*, tlsf);

	/* adjust the address to be ALIGN_SIZE bytes aligned. */
	const unsigned int addr_adjusted = align_down(tlsf_cast(unsigned int, address), ALIGN_SIZE);

	/* adjust the size to be ALIGN_SIZE bytes aligned. Add to the size the difference
	 * between the requested address and the address_adjusted. */
	size_t size_adjusted = align_up(size + (tlsf_cast(unsigned int, address) - addr_adjusted), ALIGN_SIZE);

	/* find the free block that starts before the address in the pool and is big enough
	 * to support the size of allocation at the given address */
	block_header_t* block = offset_to_block(tlsf_get_pool(tlsf), -(int)block_header_overhead);
	
	const char *alloc_start = tlsf_cast(char*, addr_adjusted);
	const char *alloc_end = alloc_start + size_adjusted;
	bool block_found = false;
	do {
		const char *block_start = tlsf_cast(char*, block_to_ptr(block));
		const char *block_end = tlsf_cast(char*, block_to_ptr(block)) + block_size(block);
		if (block_start <= alloc_start && block_end > alloc_start) {
			/* A: block_end >= alloc_end. B: block is free */
			if (block_end < alloc_end || !block_is_free(block)) {
				/* not(A) || not(B)
				 * We won't find another suitable block from this point on
				 * so we can break and return NULL */
				break;
			} 
			/* A && B
			 * The block can fit the alloc and is located at a position allowing for the alloc
			 * to be placed at the given address. We can return from the while */
			block_found = true;
		} else if (!block_is_last(block)) {
			/* the block doesn't match the expected criteria, continue with the next block */
			block = block_next(block);
		}

	} while (!block_is_last(block) && block_found == false);

	if (!block_found) {
		return NULL;
	}
	
	/* remove block from the free list since a part of it will be used */
	block_remove(control, block);

	/* trim any leading space or add the leading space to the overall requested size
	 * if the leading space is not big enough to store a block of minimum size */
	const size_t space_before_addr_adjusted = addr_adjusted - tlsf_cast(unsigned int, block_to_ptr(block));
	block_header_t *return_block = block;
	if (space_before_addr_adjusted >= block_size_min) {
		return_block = block_trim_free_leading(control, block, space_before_addr_adjusted);
	}
	else {
		size_adjusted += space_before_addr_adjusted;
	}

	/* trim trailing space if any and return a pointer to the first usable byte allocated */
	return  block_prepare_used(control, return_block, size_adjusted);
}

/**
 * @brief Allocate memory of at least `size` bytes where byte at `data_offset` will be aligned to `alignment`.
 *
 * This function will allocate memory pointed by `ptr`. However, the byte at `data_offset` of
 * this piece of memory (i.e., byte at `ptr` + `data_offset`) will be aligned to `alignment`.
 * This function is useful for allocating memory that will internally have a header, and the
 * usable memory following the header (i.e. `ptr` + `data_offset`) must be aligned.
 *
 * For example, a call to `multi_heap_aligned_alloc_impl_offs(heap, 64, 256, 20)` will return a
 * pointer `ptr` to free memory of minimum 64 bytes, where `ptr + 20` is aligned on `256`.
 * So `(ptr + 20) % 256` equals 0.
 *
 * @param tlsf TLSF structure to allocate memory from.
 * @param align Alignment for the returned pointer's offset.
 * @param size Minimum size, in bytes, of the memory to allocate INCLUDING
 *             `data_offset` bytes.
 * @param data_offset Offset to be aligned on `alignment`. This can be 0, in
 *                    this case, the returned pointer will be aligned on
 *                    `alignment`. If it is not a multiple of CPU word size,
 *                    it will be aligned up to the closest multiple of it.
 *
 * @return pointer to free memory.
 */
void* tlsf_memalign_offs(tlsf_t tlsf, size_t align, size_t size, size_t data_offset)
{
	control_t* control = tlsf_cast(control_t*, tlsf);
	const size_t adjust = adjust_request_size(tlsf, size, ALIGN_SIZE);
	const size_t off_adjust = align_up(data_offset, ALIGN_SIZE);

	/*
	** We must allocate an additional minimum block size bytes so that if
	** our free block will leave an alignment gap which is smaller, we can
	** trim a leading free block and release it back to the pool. We must
	** do this because the previous physical block is in use, therefore
	** the prev_phys_block field is not valid, and we can't simply adjust
	** the size of that block.
	*/
	const size_t gap_minimum = sizeof(block_header_t) + off_adjust;
	/* The offset is included in both `adjust` and `gap_minimum`, so we
	** need to subtract it once.
	*/
	const size_t size_with_gap = adjust_request_size(tlsf, adjust + align + gap_minimum - off_adjust, align);

	/*
	** If alignment is less than or equal to base alignment, we're done, because
	** we are guaranteed that the size is at least sizeof(block_header_t), enough
	** to store next blocks' metadata. Plus, all pointers allocated will all be
	** aligned on a 4-byte bound, so ptr + data_offset will also have this
	** alignment constraint. Thus, the gap is not required.
	** If we requested 0 bytes, return null, as tlsf_malloc(0) does.
	*/
	const size_t aligned_size = (adjust && align > ALIGN_SIZE) ? size_with_gap : adjust;

	block_header_t* block = block_locate_free(control, aligned_size);

	/* This can't be a static assert. */
	tlsf_assert(sizeof(block_header_t) == block_size_min + block_header_overhead);

	if (block)
	{
		void* ptr = block_to_ptr(block);
		void* aligned = align_ptr(ptr, align);
		size_t gap = tlsf_cast(size_t,
			tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));

	   /*
		** If gap size is too small or if there is no gap but we need one,
		** offset to next aligned boundary.
		** NOTE: No need for a gap if the alignment required is less than or is
		** equal to ALIGN_SIZE.
		*/
		if ((gap && gap < gap_minimum) || (!gap && off_adjust && align > ALIGN_SIZE))
		{
			const size_t gap_remain = gap_minimum - gap;
			const size_t offset = tlsf_max(gap_remain, align);
			const void* next_aligned = tlsf_cast(void*,
				tlsf_cast(tlsfptr_t, aligned) + offset);

			aligned = align_ptr(next_aligned, align);
			gap = tlsf_cast(size_t,
				tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));
		}

		if (gap)
		{
			tlsf_assert(gap >= gap_minimum && "gap size too small");
			block = block_trim_free_leading(control, block, gap - off_adjust);
		}
	}

	/* Preparing the block will also the trailing free memory. */
	return block_prepare_used(control, block, adjust);
}

/**
 * @brief Same as `tlsf_memalign_offs` function but with a 0 offset.
 * The pointer returned is aligned on `align`.
 */
void* tlsf_memalign(tlsf_t tlsf, size_t align, size_t size)
{
	return tlsf_memalign_offs(tlsf, align, size, 0);
}


void tlsf_free(tlsf_t tlsf, void* ptr)
{
	/* Don't attempt to free a NULL pointer. */
	if (ptr)
	{
		control_t* control = tlsf_cast(control_t*, tlsf);
		block_header_t* block = block_from_ptr(ptr);
		tlsf_assert(!block_is_free(block) && "block already marked as free");
		block_mark_as_free(block);
		block = block_merge_prev(control, block);
		block = block_merge_next(control, block);
		block_insert(control, block);
	}
}

/*
** The TLSF block information provides us with enough information to
** provide a reasonably intelligent implementation of realloc, growing or
** shrinking the currently allocated block as required.
**
** This routine handles the somewhat esoteric edge cases of realloc:
** - a non-zero size with a null pointer will behave like malloc
** - a zero size with a non-null pointer will behave like free
** - a request that cannot be satisfied will leave the original buffer
**   untouched
** - an extended buffer size will leave the newly-allocated area with
**   contents undefined
*/
void* tlsf_realloc(tlsf_t tlsf, void* ptr, size_t size)
{
	control_t* control = tlsf_cast(control_t*, tlsf);
	void* p = 0;

	/* Zero-size requests are treated as free. */
	if (ptr && size == 0)
	{
		tlsf_free(tlsf, ptr);
	}
	/* Requests with NULL pointers are treated as malloc. */
	else if (!ptr)
	{
		p = tlsf_malloc_aligned8(tlsf, size);
	}
	else
	{
		block_header_t* block = block_from_ptr(ptr);
		block_header_t* next = block_next(block);

		const size_t cursize = block_size(block);
		const size_t combined = cursize + block_size(next) + block_header_overhead;
		const size_t adjust = adjust_request_size(tlsf, size, ALIGN_SIZE);

		// if adjust if equal to 0, the size is too big
		if (adjust == 0)
		{
			return p;
		}

		tlsf_assert(!block_is_free(block) && "block already marked as free");

		/*
		** If the next block is used, or when combined with the current
		** block, does not offer enough space, we must reallocate and copy.
		*/
		if (adjust > cursize && (!block_is_free(next) || adjust > combined))
		{
			p = tlsf_malloc_aligned8(tlsf, size);
			if (p)
			{
				const size_t minsize = tlsf_min(cursize, size);
				memcpy(p, ptr, minsize);
				tlsf_free(tlsf, ptr);
			}
		}
		else
		{
			/* Do we need to expand to the next block? */
			if (adjust > cursize)
			{
				block_merge_next(control, block);
				block_mark_as_used(block);
			}

			/* Trim the resulting block and return the original pointer. */
			block_trim_used(control, block, adjust);
			p = ptr;
		}
	}

	return p;
}
