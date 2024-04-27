/**
 * @file protocol_demo1.c
 * @brief Demo protocol, doesn't do read/emulate, but does write data to the T5577 tag.
 * @details This protocol is a demo protocol that doesn't actually do any reading or
 * emulating, but does write data to the T5577 tag.  This is useful for testing the
 * write functionality of the T5577 tag.  You can change the data in the write_data
 * function to write different data to the T5577 tag.
 * 
 * A sample file looks like (test.rfid):
 * Filetype: Flipper RFID key
 * Version: 1
 * Key type: Demo1
 * Data: 42 13
 * 
 * You can also go to 125 kHz RFID, Add Manually, Derek Jamison Demo1, enter the
 * data "42 13", click save, choose a name, and click save again.  This will create
 * the test.rfid file.
 * 
 * For more information, see the youtube video at https://youtu.be/nSCIOz2bVR0
*/
#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <bit_lib/bit_lib.h>
#include "lfrfid_protocols.h"

#define TAG "Demo1"

#define DEMO1_DECODED_DATA_SIZE (2)

typedef struct {
    uint8_t data[DEMO1_DECODED_DATA_SIZE];
    uint32_t feed_counter;
} ProtocolDemo1;

ProtocolDemo1* protocol_demo1_alloc(void) {
    ProtocolDemo1* protocol = malloc(sizeof(ProtocolDemo1));
    return protocol;
};

void protocol_demo1_free(ProtocolDemo1* protocol) {
    free(protocol);
};

uint8_t* protocol_demo1_get_data(ProtocolDemo1* protocol) {
    return protocol->data;
};

void protocol_demo1_decoder_start(ProtocolDemo1* protocol) {
    // This would normally reset the decoder state (clearing encoded_data).
    protocol->feed_counter = 0;
};

bool protocol_demo1_decoder_feed(ProtocolDemo1* protocol, bool level, uint32_t duration) {
    // Normally, we would decode the data here & see if the data looks like a full packet.
    // NOTE: duration is in us.
    // When it does, you would return true.
    UNUSED(level);
    UNUSED(duration);

    // Prints "data: [0]:42 [1]:13" to the log for our sample file.
    FURI_LOG_D(TAG, "data: [0]:%02x [1]:%02x", protocol->data[0], protocol->data[1]);
    if(protocol->data[0] == 0x42) {
        protocol->feed_counter++;
    }

    return protocol->feed_counter % 100 == 99;
};

bool protocol_demo1_encoder_start(ProtocolDemo1* protocol) {
    // This would normally reset the encoder state (populating encoded_data).
    UNUSED(protocol);
    return true;
};

LevelDuration protocol_demo1_encoder_yield(ProtocolDemo1* protocol) {
    // This would normally return the next level/duration pair to send.
    // Note: duration is in ticks (8us).
    UNUSED(protocol);
    return level_duration_make(true, 1);
};

void protocol_demo1_render_data(ProtocolDemo1* protocol, FuriString* result) {
    // This would normally use protocol->data to populate the result string.
    UNUSED(protocol);
    furi_string_printf(result, "ATA5577 Maker %02x %02x", protocol->data[0], protocol->data[1]);
}

void protocol_demo1_render_brief_data(ProtocolDemo1* protocol, FuriString* result) {
    // This would normally use protocol->data to populate the brief result string.
    UNUSED(protocol);
    furi_string_printf(result, "T5577 %02x %02x", protocol->data[0], protocol->data[1]);
}

bool protocol_demo1_write_data(ProtocolDemo1* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    protocol_demo1_encoder_start(protocol);

    // This would normally write the data to the T5577 using the data
    // from the protocol->encoded_data.  For this demo, we'll just
    // populate the request with some known data.
    //
    // We use the data from https://youtu.be/5DvIBvXBDX8?t=367

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] = LFRFID_T5577_MODULATION_FSK2a | LFRFID_T5577_BITRATE_RF_50 |
                                  (3 << LFRFID_T5577_MAXBLOCK_SHIFT);
        request->t5577.block[1] = 0x1D555955;
        request->t5577.block[2] = 0x5569A599;
        request->t5577.block[3] = 0xA96A995A;
        request->t5577.blocks_to_write = 4;
        result = true;
    }
    return result;
};

const ProtocolBase protocol_demo1 = {
    .name = "Demo1",
    .manufacturer = "Derek Jamison",
    .data_size = DEMO1_DECODED_DATA_SIZE,
    .features = LFRFIDFeatureASK,
    .validate_count = 2,
    .alloc = (ProtocolAlloc)protocol_demo1_alloc,
    .free = (ProtocolFree)protocol_demo1_free,
    .get_data = (ProtocolGetData)protocol_demo1_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_demo1_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_demo1_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_demo1_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_demo1_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_demo1_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_demo1_render_brief_data,
    .write_data = (ProtocolWriteData)protocol_demo1_write_data,
};
