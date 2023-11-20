#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ExpansionFrameTypeHeartbeat = 0x0,
    ExpansionFrameTypeError = 0x1,
    ExpansionFrameTypeHold = 0x02,
    ExpansionFrameTypeBaudRate = 0x03,
    ExpansionFrameTypeData = 0x04,
} ExpansionFrameType;

typedef enum {
    ExpansionFrameErrorNone = 0x00,
    ExpansionFrameErrorUnknown = 0x01,
} ExpansionFrameErrorType;

typedef enum {
    ExpansionFrameRpcCommandStart = 0x00,
    ExpansionFrameRpcCommandStop = 0x01,
    ExpansionFrameRpcCommandRestart = 0x02,
} ExpansionFrameRpcCommand;

#pragma pack(push, 1)

typedef struct {
    uint8_t leader : 4;
    uint8_t type : 4;
} ExpansionFrameHeader;

typedef struct {
} ExpansionFrameHeartbeat;

typedef struct {
    uint8_t error;
} ExpansionFrameError;

typedef struct {
    uint32_t estimate_us;
} ExpansionFrameHold;

typedef struct {
    uint32_t baud_rate;
} ExpansionFrameBaudRate;

typedef struct {
    uint8_t command;
} ExpansionFrameRpc;

typedef struct {
    uint16_t num;
    uint16_t size;
    uint8_t data[];
} ExpansionFrameData;

typedef struct {
    ExpansionFrameHeader header;
    union {
        ExpansionFrameHeartbeat heartbeat;
        ExpansionFrameError error;
        ExpansionFrameHold hold;
        ExpansionFrameBaudRate baud_rate;
        ExpansionFrameRpc rpc;
        ExpansionFrameData data;
    };
} ExpansionFrame;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
