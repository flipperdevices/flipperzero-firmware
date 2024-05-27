#include <stddef.h>
#include <stdint.h>

#include <named_list.h>

/* Get number of elements in a list
 * This is not very efficient as-is since it has to walk the whole list.
 */
size_t namedlist_cnt(const NamedList* list) {
    size_t i;

    for(i = 0;; i++) {
        if(list[i].name == NULL) return i;
    }
}

/* Returns the generation mask of the requested item in the list */
uint32_t namedlist_gen_get_pos(const NamedList* list, uint32_t pos) {
    return list[pos].gen;
}

/* Returns the generation mask of the item in the list that matches the
 * provided index. This will ultimately return the 0th element in the case
 * of the provided index not matching any of the list elements.
 */
uint32_t namedlist_gen_get_index(const NamedList* list, uint32_t index) {
    return list[namedlist_pos_get(list, index)].gen;
}

/* Returns the list position based on the provided index. If index is not
 * matched, the 0th position is returned. In most lists this is a "NONE"
 * indicator. e.g. No Move.
 */
uint32_t namedlist_pos_get(const NamedList* list, uint32_t index) {
    int i;

    for(i = 0;; i++) {
        if(list[i].name == NULL) break;
        if(index == list[i].index) return i;
    }

    /* This will return the first entry in case index is not matched.
     * Could be surprising at runtime.
     */
    return 0;
}

/* Get the item's index value from the position specified */
uint32_t namedlist_index_get(const NamedList* list, uint32_t pos) {
    return list[pos].index;
}

/* Get a pointer to the item's name from an item's index */
const char* namedlist_name_get_index(const NamedList* list, uint32_t index) {
    return list[namedlist_pos_get(list, index)].name;
}

/* Get a pointer to the item's name from a position */
const char* namedlist_name_get_pos(const NamedList* list, uint32_t pos) {
    return list[pos].name;
}
