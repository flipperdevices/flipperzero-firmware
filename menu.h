#pragma once

#include <gui/gui.h>

typedef enum {
    NewGame,
    Continue,
    Exit,
}MenuItems;

/// @brief Draw main menu
/// @param canvas Canvas instance
/// @param ctx Context
void draw_menu(Canvas* canvas, void* ctx);

/// @brief Input handling
/// @param ctx Context
/// @return running bool
bool menu_input(void* ctx);
