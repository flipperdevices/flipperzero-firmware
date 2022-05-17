#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/toolbox/level_duration.h>

typedef void* (*ProtocolAlloc)(void);
typedef void (*ProtocolFree)(void* protocol);
typedef void (*ProtocolSetData)(void* protocol, const uint8_t* data, size_t data_size);
typedef void (*ProtocolGetData)(void* protocol, uint8_t* data, size_t data_size);
typedef size_t (*ProtocolGetDataSize)(void* protocol);
typedef const char* (*ProtocolGetName)(void* protocol);
typedef const char* (*ProtocolGetManufacturer)(void* protocol);

typedef void (*ProtocolDecoderStart)(void* protocol);
typedef bool (*ProtocolDecoderFeed)(void* protocol, bool level, uint32_t duration);
typedef void (*ProtocolDecoderReset)(void* protocol);

typedef bool (*ProtocolEncoderStart)(void* protocol);
typedef LevelDuration (*ProtocolEncoderYield)(void* protocol);
typedef void (*ProtocolEncoderReset)(void* protocol);

typedef struct {
    ProtocolDecoderStart start;
    ProtocolDecoderFeed feed;
    ProtocolDecoderReset reset;
} ProtocolDecoder;

typedef struct {
    ProtocolEncoderStart start;
    ProtocolEncoderYield yield;
    ProtocolEncoderReset reset;
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
} ProtocolBase;