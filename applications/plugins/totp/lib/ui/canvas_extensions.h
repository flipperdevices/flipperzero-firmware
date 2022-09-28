#ifndef _TOTPCANVAS_EXTENSIONS_H_
#define _TOTPCANVAS_EXTENSIONS_H_

#include <inttypes.h>
#include <gui/gui.h>
#include <furi.h>
#include <furi_hal.h>

void canvas_draw_dots(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *dots);

#endif
