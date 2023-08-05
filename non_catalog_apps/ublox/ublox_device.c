// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "ublox_device.h"

#define TAG "ublox_device"

UbloxMessage* ublox_frame_to_bytes(UbloxFrame* frame) {
    uint32_t message_size = 8 + frame->len;
    // Found the issue! frame_bytes isn't being freed. This function
    // should always have returned a pointer.
    uint8_t* frame_bytes = malloc(message_size);
    frame->sync1 = 0xb5;
    frame->sync2 = 0x62;
    frame_bytes[0] = frame->sync1;
    frame_bytes[1] = frame->sync2;
    frame_bytes[2] = frame->class;
    frame_bytes[3] = frame->id;
    frame_bytes[4] = frame->len & 0xff;
    frame_bytes[5] = (frame->len & 0xff) >> 8;

    if(frame->len != 0) {
        for(int i = 0; i < frame->len; i++) {
            frame_bytes[6 + i] = frame->payload[i];
        }
    }

    frame->ck_a = 0;
    frame->ck_b = 0;
    // checksum is calculated over class, id, length, and payload
    for(int i = 2; i < 2 + (frame->len + 4); i++) {
        frame->ck_a = frame->ck_a + frame_bytes[i];
        frame->ck_b = frame->ck_b + frame->ck_a;
    }

    frame_bytes[message_size - 2] = frame->ck_a;
    frame_bytes[message_size - 1] = frame->ck_b;

    UbloxMessage* m = malloc(sizeof(UbloxMessage));
    m->message = frame_bytes;
    m->length = message_size;

    return m;
}

void ublox_message_free(UbloxMessage* message) {
    if(message != NULL) {
        if(message->message != NULL) {
            free(message->message);
        } /*else {
      FURI_LOG_I(TAG, "message free: message->message == NULL");
      }*/
        free(message);
    } /*else {
    FURI_LOG_I(TAG, "message free: message == NULL");
    }*/
}

// Pointer, because we are assigning a pointer in the returned frame.
UbloxFrame* ublox_bytes_to_frame(UbloxMessage* message) {
    if(message->length < 8) {
        FURI_LOG_I(TAG, "message length in bytes_to_frame < 8, = 0x%x", message->length);
        // minimum 8 bytes in a message (message with no payload)
        return NULL;
    }

    UbloxFrame* frame = malloc(sizeof(UbloxFrame));

    if(message->message[0] != 0xb5) {
        FURI_LOG_E(TAG, "message[0] != 0xb5, = 0x%x", message->message[0]);
        free(frame);
        return NULL;
    }

    frame->sync1 = message->message[0];

    if(message->message[1] != 0x62) {
        FURI_LOG_E(TAG, "Message[1] != 0x62, = 0x%x", message->message[1]);
        free(frame);
        return NULL;
    }

    frame->sync2 = message->message[1];

    frame->class = message->message[2];
    frame->id = message->message[3];

    // little-endian
    frame->len = (message->message[5] << 8) | (message->message[4]);

    // frame->len must be initialized before malloc (duh, but I made that mistake...)
    frame->payload = malloc(frame->len);
    //FURI_LOG_I(TAG, "frame->len: %d", frame->len);
    for(int i = 6; i < 6 + frame->len; i++) {
        frame->payload[i - 6] = message->message[i];
    }

    frame->ck_a = message->message[6 + frame->len];
    frame->ck_b = message->message[6 + frame->len + 1];

    // Test checksum
    uint8_t ck_a = 0, ck_b = 0;
    for(int i = 2; i < 2 + (frame->len + 4); i++) {
        ck_a = ck_a + message->message[i];
        ck_b = ck_b + ck_a;
    }

    if(ck_a != frame->ck_a) {
        FURI_LOG_E(TAG, "checksum A doesn't match! expected 0x%x, got 0x%x", ck_a, frame->ck_a);
        free(frame);
        free(frame->payload);
        return NULL;
    }

    if(ck_b != frame->ck_b) {
        FURI_LOG_E(TAG, "checksum B doesn't match! expected 0x%x, got 0x%x", ck_b, frame->ck_b);
        free(frame);
        free(frame->payload);
        return NULL;
    }

    return frame;
}

void ublox_frame_free(UbloxFrame* frame) {
    if(frame != NULL) {
        if(frame->payload != NULL) {
            free(frame->payload);
        } /* else {
      FURI_LOG_I(TAG, "frame free: frame->payload == NULL");
      }*/
        free(frame);
    } /* else {
    FURI_LOG_I(TAG, "frame free: frame == NULL");
    }*/
}
