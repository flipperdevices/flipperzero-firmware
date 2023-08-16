#pragma once

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RgbColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RgbColor;

typedef struct HsvColor {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HsvColor;

int rgbcmp(const RgbColor* a, const RgbColor* b);

int hsvcmp(const HsvColor* a, const HsvColor* b);

RgbColor hsv2rgb(HsvColor hsv);

HsvColor rgb2hsv(RgbColor rgb);

#ifdef __cplusplus
}
#endif
