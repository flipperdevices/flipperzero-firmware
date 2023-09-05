#pragma once

#include <gui/gui.h>
#include "../fnaf.h"

/// @brief Draw main menu
/// @param canvas Canvas instance
/// @param ctx Context
void draw_menu(Canvas* canvas, Fnaf* fnaf);

/// @brief Input handling
/// @param ctx Context
/// @return running bool
bool menu_input(Fnaf* fnaf);
