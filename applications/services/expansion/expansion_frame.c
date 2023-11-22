#include "expansion_frame.h"

size_t expansion_frame_get_encoded_size(const ExpansionFrame* frame) {
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

size_t expansion_frame_get_remaining_size(const ExpansionFrame* frame, size_t received_size) {
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
        content_size = received_content_size < sizeof(frame->content.data.size) ?
                           sizeof(frame->content.data.size) :
                           ((sizeof(frame->content.data.size) + frame->content.data.size));
        break;
    default:
        return SIZE_MAX;
    }

    return content_size > received_content_size ? content_size - received_content_size : 0;
}

bool expansion_frame_decode(
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

bool expansion_frame_encode(
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
