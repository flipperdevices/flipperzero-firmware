#pragma once
#include "desktop/desktop_settings/desktop_settings.h"
#include <gui/canvas.h>
#include <stdint.h>
#include <furi_hal.h>

void desktop_views_helpers_pin_code_draw(Canvas* canvas, uint8_t width, uint8_t hight, const PinCode* pin_code, bool hidden, bool finished);
bool desktop_views_helpers_pin_code_are_equal(PinCode* pin_code1, PinCode* pin_code2);

