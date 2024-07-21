#include <furi.h>
#include "toolbox/level_duration.h"
#include "protocol_gproxii.h"
#include <toolbox/manchester_decoder.h>
#include <bit_lib/bit_lib.h>
#include "lfrfid_protocols.h"

#define GPROXII_PREAMBLE_BIT_SIZE  (6)
#define GPROXII_ENCODED_BIT_SIZE   (90)
#define GPROXII_ENCODED_BYTE_FULL_SIZE (((GPROXII_PREAMBLE_BIT_SIZE + GPROXII_ENCODED_BIT_SIZE) / 8))

#define GPROXII_DATA_SIZE (12)

#define GPROXII_SHORT_TIME  (256)
#define GPROXII_LONG_TIME   (512)
#define GPROXII_JITTER_TIME (120)

#define GPROXII_SHORT_TIME_LOW  (GPROXII_SHORT_TIME - GPROXII_JITTER_TIME)
#define GPROXII_SHORT_TIME_HIGH (GPROXII_SHORT_TIME + GPROXII_JITTER_TIME)
#define GPROXII_LONG_TIME_LOW   (GPROXII_LONG_TIME - GPROXII_JITTER_TIME)
#define GPROXII_LONG_TIME_HIGH  (GPROXII_LONG_TIME + GPROXII_JITTER_TIME)

typedef struct {
    bool last_short;
    bool last_level;
    size_t encoded_index;
    uint8_t xor;
    uint8_t card_len;
    uint8_t crc_info;
    uint16_t profile;
    uint8_t facility_code;
    uint16_t card_code;
    uint8_t decoded_data[GPROXII_ENCODED_BYTE_FULL_SIZE];
    uint8_t data[GPROXII_ENCODED_BYTE_FULL_SIZE];
} ProtocolGProxII;

ProtocolGProxII* protocol_gproxii_alloc(void) {
    ProtocolGProxII* protocol = malloc(sizeof(ProtocolGProxII));
    return protocol;
}

void protocol_gproxii_free(ProtocolGProxII* protocol) {
    free(protocol);
}

uint8_t* protocol_gproxii_get_data(ProtocolGProxII* proto) {
    return proto->data;
}

void protocol_gproxii_decoder_start(ProtocolGProxII* protocol) {
    memset(protocol->data, 0, GPROXII_ENCODED_BYTE_FULL_SIZE);
    protocol->last_short = false;
}

static bool protocol_gproxii_can_be_decoded(ProtocolGProxII* protocol) {
    // Check 6 bits preamble 111110
    if(bit_lib_get_bits(protocol->data, 0, 6) != 0b111110) return false;

    // Check always 0 parity on every 5th bit after preamble
    if(bit_lib_test_parity(protocol->data, 5, GPROXII_ENCODED_BIT_SIZE, BitLibParityAlways0, 5)) return false;

    // Start GProx II decode 
    bit_lib_copy_bits(protocol->decoded_data, 0, GPROXII_ENCODED_BIT_SIZE, protocol->data, 6);

    // Remove parity
    bit_lib_remove_bit_every_nth(protocol->decoded_data, 0, GPROXII_ENCODED_BIT_SIZE, 5);

    // Reverse bytes
    for (int i = 0; i < 9; i++) {
        protocol->decoded_data[i] = bit_lib_reverse_8_fast(protocol->decoded_data[i]);
    }

    // DeXOR from byte 1 using byte 0
    for (int i = 1; i < 9; i++) {
        protocol->decoded_data[i] = protocol->decoded_data[0] ^ protocol->decoded_data[i];
    }

    protocol->xor = bit_lib_get_bits(protocol->decoded_data, 0, 8);
    protocol->card_len = bit_lib_get_bits(protocol->decoded_data, 8, 6);;
    protocol->crc_info = bit_lib_get_bits(protocol->decoded_data, 14, 2);
    protocol->profile = bit_lib_get_bits_16(protocol->decoded_data, 16, 16);
    protocol->facility_code = bit_lib_get_bits(protocol->decoded_data, 33, 8);
    protocol->card_code = bit_lib_get_bits_16(protocol->decoded_data, 41, 16);

    // Only supporting GProx 26 cards right now 
    if(protocol->card_len == 26) return true;

    return false;
}

