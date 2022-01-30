#pragma once
#include "subghz.h"
#include <stddef.h>
#include <stdint.h>
#include <m-string.h>
#include <furi_hal.h>

typedef void* (*SubGhzAlloc)(void);
typedef void (*SubGhzFree)(void* context);
typedef bool (*SubGhzSaveFile)(void* context, FlipperFile* flipper_file);
typedef bool (*SubGhzLoadFile)(void* context, FlipperFile* flipper_file, const char* file_path);

typedef void (*SubGhzDecoderReset)(void* decoder);
typedef void (*SubGhzDecode)(void* decoder, bool level, uint32_t duration);
typedef void (*SubGhzDecoderSerialization)(void* decoder, string_t output);

typedef void (*SubGhzEncoderStop)(void* encoder);
typedef bool (*SubGhzEncodeLoad)(void* encoder, uint64_t key, uint8_t count_bit, size_t repeat);
typedef LevelDuration (*SubGhzEncodeYield)(void* context);


