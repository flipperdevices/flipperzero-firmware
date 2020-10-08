#include "menu_item.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct menu_item_t {
    uint8_t type;
    const char * label;
    void * icon;
    void * data;
};

menu_item_t * menu_item_alloc()
{
    menu_item_t * p = malloc(sizeof(menu_item_t));
    assert(p != NULL);
    memset(p, 0, sizeof(menu_item_t));
    return p;
}

menu_item_t * menu_item_alloc_init(uint8_t type, const char *label, void *icon, void *data)
{
    menu_item_t * menu_item = menu_item_alloc();
    menu_item_set_type(menu_item, type);
    menu_item_set_label(menu_item, label);
    menu_item_set_icon(menu_item, icon);
    menu_item_set_data(menu_item, data);
}


void menu_item_release(menu_item_t * menu_item)
{
    free(menu_item);
}

void menu_item_set_type(menu_item_t * menu_item, uint8_t type)
{
    menu_item->type = type;
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

void menu_item_set_data(menu_item_t * menu_item, void * data)
{
    menu_item->data = data;
}

void * menu_item_get_data(menu_item_t * menu_item)
{
    return menu_item->data;
}
