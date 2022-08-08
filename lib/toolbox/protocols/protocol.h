#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/toolbox/level_duration.h>
#include <mlib/m-string.h>

typedef void* (*ProtocolAlloc)(void);
typedef void (*ProtocolFree)(void* protocol);
typedef void (*ProtocolSetData)(void* protocol, const uint8_t* data, size_t data_size);
typedef void (*ProtocolGetData)(void* protocol, uint8_t* data, size_t data_size);
typedef size_t (*ProtocolGetDataSize)(void* protocol);
typedef const char* (*ProtocolGetName)(void* protocol);
typedef const char* (*ProtocolGetManufacturer)(void* protocol);

typedef void (*ProtocolDecoderStart)(void* protocol);
typedef bool (*ProtocolDecoderFeed)(void* protocol, bool level, uint32_t duration);

typedef bool (*ProtocolEncoderStart)(void* protocol);
typedef LevelDuration (*ProtocolEncoderYield)(void* protocol);

typedef void (*ProtocolRenderData)(void* protocol, string_t result);
typedef bool (*ProtocolWriteData)(void* protocol, void* data);

typedef uint32_t (*ProtocolGetFeatures)(void* protocol);
typedef uint32_t (*ProtocolGetValidateCount)(void* protocol);

typedef struct {
    ProtocolDecoderStart start;
    ProtocolDecoderFeed feed;
} ProtocolDecoder;

typedef struct {
    ProtocolEncoderStart start;
    ProtocolEncoderYield yield;
} ProtocolEncoder;

typedef struct {
    ProtocolAlloc alloc;
    ProtocolFree free;
    ProtocolSetData set_data;
    ProtocolGetData get_data;
    ProtocolGetDataSize get_data_size;
    ProtocolGetName get_name;
    ProtocolGetManufacturer get_manufacturer;

    ProtocolDecoder decoder;
    ProtocolEncoder encoder;

    ProtocolRenderData render_data;
    ProtocolWriteData write_data;
    ProtocolGetFeatures get_features;
    ProtocolGetValidateCount get_validate_count;
} ProtocolBase;