#pragma once

#include <furi.h>

typedef struct FlipboardLeds FlipboardLeds;

typedef enum {
    LedId1 = 1,
    LedId2 = 1 << 1,
    LedId3 = 1 << 2,
    LedId4 = 1 << 3,
} LedIds;

FlipboardLeds* flipboard_leds_alloc();
void flipboard_leds_reset(FlipboardLeds* leds);
void flipboard_leds_free(FlipboardLeds* leds);
void flipboard_leds_set(FlipboardLeds* leds, LedIds led, uint32_t color);
void flipboard_leds_update(FlipboardLeds* leds);
void flipboard_status_led(FlipboardLeds* leds, bool glow);