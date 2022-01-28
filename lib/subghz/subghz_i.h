#pragma once
#include "subghz.h"
#include <stddef.h>
#include <stdint.h>
#include <m-string.h>
#include <furi_hal.h>

// typedef struct {
//     uint32_t min_split_time;
//     uint32_t silence_time;
//     uint16_t preamble_mark;
//     uint16_t preamble_space;
//     uint16_t bit1_mark;
//     uint16_t bit1_space;
//     uint16_t bit0_mark;
//     uint16_t bit0_space;
//     uint32_t preamble_tolerance;
//     uint32_t bit_tolerance;
// } SubGhzTimings;

//typedef const SubGhzProtocolSpecification* (*SubGhzGetProtocolSpec)(void);

typedef void* (*SubGhzAlloc)(void);
typedef void (*SubGhzFree)(void* context);

typedef void (*SubGhzDecoderReset)(void* context);
typedef void (*SubGhzDecode)(void* ctx, bool level, uint32_t duration);
//typedef void (*SubGhzProtocolTextCallback)(string_t text, void* context);
//typedef SubGhzMessage* (*SubGhzDecoderCheckReady)(void*);

typedef void (*SubGhzEncoderReset)(void* encoder);
typedef bool (*SubGhzEncode)(void* encoder);

// static inline uint8_t reverse(uint8_t value) {
//     uint8_t reverse_value = 0;
//     for(int i = 0; i < 8; ++i) {
//         reverse_value |= (value & (0x01 << i)) ? 1 << (7 - i) : 0;
//     }

//     return reverse_value;
// }
