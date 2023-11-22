/**
 * @file expansion_protocol.h
 */
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

static size_t expansion_frame_get_encoded_size(const ExpansionFrame* frame) {
    switch(frame->header.type) {
    case ExpansionFrameTypeHeartbeat:
        return sizeof(frame->header);
    case ExpansionFrameTypeStatus:
        return sizeof(frame->header) + sizeof(frame->content.status);
    case ExpansionFrameTypeHold:
        return sizeof(frame->header) + sizeof(frame->content.hold);
    case ExpansionFrameTypeBaudRate:
        return sizeof(frame->header) + sizeof(frame->content.baud_rate);
    case ExpansionFrameTypeControl:
        return sizeof(frame->header) + sizeof(frame->content.control);
    case ExpansionFrameTypeData:
        return sizeof(frame->header) + sizeof(frame->content.data.size) + frame->content.data.size;
    default:
        return 0;
    }
}

static size_t
    expansion_frame_get_remaining_size(const ExpansionFrame* frame, size_t received_size) {
    if(received_size < sizeof(ExpansionFrameHeader)) return sizeof(ExpansionFrameHeader);

    const size_t received_content_size = received_size - sizeof(ExpansionFrameHeader);
    size_t content_size;

    switch(frame->header.type) {
    case ExpansionFrameTypeHeartbeat:
        content_size = 0;
        break;
    case ExpansionFrameTypeStatus:
        content_size = sizeof(frame->content.status);
        break;
    case ExpansionFrameTypeHold:
        content_size = sizeof(frame->content.hold);
        break;
    case ExpansionFrameTypeBaudRate:
        content_size = sizeof(frame->content.baud_rate);
        break;
    case ExpansionFrameTypeControl:
        content_size = sizeof(frame->content.control);
        break;
    case ExpansionFrameTypeData:
        if(received_content_size < sizeof(frame->content.data.size)) {
            content_size = sizeof(frame->content.data.size);
        } else {
            content_size = sizeof(frame->content.data.size) + frame->content.data.size;
        }
        break;
    default:
        return SIZE_MAX;
    }

    return content_size > received_content_size ? content_size - received_content_size : 0;
}

static bool expansion_frame_decode(
    ExpansionFrame* frame,
    ExpansionFrameReceiveCallback receive,
    void* context) {
    size_t total_size = 0;
    size_t remaining_size;

    while(true) {
        remaining_size = expansion_frame_get_remaining_size(frame, total_size);
        if(remaining_size == 0 || remaining_size == SIZE_MAX) break;

        const size_t received_size =
            receive((uint8_t*)frame + total_size, remaining_size, context);
        if(received_size == 0) break;

        total_size += received_size;
    }

    return remaining_size == 0;
}

static bool expansion_frame_encode(
    const ExpansionFrame* frame,
    ExpansionFrameSendCallback send,
    void* context) {
    const size_t encoded_size = expansion_frame_get_encoded_size(frame);

    if(encoded_size != 0) {
        return send((const uint8_t*)frame, encoded_size, context) == encoded_size;
    } else {
        return false;
    }
}

#ifdef __cplusplus
}
#endif
