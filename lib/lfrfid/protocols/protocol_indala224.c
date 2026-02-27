#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <bit_lib/bit_lib.h>
#include "lfrfid_protocols.h"

#define INDALA224_PREAMBLE_BIT_SIZE  (30)
#define INDALA224_PREAMBLE_DATA_SIZE (4)

#define INDALA224_ENCODED_BIT_SIZE (224)
#define INDALA224_ENCODED_DATA_SIZE \
    (((INDALA224_ENCODED_BIT_SIZE) / 8) + INDALA224_PREAMBLE_DATA_SIZE)
#define INDALA224_ENCODED_DATA_LAST ((INDALA224_ENCODED_BIT_SIZE) / 8)

#define INDALA224_DECODED_BIT_SIZE  (224)
#define INDALA224_DECODED_DATA_SIZE (28)

#define INDALA224_US_PER_BIT             (255)
#define INDALA224_ENCODER_PULSES_PER_BIT (16)

typedef struct {
    uint8_t data_index;
    uint8_t bit_clock_index;
    bool last_bit;
    bool current_polarity;
    bool pulse_phase;
} ProtocolIndala224Encoder;

typedef struct {
    uint8_t encoded_data[INDALA224_ENCODED_DATA_SIZE];
    uint8_t negative_encoded_data[INDALA224_ENCODED_DATA_SIZE];
    uint8_t corrupted_encoded_data[INDALA224_ENCODED_DATA_SIZE];
    uint8_t corrupted_negative_encoded_data[INDALA224_ENCODED_DATA_SIZE];

    uint8_t data[INDALA224_DECODED_DATA_SIZE];
    ProtocolIndala224Encoder encoder;
} ProtocolIndala224;

ProtocolIndala224* protocol_indala224_alloc(void) {
    ProtocolIndala224* protocol = malloc(sizeof(ProtocolIndala224));
    return protocol;
}

void protocol_indala224_free(ProtocolIndala224* protocol) {
    free(protocol);
}

uint8_t* protocol_indala224_get_data(ProtocolIndala224* protocol) {
    return protocol->data;
}

