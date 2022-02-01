#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/flipper_file/flipper_file.h>
#include <lib/toolbox/level_duration.h>

#include "environment.h"

//
// Abstract method types
//

// Allocator and Deallocator
typedef void* (*SubGhzAlloc)(SubGhzEnvironment* environment);
typedef void (*SubGhzFree)(void* context);

// Load And Save
typedef bool (*SubGhzSaveFile)(void* context, FlipperFile* flipper_file);
typedef bool (*SubGhzLoadFile)(void* context, FlipperFile* flipper_file, const char* file_path);

// Serialize and Deserialize
typedef void (*SubGhzSerialize)(void* decoder, string_t output);
typedef void (*SubGhzDeserialize)(void* decoder, string_t output);

// Decoder specific
typedef void (*SubGhzDecoderFeed)(void* decoder, bool level, uint32_t duration);
typedef void (*SubGhzDecoderReset)(void* decoder);

// Encoder specific
typedef void (*SubGhzEncoderStop)(void* encoder);
typedef bool (*SubGhzEncoderLoad)(void* encoder, uint64_t key, uint8_t count_bit, size_t repeat);
typedef LevelDuration (*SubGhzEncoderYield)(void* context);

typedef struct {
    SubGhzAlloc alloc;
    SubGhzFree free;

    SubGhzDecoderFeed feed;
    SubGhzDecoderReset reset;

    SubGhzSerialize serialize;
    SubGhzSaveFile save_file;
} SubGhzProtocolDecoder;

typedef struct {
    SubGhzAlloc alloc;
    SubGhzFree free;

    SubGhzEncoderLoad load;
    SubGhzEncoderStop stop;
    SubGhzEncoderYield yield;

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
