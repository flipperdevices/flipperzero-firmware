#pragma once
#include <stdint.h>

#define DEF_TIMINGS(bit1m, bit1s, bit0m, bit0s, duty, carrier)      \
static const struct {                                               \
    uint32_t bit1_mark;                                             \
    uint32_t bit1_space;                                            \
    uint32_t bit0_mark;                                             \
    uint32_t bit0_space;                                            \
    float duty_cycle;                                               \
    uint32_t carrier_frequency;                                     \
} i_encoder_timings = {                                             \
    .bit1_mark = bit1m,                                             \
    .bit1_space = bit1s,                                            \
    .bit0_mark = bit0m,                                             \
    .bit0_space = bit0s,                                            \
    .duty_cycle = duty,                                             \
    .carrier_frequency = carrier,                                   \
};