void protocol_indala224_decoder_start(ProtocolIndala224* protocol) {
    memset(protocol->encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    memset(protocol->negative_encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    memset(protocol->corrupted_encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    memset(protocol->corrupted_negative_encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
}

static bool protocol_indala224_check_preamble(uint8_t* data, size_t bit_index) {
    // Preamble: 1 followed by 29 zeros = 10000000 00000000 00000000 000000xx
    // First byte at bit_index should be 0x80
    if(bit_lib_get_bits(data, bit_index, 8) != 0b10000000) return false;
    // Next 21 bits (bits 8..28) must all be zero
    if(bit_lib_get_bits(data, bit_index + 8, 8) != 0) return false;
    if(bit_lib_get_bits(data, bit_index + 16, 8) != 0) return false;
    // Bits 24..29 (6 more bits) must be zero
    if(bit_lib_get_bits(data, bit_index + 24, 5) != 0) return false;
    return true;
}

static bool protocol_indala224_can_be_decoded(uint8_t* data) {
    if(!protocol_indala224_check_preamble(data, 0)) return false;
    if(!protocol_indala224_check_preamble(data, INDALA224_ENCODED_BIT_SIZE)) return false;
    return true;
}

static bool protocol_indala224_decoder_feed_internal(bool polarity, uint32_t time, uint8_t* data) {
    time += (INDALA224_US_PER_BIT / 2);

    size_t bit_count = (time / INDALA224_US_PER_BIT);
    bool result = false;

    if(bit_count < INDALA224_ENCODED_BIT_SIZE) {
        for(size_t i = 0; i < bit_count; i++) {
            bit_lib_push_bit(data, INDALA224_ENCODED_DATA_SIZE, polarity);
            if(protocol_indala224_can_be_decoded(data)) {
                result = true;
                break;
            }
        }
    }

    return result;
}

static void protocol_indala224_decoder_save(uint8_t* data_to, const uint8_t* data_from) {
    // Raw 224-bit UID: copy all 224 bits starting after preamble alignment (bit 0)
    // The encoded_data buffer has the preamble at bit 0, data continues through bit 223
    // We store the entire 224-bit frame as-is (including preamble bits as part of UID)
    memcpy(data_to, data_from, INDALA224_DECODED_DATA_SIZE);
}

bool protocol_indala224_decoder_feed(ProtocolIndala224* protocol, bool level, uint32_t duration) {
    bool result = false;

    if(duration > (INDALA224_US_PER_BIT / 2)) {
        if(protocol_indala224_decoder_feed_internal(level, duration, protocol->encoded_data)) {
            protocol_indala224_decoder_save(protocol->data, protocol->encoded_data);
            FURI_LOG_D("Indala224", "Positive");
            result = true;
            return result;
        }

        if(protocol_indala224_decoder_feed_internal(
               !level, duration, protocol->negative_encoded_data)) {
            protocol_indala224_decoder_save(protocol->data, protocol->negative_encoded_data);
            FURI_LOG_D("Indala224", "Negative");
            result = true;
            return result;
        }
    }

    if(duration > (INDALA224_US_PER_BIT / 4)) {
        if(level) {
            duration += 120;
        } else {
            if(duration > 120) {
                duration -= 120;
            }
        }

        if(protocol_indala224_decoder_feed_internal(
               level, duration, protocol->corrupted_encoded_data)) {
            protocol_indala224_decoder_save(protocol->data, protocol->corrupted_encoded_data);
            FURI_LOG_D("Indala224", "Positive Corrupted");

            result = true;
            return result;
        }

        if(protocol_indala224_decoder_feed_internal(
               !level, duration, protocol->corrupted_negative_encoded_data)) {
            protocol_indala224_decoder_save(
                protocol->data, protocol->corrupted_negative_encoded_data);
            FURI_LOG_D("Indala224", "Negative Corrupted");

            result = true;
            return result;
        }
    }

    return result;
}

bool protocol_indala224_encoder_start(ProtocolIndala224* protocol) {
    // Encoded data = raw 224-bit UID (preamble is already part of the UID)
    memcpy(protocol->encoded_data, protocol->data, INDALA224_DECODED_DATA_SIZE);

    protocol->encoder.last_bit =
        bit_lib_get_bit(protocol->encoded_data, INDALA224_ENCODED_BIT_SIZE - 1);
    protocol->encoder.data_index = 0;
    protocol->encoder.current_polarity = true;
    protocol->encoder.pulse_phase = true;
    protocol->encoder.bit_clock_index = 0;

    return true;
}

LevelDuration protocol_indala224_encoder_yield(ProtocolIndala224* protocol) {
    LevelDuration level_duration;
    ProtocolIndala224Encoder* encoder = &protocol->encoder;

    if(encoder->pulse_phase) {
        level_duration = level_duration_make(encoder->current_polarity, 1);
        encoder->pulse_phase = false;
    } else {
        level_duration = level_duration_make(!encoder->current_polarity, 1);
        encoder->pulse_phase = true;

        encoder->bit_clock_index++;
        if(encoder->bit_clock_index >= INDALA224_ENCODER_PULSES_PER_BIT) {
            encoder->bit_clock_index = 0;

            bool current_bit = bit_lib_get_bit(protocol->encoded_data, encoder->data_index);

            if(current_bit != encoder->last_bit) {
                encoder->current_polarity = !encoder->current_polarity;
            }

            encoder->last_bit = current_bit;

            bit_lib_increment_index(encoder->data_index, INDALA224_ENCODED_BIT_SIZE);
        }
    }

    return level_duration;
}

static void protocol_indala224_render_data_internal(
    ProtocolIndala224* protocol,
    FuriString* result,
    bool brief) {
    if(brief) {
        furi_string_printf(
            result,
            "Raw: %02X%02X%02X%02X\n"
            "     %02X%02X%02X%02X...",
            protocol->data[0],
            protocol->data[1],
            protocol->data[2],
            protocol->data[3],
            protocol->data[4],
            protocol->data[5],
            protocol->data[6],
            protocol->data[7]);
    } else {
        furi_string_printf(
            result,
            "Raw: %02X%02X%02X%02X %02X%02X%02X%02X\n"
            "%02X%02X%02X%02X %02X%02X%02X%02X\n"
            "%02X%02X%02X%02X %02X%02X%02X%02X\n"
            "%02X%02X%02X%02X",
            protocol->data[0],
            protocol->data[1],
            protocol->data[2],
            protocol->data[3],
            protocol->data[4],
            protocol->data[5],
            protocol->data[6],
            protocol->data[7],
            protocol->data[8],
            protocol->data[9],
            protocol->data[10],
            protocol->data[11],
            protocol->data[12],
            protocol->data[13],
            protocol->data[14],
            protocol->data[15],
            protocol->data[16],
            protocol->data[17],
            protocol->data[18],
            protocol->data[19],
            protocol->data[20],
            protocol->data[21],
            protocol->data[22],
            protocol->data[23],
            protocol->data[24],
            protocol->data[25],
            protocol->data[26],
            protocol->data[27]);
    }
}

void protocol_indala224_render_data(ProtocolIndala224* protocol, FuriString* result) {
    protocol_indala224_render_data_internal(protocol, result, false);
}

void protocol_indala224_render_brief_data(ProtocolIndala224* protocol, FuriString* result) {
    protocol_indala224_render_data_internal(protocol, result, true);
}

bool protocol_indala224_write_data(ProtocolIndala224* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    protocol_indala224_encoder_start(protocol);

    if(request->write_type == LFRFIDWriteTypeT5577) {
        // Config: PSK1, RF/32, 7 data blocks (matches Proxmark3 T55X7_INDALA_224_CONFIG_BLOCK)
        request->t5577.block[0] = LFRFID_T5577_BITRATE_RF_32 | LFRFID_T5577_MODULATION_PSK1 |
                                  (7 << LFRFID_T5577_MAXBLOCK_SHIFT);
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->encoded_data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->encoded_data, 32, 32);
        request->t5577.block[3] = bit_lib_get_bits_32(protocol->encoded_data, 64, 32);
        request->t5577.block[4] = bit_lib_get_bits_32(protocol->encoded_data, 96, 32);
        request->t5577.block[5] = bit_lib_get_bits_32(protocol->encoded_data, 128, 32);
        request->t5577.block[6] = bit_lib_get_bits_32(protocol->encoded_data, 160, 32);
        request->t5577.block[7] = bit_lib_get_bits_32(protocol->encoded_data, 192, 32);
        request->t5577.blocks_to_write = 8;
        result = true;
    }
    return result;
}

const ProtocolBase protocol_indala224 = {
    .name = "Indala224",
    .manufacturer = "Motorola",
    .data_size = INDALA224_DECODED_DATA_SIZE,
    .features = LFRFIDFeaturePSK,
    .validate_count = 6,
    .alloc = (ProtocolAlloc)protocol_indala224_alloc,
    .free = (ProtocolFree)protocol_indala224_free,
    .get_data = (ProtocolGetData)protocol_indala224_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_indala224_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_indala224_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_indala224_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_indala224_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_indala224_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_indala224_render_brief_data,
    .write_data = (ProtocolWriteData)protocol_indala224_write_data,
};
