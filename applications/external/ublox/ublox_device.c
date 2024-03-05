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
        free(frame->payload);
        free(frame);
        return NULL;
    }

    if(ck_b != frame->ck_b) {
        FURI_LOG_E(TAG, "checksum B doesn't match! expected 0x%x, got 0x%x", ck_b, frame->ck_b);
        free(frame->payload);
        free(frame);
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

UbloxMessage* ublox_i2c_transfer(UbloxMessage* message_tx, uint8_t read_length) {
    if(!furi_hal_i2c_is_device_ready(
           &furi_hal_i2c_handle_external,
           UBLOX_I2C_ADDRESS << 1,
           furi_ms_to_ticks(I2C_TIMEOUT_MS))) {
        FURI_LOG_E(TAG, "device not ready");
        return NULL;
    }

    // Either our I2C implementation is broken or the GPS's is, so we
    // end up reading a lot more data than we need to. That means that
    // the I2C comm code for this app is a little bit of a hack, but
    // it works fine and is fast enough, so I don't really care. It
    // certainly doesn't break the GPS.
    if(!furi_hal_i2c_tx(
           &furi_hal_i2c_handle_external,
           UBLOX_I2C_ADDRESS << 1,
           message_tx->message,
           message_tx->length,
           furi_ms_to_ticks(I2C_TIMEOUT_MS))) {
        FURI_LOG_E(TAG, "error writing message to GPS");
        return NULL;
    }
    uint8_t* response = malloc((size_t)read_length);
    // The GPS sends 0xff until it has a complete message to respond
    // with. We have to wait until it stops sending that. (Why this
    // works is a little bit...uh, well, I don't know. Shouldn't reading
    // more bytes make it so that the data is completely read out and no
    // longer available?)

    //FURI_LOG_I(TAG, "start ticks at %lu", furi_get_tick()); // returns ms
    while(true) {
        if(!furi_hal_i2c_rx(
               &furi_hal_i2c_handle_external,
               UBLOX_I2C_ADDRESS << 1,
               response,
               1,
               furi_ms_to_ticks(I2C_TIMEOUT_MS))) {
            FURI_LOG_E(TAG, "error reading first byte of response");
            free(response);
            return NULL;
        }

        // checking with 0xb5 prevents strange bursts of junk data from becoming an issue.
        if(response[0] != 0xff && response[0] == 0xb5) {
            //FURI_LOG_I(TAG, "read rest of message at %lu", furi_get_tick());
            if(!furi_hal_i2c_rx(
                   &furi_hal_i2c_handle_external,
                   UBLOX_I2C_ADDRESS << 1,
                   &(response[1]),
                   read_length - 1, // first byte already read
                   furi_ms_to_ticks(I2C_TIMEOUT_MS))) {
                FURI_LOG_E(TAG, "error reading rest of response");
                free(response);
                return NULL;
            }
            break;
        }
        furi_delay_ms(1);
    }

    UbloxMessage* message_rx = malloc(sizeof(UbloxMessage));
    message_rx->message = response;
    message_rx->length = read_length;
    return message_rx; // message_rx->message needs to be freed later
}
