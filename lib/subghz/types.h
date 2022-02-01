#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/flipper_file/flipper_file.h>
#include <lib/toolbox/level_duration.h>

#include <m-string.h>
#include <furi_hal.h>

// Allocator and Deallocator
typedef void* (*SubGhzAlloc)(void);
typedef void (*SubGhzFree)(void* context);

// Load And Save
typedef bool (*SubGhzSaveFile)(void* context, FlipperFile* flipper_file);
typedef bool (*SubGhzLoadFile)(void* context, FlipperFile* flipper_file, const char* file_path);

// Decoder specific
typedef void (*SubGhzDecoderFeed)(void* decoder, bool level, uint32_t duration);
typedef void (*SubGhzDecoderReset)(void* decoder);
typedef void (*SubGhzDecoderSerialize)(void* decoder, string_t output);

// Encoder specific
typedef void (*SubGhzEncoderStop)(void* encoder);
typedef bool (*SubGhzEncodeLoad)(void* encoder, uint64_t key, uint8_t count_bit, size_t repeat);
typedef LevelDuration (*SubGhzEncodeYield)(void* context);

typedef struct {
    SubGhzAlloc alloc;
    SubGhzFree free;

    SubGhzDecoderFeed feed;
    SubGhzDecoderReset reset;
    SubGhzDecoderSerialize serialize;

    SubGhzSaveFile save_file;
} SubGhzProtocolDecoder;

typedef struct {
    SubGhzAlloc alloc;
    SubGhzFree free;

    SubGhzEncodeLoad load;
    SubGhzEncoderStop stop;
    SubGhzEncodeYield yield;

    SubGhzLoadFile load_file;
} SubGhzProtocolEncoder;

typedef enum {
    SubGhzProtocolCommonTypeUnknown_ = 0,
    SubGhzProtocolCommonTypeStatic_,
    SubGhzProtocolCommonTypeDynamic_,
    SubGhzProtocolCommonTypeRAW_,
    //.....
} SubGhzProtocolCommonType_;

typedef struct {
    const char* name;
    SubGhzProtocolCommonType_ type;

    const SubGhzProtocolEncoder* encoder;
    const SubGhzProtocolDecoder* decoder;
} SubGhzProtocol;
