#ifndef UI_DRAWING_H
#define UI_DRAWING_H

#include "calculator_state.h"
#include <gui/canvas.h>

void drawElement(Canvas* canvas, const char* str, int x, int y, int width, int height, Font font);
void generate_calculator_layout(Canvas* canvas);
void draw_highlighted_cell(Canvas* canvas, short x, short y, short width, short height);
void displayResult(Canvas* canvas, char* result, int x, int y);
void calculator_draw_callback(Canvas* canvas, void* ctx);

#endif // UI_DRAWING_H
