#pragma once

#include <stdint.h>
#include "canvas.h"

void elements_scrollbar(Canvas* canvas, uint8_t pos, uint8_t total);

void elements_frame(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
