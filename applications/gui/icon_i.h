/**
 * @file icon_i.h
 * GUI: internal Icon API
 */

#include "icon.h"

struct Icon {
    uint8_t width;
    uint8_t height;
    uint8_t frame_count;
    uint8_t frame_rate;
    const uint8_t** frames;
};
