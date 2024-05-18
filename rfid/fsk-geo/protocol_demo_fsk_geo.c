/**
 * @file protocol_demo_fsk_geo.c
 * 
 * This is an example of implementing a low-frequency (125kHz) RFID protocol
 * using FSK modulation. This protocol is used to transmit geographical
 * coordinates. The data is encoded in the following way:
 *  - 16 bits preamble (0xF337)
 *  - 64 bits of data (32 bits latitude, 32 bits longitude)
 *  - 8 bits of XOR parity
 * 
 * You can create similar protocols, just be sure to change the preamble so
 * that the decoder can distinguish between different protocols.
 * 
 * Some FSK protocols decide to also Manchester encode the data, but this
 * example does not do that.  Manchester encoding would provide an additional
 * layer of error detection (since data must be encoded as either 01 or 10).
 * 
*/
#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <lfrfid/tools/fsk_demod.h>
#include <lfrfid/tools/fsk_osc.h>
#include <bit_lib/bit_lib.h>
#include "lfrfid_protocols.h"

// 16us of jitter.
#define JITTER_TIME (20)

// 8 bytes of data.
#define DEMO_DECODED_DATA_SIZE (8)

// 2 byte header, 8 bytes data, 1 byte xor parity
#define DEMO_ENCODED_DATA_SIZE (11)

// 2 byte header, 8 bytes data, 1 byte xor parity
#define DEMO_ENCODED_DATA_BITS (2 * 8 + 8 * 8 + 8)

#define TAG "Demo FSK Geo"

typedef struct {
    FSKDemod* fsk_demod;
} ProtocolDemoDecoder;

typedef struct {
    FSKOsc* fsk_osc;
    uint8_t encoded_index;
    uint32_t pulse;
} ProtocolDemoEncoder;

typedef struct {
    ProtocolDemoDecoder decoder;
    ProtocolDemoEncoder encoder;
    uint8_t encoded_data[DEMO_ENCODED_DATA_SIZE];
    uint8_t data[DEMO_DECODED_DATA_SIZE];
} ProtocolDemo;

static ProtocolDemo* protocol_demo_alloc(void) {
    ProtocolDemo* protocol = malloc(sizeof(ProtocolDemo));
    protocol->decoder.fsk_demod = fsk_demod_alloc(64 - JITTER_TIME, 6, 80 + JITTER_TIME, 5);
    protocol->encoder.fsk_osc = fsk_osc_alloc(8, 10, 50);
    float* f = (float*)protocol->data;
    *f = 36.63813; // Sample latitude
    f = (float*)(protocol->data + 4);
    *f = -93.28007; // Sample longitude
    return protocol;
};

static void protocol_demo_free(ProtocolDemo* protocol) {
    fsk_demod_free(protocol->decoder.fsk_demod);
    fsk_osc_free(protocol->encoder.fsk_osc);
    free(protocol);
};

static uint8_t* protocol_demo_get_data(ProtocolDemo* protocol) {
    return protocol->data;
};

static void protocol_demo_decoder_start(ProtocolDemo* protocol) {
    memset(protocol->encoded_data, 0, sizeof(uint8_t) * DEMO_ENCODED_DATA_SIZE);
};

static void protocol_demo_decoder_store_data(ProtocolDemo* protocol, bool data) {
    bit_lib_push_bit(protocol->encoded_data, DEMO_ENCODED_DATA_SIZE, data);
}

static bool protocol_demo_can_be_decoded(const uint8_t* encoded_data) {
    // Our preamble is 0xF337.
    if(bit_lib_get_bits(encoded_data, 0, 8) != 0xF3) {
        return false;
    }

    if(bit_lib_get_bits(encoded_data, 8, 8) != 0x37) {
        return false;
    }

    // Checksum is XOR of each byte of data
    uint8_t xor = 0;
    for(int i = 0; i < DEMO_DECODED_DATA_SIZE; i++) {
        xor ^= bit_lib_get_bits(encoded_data, 16 + i * 8, 8);
    }

    // Checksum is stored after data
    if(xor != bit_lib_get_bits(encoded_data, 16 + DEMO_DECODED_DATA_SIZE * 8, 8)) {
        return false;
    }

    return true;
}

