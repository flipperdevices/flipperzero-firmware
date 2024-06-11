/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INCLUDED_tlsfbits
#define INCLUDED_tlsfbits

#include <stdint.h>
typedef uint32_t u32;

#define BITS_PER_BYTE		8
#define BITS_PER_LONG		(sizeof(long) * BITS_PER_BYTE)

#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
#define BITS_TO_U64(nr)		DIV_ROUND_UP(nr, BITS_PER_TYPE(u64))
#define BITS_TO_U32(nr)		DIV_ROUND_UP(nr, BITS_PER_TYPE(u32))
#define BITS_TO_BYTES(nr)	DIV_ROUND_UP(nr, BITS_PER_TYPE(char))
#define BYTES_TO_BITS(nb)	(((BITS_PER_LONG * (nb)) / sizeof(long)))

#ifdef CONFIG_64BIT
#define TLSF_64BIT
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

static int tlsf_ffs(unsigned int word)
{
	return ffs(word) - 1;
}

static int tlsf_fls(unsigned int word)
{
	return fls(word) - 1;
}

/* Possibly 64-bit version of tlsf_fls. */
#if defined (TLSF_64BIT)
static int tlsf_fls_sizet(size_t size)
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

#endif
