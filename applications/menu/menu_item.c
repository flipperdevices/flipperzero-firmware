#include "menu_item.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <furi.h>

struct menu_item_t {
    menu_item_type_t type;
    const char * label;
    void * icon;
    menu_item_t * parent;
    void * data;
};

menu_item_t * menu_item_alloc()
{
    menu_item_t * p = furi_alloc(sizeof(menu_item_t));
    return p;
}

menu_item_t * menu_item_alloc_menu(const char *label, void *icon)
{
    menu_item_t * menu_item = menu_item_alloc();

    menu_item->type = MENU_ITEM_TYPE_MENU;
    menu_item->label = label;
    menu_item->icon = icon;

    menu_items_t *items = furi_alloc(sizeof(menu_items_t));
    menu_items_init(*items);
    menu_item->data = items;

    return menu_item;
}

menu_item_t * menu_item_alloc_function(const char *label, void *icon, menu_function_t function)
{
    menu_item_t * menu_item = menu_item_alloc();

    menu_item->type = MENU_ITEM_TYPE_FUNCTION;
    menu_item->label = label;
    menu_item->icon = icon;
    menu_item->data = function;

    return menu_item;
}

void menu_item_release(menu_item_t * menu_item)
{
    if (menu_item->type == MENU_ITEM_TYPE_MENU) {
        //TODO: release subitems
    }
    free(menu_item);
}

menu_item_t * menu_item_get_parent(menu_item_t * menu_item)
{
    return menu_item->parent;
}

void menu_subitem_add(menu_item_t * menu_item, menu_item_t * sub_item)
{
    assert(menu_item->type == MENU_ITEM_TYPE_MENU);
    menu_items_t *items = menu_item->data;
    sub_item->parent = menu_item;
    menu_items_push_back(*items, sub_item);
}

uint8_t menu_item_get_type(menu_item_t * menu_item)
{
    return menu_item->type;
}

void menu_item_set_label(menu_item_t * menu_item, const char * label)
{
    menu_item->label = label;
}

const char * menu_item_get_label(menu_item_t * menu_item)
{
    return menu_item->label;
}

void menu_item_set_icon(menu_item_t * menu_item, void * icon)
{
    menu_item->icon = icon;
}

void * menu_item_get_icon(menu_item_t * menu_item)
{
    return menu_item->icon;
}

menu_items_t * menu_item_get_subitems(menu_item_t * menu_item)
{
    assert(menu_item->type == MENU_ITEM_TYPE_MENU);
    return menu_item->data;
}

menu_function_t menu_item_get_function(menu_item_t * menu_item)
{
    assert(menu_item->type == MENU_ITEM_TYPE_FUNCTION);
    return menu_item->data;
}
