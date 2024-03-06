#pragma once

#include <m-array.h>

#define MAX_EXT_LEN 6

typedef enum { Item_Action, Item_Group } ItemType;

typedef struct Item {
    ItemType type;
    FuriString* name;
    FuriString* path;
    char ext[MAX_EXT_LEN + 1];
} Item;

ARRAY_DEF(ItemArray, Item, M_POD_OPLIST);

typedef struct ItemsView {
    FuriString* name;
    FuriString* path;
    ItemArray_t items;
} ItemsView;

/** Allocates and returns an ItemsView* which contains the list of
 * items to display for the given path.
 * 
 * @param   context App*
 * @param   path    FuriString*
 * @return  ItemsView*
*/
ItemsView* item_get_items_view_from_path(void* context, FuriString* path);

/** Free ItemsView
 * @param   items_view
*/
void item_items_view_free(ItemsView* items_view);

/** Prettify the name by removing a leading XX_, only if both X are digits,
 * as well as replace all '_' with ' '.
 * @param   name    FuriString*
*/
void item_prettify_name(FuriString* name);