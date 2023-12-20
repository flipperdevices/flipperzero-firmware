#ifndef UI_DRAWING_H
#define UI_DRAWING_H

#include "calculator_state.h"
#include <gui/canvas.h>

void generate_calculator_layout(Canvas* canvas);
void draw_highlighted_cell(Canvas* canvas, short x, short y, short width, short height);
void displayResultInTwoLines(Canvas* canvas, char* result, int x, int y);
void calculator_draw_callback(Canvas* canvas, void* ctx);

#endif // UI_DRAWING_H
