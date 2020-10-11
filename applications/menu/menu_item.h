#pragma once

#include <stdint.h>
#include <m-array.h>

typedef enum {
    MENU_ITEM_TYPE_MENU     = 0x00,
    MENU_ITEM_TYPE_FUNCTION = 0x01
} menu_item_type_t;

typedef void (*menu_function_t)();
typedef struct menu_item_t menu_item_t;

ARRAY_DEF(menu_items_array, menu_item_t *, M_PTR_OPLIST);

menu_item_t * menu_item_alloc_menu(const char *label, void *icon);

menu_item_t * menu_item_alloc_function(const char *label, void *icon, menu_function_t function);

void menu_item_release(menu_item_t *menu_item);

menu_item_t * menu_item_get_parent(menu_item_t *menu_item);

void menu_item_subitem_add(menu_item_t *menu_item, menu_item_t *sub_item);

menu_item_type_t menu_item_get_type(menu_item_t *menu_item);

void menu_item_set_label(menu_item_t *menu_item, const char *label);
const char * menu_item_get_label(menu_item_t *menu_item);

void menu_item_set_icon(menu_item_t *menu_item, void *icon);
void * menu_item_get_icon(menu_item_t *menu_item);

menu_items_array_t * menu_item_get_subitems(menu_item_t *menu_item);

menu_function_t menu_item_get_function(menu_item_t *menu_item);