static void protocol_demo_decode(const uint8_t* encoded_data, uint8_t* decoded_data) {
    // Skip preamble, copy data.
    // If data was Manchester encoded, decoded first (or copy every other bit).
    bit_lib_copy_bits(decoded_data, 0, 8 * DEMO_DECODED_DATA_SIZE, encoded_data, 16);
}

static bool protocol_demo_decoder_feed(ProtocolDemo* protocol, bool level, uint32_t duration) {
    bool value;
    uint32_t count;
    bool result = false;

    fsk_demod_feed(protocol->decoder.fsk_demod, level, duration, &value, &count);
    if(count > 0) {
        // Store data and check if it can be decoded
        for(size_t i = 0; i < count; i++) {
            protocol_demo_decoder_store_data(protocol, value);
            if(protocol_demo_can_be_decoded(protocol->encoded_data)) {
                protocol_demo_decode(protocol->encoded_data, protocol->data);
                result = true;
                break;
            }
        }
    }

    return result;
};

static void protocol_demo_write_raw_bit(bool bit, uint8_t position, uint8_t* card_data) {
    bit_lib_set_bits(card_data, position, bit, 1);
}

static void protocol_demo_encode(const uint8_t* decoded_data, uint8_t* encoded_data) {
    uint8_t card_data[DEMO_ENCODED_DATA_SIZE] = {0};

    // 0xF337 (1111001100110111) preamble
    protocol_demo_write_raw_bit(1, 0, card_data);
    protocol_demo_write_raw_bit(1, 1, card_data);
    protocol_demo_write_raw_bit(1, 2, card_data);
    protocol_demo_write_raw_bit(1, 3, card_data);

    protocol_demo_write_raw_bit(0, 4, card_data);
    protocol_demo_write_raw_bit(0, 5, card_data);
    protocol_demo_write_raw_bit(1, 6, card_data);
    protocol_demo_write_raw_bit(1, 7, card_data);

    protocol_demo_write_raw_bit(0, 8, card_data);
    protocol_demo_write_raw_bit(0, 9, card_data);
    protocol_demo_write_raw_bit(1, 10, card_data);
    protocol_demo_write_raw_bit(1, 11, card_data);

    protocol_demo_write_raw_bit(0, 12, card_data);
    protocol_demo_write_raw_bit(1, 13, card_data);
    protocol_demo_write_raw_bit(1, 14, card_data);
    protocol_demo_write_raw_bit(1, 15, card_data);

    // Some protocols may want to Manchester encode the data, but this one does not.
    bit_lib_copy_bits(card_data, 16, DEMO_DECODED_DATA_SIZE * 8, decoded_data, 0);

    // Checksum is XOR of each byte of data
    uint8_t xor = 0;
    for(uint8_t i = 0; i < DEMO_DECODED_DATA_SIZE; i++) {
        xor ^= decoded_data[i];
    }

    // Checksum is stored after data
    bit_lib_copy_bits(card_data, 16 + DEMO_DECODED_DATA_SIZE * 8, 8, &xor, 0);

    memcpy(encoded_data, &card_data, DEMO_ENCODED_DATA_SIZE);
}

static bool protocol_demo_encoder_start(ProtocolDemo* protocol) {
    protocol_demo_encode(protocol->data, (uint8_t*)protocol->encoded_data);
    protocol->encoder.encoded_index = 0;
    protocol->encoder.pulse = 0;

    return true;
};

