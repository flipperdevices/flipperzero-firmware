/*
 * SPDX-FileCopyrightText: 2006-2016 Matthew Conte
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef INCLUDED_tlsf
#define INCLUDED_tlsf

#include <stddef.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* tlsf_t: a TLSF structure. Can contain 1 to N pools. */
/* pool_t: a block of memory that TLSF can manage. */
typedef void* tlsf_t;
typedef void* pool_t;

/* Create/destroy a memory pool. */
tlsf_t tlsf_create(void* mem, size_t max_bytes);
tlsf_t tlsf_create_with_pool(void* mem, size_t pool_bytes, size_t max_bytes);
void tlsf_destroy(tlsf_t tlsf);
pool_t tlsf_get_pool(tlsf_t tlsf);

/* Add/remove memory pools. */
pool_t tlsf_add_pool(tlsf_t tlsf, void* mem, size_t bytes);
void tlsf_remove_pool(tlsf_t tlsf, pool_t pool);

/* malloc/memalign/realloc/free replacements. */
void* tlsf_malloc(tlsf_t tlsf, size_t size);
void* tlsf_memalign(tlsf_t tlsf, size_t align, size_t size);
void* tlsf_memalign_offs(tlsf_t tlsf, size_t align, size_t size, size_t offset);
void* tlsf_malloc_addr(tlsf_t tlsf, size_t size, void *address);
void* tlsf_realloc(tlsf_t tlsf, void* ptr, size_t size);
void tlsf_free(tlsf_t tlsf, void* ptr);

/* Returns internal block size, not original request size */
size_t tlsf_block_size(void* ptr);

/* Overheads/limits of internal structures. */
size_t tlsf_size(tlsf_t tlsf);
size_t tlsf_align_size(void);
size_t tlsf_block_size_min(void);
size_t tlsf_block_size_max(tlsf_t tlsf);
size_t tlsf_pool_overhead(void);
size_t tlsf_alloc_overhead(void);

/**
 * @brief Return the allocable size based on the size passed
 * as parameter
 * 
 * @param tlsf Pointer to the tlsf structure
 * @param size The allocation size
 * @return size_t The updated allocation size
 */
size_t tlsf_fit_size(tlsf_t tlsf, size_t size);

/* Debugging. */
typedef bool (*tlsf_walker)(void* ptr, size_t size, int used, void* user);
void tlsf_walk_pool(pool_t pool, tlsf_walker walker, void* user);
/* Returns nonzero if any internal consistency check fails. */
int tlsf_check(tlsf_t tlsf);
int tlsf_check_pool(pool_t pool);

/*!
 * @brief Weak function filling the given memory with a given fill pattern.
 * 
 * @param start: pointer to the start of the memory region to fill
 * @param size: size of the memory region to fill
 * @param is_free: Indicate if the pattern to use the fill the region should be 
 * an after free or after allocation pattern.
 */
__attribute__((weak)) void block_absorb_post_hook(void *start, size_t size, bool is_free);

/**
 * @brief Weak function called on every free block of memory allowing the user to implement
 * application specific checks on the memory.
 * 
 * @param start The start pointer to the memory of a block
 * @param size The size of the memory in the block
 * @param is_free Set to true when the memory belongs to a free block.
 * False if it belongs to an allocated block.
 * @return true The checks found no inconsistency in the memory
 * @return false The checks in the function highlighted an inconsistency in the memory
 */
__attribute__((weak))  bool tlsf_check_hook(void *start, size_t size, bool is_free);

#if defined(__cplusplus)
};
#endif

#endif
