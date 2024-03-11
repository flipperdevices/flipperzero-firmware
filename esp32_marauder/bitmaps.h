#pragma once

#include <stddef.h>
#include <stdint.h>

#define FLIPPER_SCREEN_WIDTH (128)
#define FLIPPER_SCREEN_HEIGHT (64)

#define FLIPPER_BITMAP_SIZE (FLIPPER_SCREEN_WIDTH * FLIPPER_SCREEN_HEIGHT / 8)

extern const uint8_t bitmap_malveke_splash_screen[FLIPPER_BITMAP_SIZE];

void bitmap_xbm_to_screen_frame(uint8_t* dst, const uint8_t* src, size_t w, size_t h);