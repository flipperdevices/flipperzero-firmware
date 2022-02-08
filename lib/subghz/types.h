#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/flipper_file/flipper_file.h>
#include <lib/toolbox/level_duration.h>

#include "environment.h"


#define SUBGHZ_APP_FOLDER "/any/subghz"
#define SUBGHZ_APP_PATH_FOLDER "/any/subghz/saved"
#define SUBGHZ_RAW_FOLDER "/ext/subghz"
#define SUBGHZ_RAW_PATH_FOLDER "/ext/subghz/saved"
#define SUBGHZ_APP_EXTENSION ".sub"

#define SUBGHZ_PARSER_TAG "SubGhzParser"
#define SUBGHZ_KEY_FILE_VERSION 1
#define SUBGHZ_KEY_FILE_TYPE "Flipper SubGhz Key File"

#define SUBGHZ_RAW_FILE_VERSION 1
#define SUBGHZ_RAW_FILE_TYPE "Flipper SubGhz RAW File"

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
    SubGhzProtocolTypeUnknown = 0,
    SubGhzProtocolTypeStatic,
    SubGhzProtocolTypeDynamic,
    SubGhzProtocolTypeRAW,
} SubGhzProtocolType;

typedef enum {
    SubGhzProtocolFlag_RAW = (1 << 0),
    SubGhzProtocolFlag_Decodable =(1 << 1),
    SubGhzProtocolFlag_315 = (1 << 2),
    SubGhzProtocolFlag_433 = (1 << 3),
    SubGhzProtocolFlag_868 = (1 << 4),
    SubGhzProtocolFlag_AM = (1 << 5),
    SubGhzProtocolFlag_FM = (1 << 6),
} SubGhzProtocolFlag;

typedef struct {
    const char* name;
    SubGhzProtocolType type;
    SubGhzProtocolFlag flag;

    const SubGhzProtocolEncoder* encoder;
    const SubGhzProtocolDecoder* decoder;
} SubGhzProtocol;