static LevelDuration protocol_demo_encoder_yield(ProtocolDemo* protocol) {
    bool level = 0;
    uint32_t duration = 0;

    // if pulse is zero, we need to output high, otherwise we need to output low
    if(protocol->encoder.pulse == 0) {
        // get bit
        uint8_t bit = bit_lib_get_bits(
                          (uint8_t*)protocol->encoded_data, protocol->encoder.encoded_index, 1) &
                      1;

        // get pulse from oscillator
        bool advance = fsk_osc_next(protocol->encoder.fsk_osc, bit, &duration);
        if(advance) {
            protocol->encoder.encoded_index++;
            if(protocol->encoder.encoded_index >= (DEMO_ENCODED_DATA_BITS)) {
                protocol->encoder.encoded_index = 0;
            }
        }

        // duration divided by 2 because we need to output high and low
        duration = duration / 2;
        protocol->encoder.pulse = duration;
        level = true;
    } else {
        // output low half and reset pulse
        duration = protocol->encoder.pulse;
        protocol->encoder.pulse = 0;
        level = false;
    }

    return level_duration_make(level, duration);
};

static bool protocol_demo_write_data(ProtocolDemo* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    protocol_demo_encoder_start(protocol);

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] = LFRFID_T5577_MODULATION_FSK2a | LFRFID_T5577_BITRATE_RF_50 |
                                  (3 << LFRFID_T5577_MAXBLOCK_SHIFT);
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->encoded_data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->encoded_data, 32, 32);
        // Since we only have 3 bytes of data, we need to shift the last byte by 8 bits
        // so we send "123456" as "12345600" instead of "00123456" (with 0s in the middle
        // of our signal).
        request->t5577.block[3] = bit_lib_get_bits_32(protocol->encoded_data, 64, 24) << 8;
        request->t5577.blocks_to_write = 4;
        result = true;

        FURI_LOG_D(
            TAG,
            "T5577 Write data: %08lX %08lX %08lX",
            request->t5577.block[1],
            request->t5577.block[2],
            request->t5577.block[3]);
    }
    return result;
};

static void protocol_demo_render_data(ProtocolDemo* protocol, FuriString* result) {
    uint8_t* data = protocol->data;
    float* lat = (float*)data;
    float* lon = (float*)(data + 4);
    furi_string_printf(
        result,
        "Lat: %f\n"
        "Long: %f",
        (double)*lat,
        (double)*lon);
};

static void protocol_demo_render_brief_data(ProtocolDemo* protocol, FuriString* result) {
    uint8_t* data = protocol->data;
    float* lat = (float*)data;
    float* lon = (float*)(data + 4);
    furi_string_printf(result, "(%f,%f)", (double)*lat, (double)*lon);
};

/*
// https://onlinegdb.com/UHHaOKrpj
// E.g.  36.0352, -86.6797    => 0B 24 10 42 02 5C AD C2

#include <stdio.h>
#include <stdint.h>

int main() {
    float f,lat,lon;
    printf("enter latitude,longitude: "); // E.g.  36.0352, -86.6797
    scanf("%f,%f", &lat,&lon);
    f = lat;
    uint8_t* lat_p = (uint8_t*)&f;
    printf("%02X %02X %02X %02X ", *lat_p, *(lat_p+1),*(lat_p+2),*(lat_p+3));
    f = lon;
    printf("%02X %02X %02X %02X", *lat_p, *(lat_p+1),*(lat_p+2),*(lat_p+3));
    return 0;
}
*/

const ProtocolBase protocol_demo_fsk_geo = {
    .name = "FEE7",
    .manufacturer = "JAMISON",
    .data_size = DEMO_DECODED_DATA_SIZE,
    .features = LFRFIDFeatureASK,
    .validate_count = 3,
    .alloc = (ProtocolAlloc)protocol_demo_alloc,
    .free = (ProtocolFree)protocol_demo_free,
    .get_data = (ProtocolGetData)protocol_demo_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_demo_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_demo_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_demo_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_demo_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_demo_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_demo_render_brief_data,
    .write_data = (ProtocolWriteData)protocol_demo_write_data,
};
