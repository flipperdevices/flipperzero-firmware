#pragma once

#include <m-array.h>

// Max length of a filename, final path element only
#define MAX_NAME_LEN 64
#define MAX_EXT_LEN 6

/** Defines an individual item action or item group. Each object contains
 * the relevant file and type information needed to both render correctly
 * on-screen as well as to perform that action.
*/

typedef enum {
    Item_SubGhz,
    Item_RFID,
    Item_IR,
    Item_NFC,
    Item_Playlist,
    Item_Group,
    Item_Settings,
    Item_Unknown,
    Item_count
} ItemType;

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
 * items to display for the given path. Contains everything needed
 * to render a scene_items.
 * 
 * @param   context App*
 * @param   path    FuriString*
 * @return  ItemsView*
*/
ItemsView* item_get_items_view_from_path(void* context, const FuriString* path);

/** Free ItemsView
 * @param   items_view
*/
void item_items_view_free(ItemsView* items_view);

/** Prettify the name by removing a leading XX_, only if both X are digits,
 * as well as replace all '_' with ' '.
 * @param   name    FuriString*
*/
void item_prettify_name(FuriString* name);

/** Return the ItemType enum for the given extension
 * @param   ext     File extension
*/
ItemType item_get_item_type_from_extension(const char* ext);