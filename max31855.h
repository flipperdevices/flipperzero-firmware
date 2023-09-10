#pragma once

#include <stdint.h>
#include <stdbool.h>

static inline float max31855_unpack_temp(uint32_t data) {
    data >>= 18;
    return ((data & 0x4000) ? -0.25f : 0.25f) * (data & 0x3FFF);
}

static inline float max31855_unpack_internal_temp(uint32_t data) {
    data >>= 4;
    return ((data & 0x800) ? -0.0625f : 0.0625f) * (data & 0x7FF);
}

static inline bool max31855_unpack_fault(uint32_t data) {
    return data & (1u << 16);
}

static inline bool max31855_unpack_oc(uint32_t data) {
    return data & (1u << 0);
}

static inline bool max31855_unpack_scg(uint32_t data) {
    return data & (1u << 1);
}

static inline bool max31855_unpack_scv(uint32_t data) {
    return data & (1u << 2);
}

void max31855_open();

void max31855_close();

uint32_t max31855_read();
