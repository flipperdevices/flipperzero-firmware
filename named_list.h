#ifndef __NAMED_LIST_H__
#define __NAMED_LIST_H__

#pragma once

#include <stddef.h>
#include <stdint.h>

struct __attribute__((__packed__)) named_list {
    const char* name;
    const uint8_t index;
    const uint8_t gen; // Bitfield of compatible generations
};

typedef struct named_list NamedList;

/* Get number of elements in a list
 * This is not very efficient as-is since it has to walk the whole list.
 */
size_t namedlist_cnt(const NamedList* list);

/* Returns the generation mask of the requested item in the list */
uint32_t namedlist_gen_get_pos(const NamedList* list, uint32_t pos);

/* Returns the generation mask of the item in the list that matches the
 * provided index. This will ultimately return the 0th element in the case
 * of the provided index not matching any of the list elements.
 */
uint32_t namedlist_gen_get_index(const NamedList* list, uint32_t index);

/* Returns the list position based on the provided index. If index is not
 * matched, the 0th position is returned. In most lists this is a "NONE"
 * indicator. e.g. No Move.
 */
uint32_t namedlist_pos_get(const NamedList* list, uint32_t index);

/* Get the item's index value from the position specified */
uint32_t namedlist_index_get(const NamedList* list, uint32_t pos);

/* Get a pointer to the item's name from an item's index */
const char* namedlist_name_get_index(const NamedList* list, uint32_t index);

/* Get a pointer to the item's name from a position */
const char* namedlist_name_get_pos(const NamedList* list, uint32_t pos);

#endif //__NAMED_LIST_H__
