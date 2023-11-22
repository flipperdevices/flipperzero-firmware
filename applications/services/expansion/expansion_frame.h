#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXPANSION_MAX_DATA_SIZE (64)

typedef enum {
    ExpansionFrameTypeHeartbeat = 1,
    ExpansionFrameTypeStatus = 2,
    ExpansionFrameTypeHold = 3,
    ExpansionFrameTypeBaudRate = 4,
    ExpansionFrameTypeControl = 5,
    ExpansionFrameTypeData = 6,
    ExpansionFrameTypeReserved,
} ExpansionFrameType;

typedef enum {
    ExpansionFrameErrorNone = 0x00,
    ExpansionFrameErrorUnknown = 0x01,
} ExpansionFrameError;

typedef enum {
    ExpansionFrameControlCommandStartRpc = 0x00,
    ExpansionFrameControlCommandStopRpc = 0x01,
} ExpansionFrameControlCommand;

#pragma pack(push, 1)

typedef struct {
    uint8_t type;
} ExpansionFrameHeader;

typedef struct {
} ExpansionFrameHeartbeat;

typedef struct {
    uint8_t error;
} ExpansionFrameStatus;

typedef struct {
    uint32_t eta_us;
} ExpansionFrameHold;

typedef struct {
    uint32_t baud;
} ExpansionFrameBaudRate;

typedef struct {
    uint8_t command;
} ExpansionFrameControl;

typedef struct {
    uint16_t size;
    uint8_t bytes[EXPANSION_MAX_DATA_SIZE];
} ExpansionFrameData;

typedef struct {
    ExpansionFrameHeader header;
    union {
        ExpansionFrameHeartbeat heartbeat;
        ExpansionFrameStatus status;
        ExpansionFrameHold hold;
        ExpansionFrameBaudRate baud_rate;
        ExpansionFrameControl control;
        ExpansionFrameData data;
    } content;
} ExpansionFrame;

#pragma pack(pop)

typedef size_t (*ExpansionFrameReceiveCallback)(uint8_t* data, size_t data_size, void* context);
typedef size_t (*ExpansionFrameSendCallback)(const uint8_t* data, size_t data_size, void* context);

bool expansion_frame_decode(
    ExpansionFrame* frame,
    ExpansionFrameReceiveCallback receive,
    void* context);

bool expansion_frame_encode(
    const ExpansionFrame* frame,
    ExpansionFrameSendCallback send,
    void* context);

#ifdef __cplusplus
}
#endif
