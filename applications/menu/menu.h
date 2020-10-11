#pragma once

typedef struct menu_t menu_t;
typedef struct menu_item_t menu_item_t;

// Add menu item to root menu
void menu_item_add(menu_t *menu, menu_item_t *item);

// Add menu item to settings menu
void menu_settings_item_add(menu_t *menu, menu_item_t *item);

// Menu controls
void menu_up(menu_t *menu);
void menu_down(menu_t *menu);
void menu_ok(menu_t *menu);
void menu_back(menu_t *menu);
void menu_exit(menu_t *menu);
