#pragma once

#include <stdint.h>

#define MENU_ITEM_TYPE_SUBMENU      0x00
#define MENU_ITEM_TYPE_FUNCTION     0x01

typedef struct menu_item_t menu_item_t;

menu_item_t * menu_item_alloc();
menu_item_t * menu_item_alloc_init(uint8_t type, const char *label, void *icon, void *data);

void menu_item_release(menu_item_t * menu_item);

void menu_item_set_type(menu_item_t * menu_item, uint8_t type);
uint8_t menu_item_get_type(menu_item_t * menu_item);

void menu_item_set_label(menu_item_t * menu_item, const char * label);
const char * menu_item_get_label(menu_item_t * menu_item);

void menu_item_set_icon(menu_item_t * menu_item, void * icon);
void * menu_item_get_icon(menu_item_t * menu_item);

void menu_item_set_data(menu_item_t * menu_item, void * data);
void * menu_item_get_data(menu_item_t * menu_item);

