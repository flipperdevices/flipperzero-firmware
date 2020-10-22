#pragma once

#include <stdint.h>

typedef struct Icon Icon;

uint8_t icon_get_width(const Icon* icon);
uint8_t icon_get_height(const Icon* icon);
const uint8_t * icon_get_data(const Icon* icon);