bool protocol_gproxii_decoder_feed(ProtocolGProxII* protocol, bool level, uint32_t duration) {
    UNUSED(level);
    bool pushed = false;

    // Bi-Phase Manchester decoding inverse. Short = 1, Long = 0
    if(duration >= GPROXII_SHORT_TIME_LOW && duration <= GPROXII_SHORT_TIME_HIGH) {
        if(protocol->last_short == false) {
            protocol->last_short = true;
        } else {
            pushed = true;
            bit_lib_push_bit(protocol->data, GPROXII_ENCODED_BYTE_FULL_SIZE, true);
            protocol->last_short = false;
        }
    } else if(duration >= GPROXII_LONG_TIME_LOW && duration <= GPROXII_LONG_TIME_HIGH) {
        if(protocol->last_short == false) {
            pushed = true;
            bit_lib_push_bit(protocol->data, GPROXII_ENCODED_BYTE_FULL_SIZE, false);
        } else {
            // reset
            protocol->last_short = false;
        }
    } else {
        // reset
        protocol->last_short = false;
    }
 
    if(pushed && protocol_gproxii_can_be_decoded(protocol)) {
        return true;
    }

    return false;
}

bool protocol_gproxii_encoder_start(ProtocolGProxII* protocol) {
    protocol->encoded_index = 0;
    protocol->last_short = false;
    protocol->last_level = false;
    return true;
}

LevelDuration protocol_gproxii_encoder_yield(ProtocolGProxII* protocol) {
    uint32_t duration;
    protocol->last_level = !protocol->last_level;

    bool bit = bit_lib_get_bit(protocol->data, protocol->encoded_index);

    // Bi-Phase Manchester encoder inverted
    if(bit) {
        // two short pulses for 1
        duration = GPROXII_SHORT_TIME / 8;
        if(protocol->last_short) {
            bit_lib_increment_index(protocol->encoded_index, 96);
            protocol->last_short = false;
        } else {
            protocol->last_short = true;
        }
    } else {
        // one long pulse for 0
        duration = GPROXII_LONG_TIME / 8;
        bit_lib_increment_index(protocol->encoded_index, 96);
    }

    return level_duration_make(protocol->last_level, duration);
}

void protocol_gproxii_render_data(ProtocolGProxII* protocol, FuriString* result) {
    // Print FC, Card and Profile
    furi_string_cat_printf(result, "FC: %hhu Card: %hu P: %04hX\n", protocol->facility_code, protocol->card_code, protocol->profile);
    // XOR Key, Length and CRC
    furi_string_cat_printf(result, "XOR: %hhu LEN: %hhu CRC: %hhu", protocol->xor, protocol->card_len, protocol->crc_info);
}

bool protocol_gproxii_write_data(ProtocolGProxII* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] = LFRFID_T5577_MODULATION_BIPHASE | LFRFID_T5577_BITRATE_RF_64 | (3 << LFRFID_T5577_MAXBLOCK_SHIFT);
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.block[3] = bit_lib_get_bits_32(protocol->data, 64, 32);
        request->t5577.blocks_to_write = 4;
        result = true;
    }
    return result;
}

const ProtocolBase protocol_gproxii = {
    .name = "GProxII",
    .manufacturer = "GProxII",
    .data_size = GPROXII_DATA_SIZE,
    .features = LFRFIDFeatureASK,
    .validate_count = 3,
    .alloc = (ProtocolAlloc)protocol_gproxii_alloc,
    .free = (ProtocolFree)protocol_gproxii_free,
    .get_data = (ProtocolGetData)protocol_gproxii_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_gproxii_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_gproxii_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_gproxii_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_gproxii_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_gproxii_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_gproxii_render_data,
    .write_data = (ProtocolWriteData)protocol_gproxii_write_data,
};
