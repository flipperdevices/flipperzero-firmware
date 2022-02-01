#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/flipper_file/flipper_file.h>
#include <lib/toolbox/level_duration.h>

#include <m-string.h>
#include <furi_hal.h>

typedef void* (*SubGhzAlloc)(void);

typedef void (*SubGhzFree)(void* context);

typedef bool (*SubGhzSaveFile)(void* context, FlipperFile* flipper_file);

typedef bool (*SubGhzLoadFile)(void* context, FlipperFile* flipper_file, const char* file_path);

typedef void (*SubGhzDecoderReset)(void* decoder);

typedef void (*SubGhzDecoderSerialization)(void* decoder, string_t output);

typedef void (*SubGhzDecode)(void* decoder, bool level, uint32_t duration);

typedef void (*SubGhzEncoderStop)(void* encoder);

typedef bool (*SubGhzEncodeLoad)(void* encoder, uint64_t key, uint8_t count_bit, size_t repeat);

typedef LevelDuration (*SubGhzEncodeYield)(void* context);

typedef enum {
    SubGhzProtocolStatusNoProtocol = -1,
    SubGhzProtocolStatusUnknown = 0,
    SubGhzProtocolStatusNoInit,
    SubGhzProtocolStatusOk,
} SubGhzProtocolStatus;

typedef struct {
    SubGhzAlloc alloc;
    SubGhzDecode decode;
    SubGhzDecoderReset reset;
    SubGhzFree free;
    SubGhzSaveFile save_file;
    SubGhzDecoderSerialization serialization;
} SubGhzProtocolDecoder;

typedef struct {
    SubGhzAlloc alloc;
    SubGhzEncodeLoad load;
    SubGhzEncoderStop stop;
    SubGhzFree free;
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
    //SubGhzProtocolCommonflags_ flag;
} SubGhzProtocolSpecification;

typedef struct {
    const SubGhzProtocolSpecification specification;
    const SubGhzProtocolEncoder* encoder;
    const SubGhzProtocolDecoder* decoder;
} SubGhzProtocol;
