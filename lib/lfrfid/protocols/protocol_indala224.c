#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <lfrfid/tools/bit_lib.h>
#include "lfrfid_protocols.h"

//----------------------------------------------------------------
// Parameter Definitions
//----------------------------------------------------------------

#define INDALA224_PREAMBLE_BIT_SIZE (32)
#define INDALA224_PREAMBLE_DATA_SIZE (4)

#define INDALA224_ENCODED_BIT_SIZE (224)
#define INDALA224_ENCODED_DATA_SIZE \
    (((INDALA224_ENCODED_BIT_SIZE) / 8) + INDALA224_PREAMBLE_DATA_SIZE)
#define INDALA224_ENCODED_DATA_LAST ((INDALA224_ENCODED_BIT_SIZE) / 8)

#define INDALA224_DECODED_BIT_SIZE (224)
#define INDALA224_DECODED_DATA_SIZE (28)

#define INDALA224_US_PER_BIT (255)
#define INDALA224_ENCODER_PULSES_PER_BIT (16)

//----------------------------------------------------------------
// Struct Definitions
//----------------------------------------------------------------

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

//----------------------------------------------------------------
// Utility Functions
//----------------------------------------------------------------

// Neater aliases for getting and setting bits

bool gb(const uint8_t* data, size_t position) {
    return bit_lib_get_bit(data, position);
}

void sb(uint8_t* data, size_t position, bool bit) {
    bit_lib_set_bit(data, position, bit);
}

void psk1_to_psk2(uint8_t* data, size_t size) {
    bool lastbit = gb(data, 0);
    size_t num_bits = size * 8;

    for(size_t i = 1; i < num_bits; i++) {
        if(lastbit != gb(data, i)) {
            lastbit = gb(data, i);
            sb(data, i, 1);
        } else {
            sb(data, i, 0);
        }
    }
}

void psk2_to_psk1(uint8_t* data, size_t size) {
    bool phase = 0;
    size_t num_bits = size * 8;

    for(size_t i = 0; i < num_bits; i++) {
        if(gb(data, i) == 1) {
            phase ^= 1;
        }
        sb(data, i, phase);
    }
}

//----------------------------------------------------------------
// Indala 224 Functions
//----------------------------------------------------------------

ProtocolIndala224* protocol_indala224_alloc(void) {
    ProtocolIndala224* protocol = malloc(sizeof(ProtocolIndala224));
    return protocol;
};

void protocol_indala224_free(ProtocolIndala224* protocol) {
    free(protocol);
};

uint8_t* protocol_indala224_get_data(ProtocolIndala224* protocol) {
    return protocol->data;
};

void protocol_indala224_decoder_start(ProtocolIndala224* protocol) {
    memset(protocol->encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    memset(protocol->negative_encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    memset(protocol->corrupted_encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    memset(protocol->corrupted_negative_encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
};

static bool protocol_indala224_check_preamble(uint8_t* data, size_t bit_index) {
    // Preamble 10000000__00000000__00000000__00000001
    if(data[bit_index / 8] != 0b10000000) return false;
    if(data[(bit_index + 1) / 8] != 0b00000000) return false;
    if(data[(bit_index + 2) / 8] != 0b00000000) return false;
    if(data[(bit_index + 3) / 8] != 0b00000001) return false;
    return true;
}

static bool protocol_indala224_can_be_decoded(uint8_t* data) {
    // Use PSK2 Demodulated version
    uint8_t temp_data[INDALA224_ENCODED_DATA_SIZE];
    memcpy(temp_data, data, INDALA224_ENCODED_DATA_SIZE);
    psk1_to_psk2(temp_data, INDALA224_ENCODED_DATA_SIZE);

    if(!protocol_indala224_check_preamble(temp_data, 0)) return false;
    if(!protocol_indala224_check_preamble(temp_data, 224)) return false;
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
    bit_lib_copy_bits(data_to, 0, 32, data_from, 0); // UID 1
    bit_lib_copy_bits(data_to, 32, 32, data_from, 0 + 32); // UID 2
    bit_lib_copy_bits(data_to, 64, 32, data_from, 0 + 64); // UID 3
    bit_lib_copy_bits(data_to, 96, 32, data_from, 0 + 96); // UID 4
    bit_lib_copy_bits(data_to, 128, 32, data_from, 0 + 128); // UID 5
    bit_lib_copy_bits(data_to, 160, 32, data_from, 0 + 160); // UID 6
    bit_lib_copy_bits(data_to, 192, 32, data_from, 0 + 192); // UID 7

    // Convert from PSK1 demodulation to PSK2 demodulation
    psk1_to_psk2(data_to, INDALA224_DECODED_DATA_SIZE);
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
        // Try to decode wrong phase synced data
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
};

bool protocol_indala224_encoder_start(ProtocolIndala224* protocol) {
    memset(protocol->encoded_data, 0, INDALA224_ENCODED_DATA_SIZE);
    bit_lib_copy_bits(protocol->encoded_data, 0, 32, protocol->data, 0); // UID 1
    bit_lib_copy_bits(protocol->encoded_data, 0 + 32, 32, protocol->data, 32); // UID 2
    bit_lib_copy_bits(protocol->encoded_data, 0 + 64, 32, protocol->data, 64); // UID 3
    bit_lib_copy_bits(protocol->encoded_data, 0 + 96, 32, protocol->data, 96); // UID 4
    bit_lib_copy_bits(protocol->encoded_data, 0 + 128, 32, protocol->data, 128); // UID 5
    bit_lib_copy_bits(protocol->encoded_data, 0 + 160, 32, protocol->data, 160); // UID 6
    bit_lib_copy_bits(protocol->encoded_data, 0 + 192, 32, protocol->data, 192); // UID 7

    // Convert from PSK2 Demodulation to PSK1 Demodulation
    psk2_to_psk1(protocol->encoded_data, INDALA224_ENCODED_DATA_SIZE);

    protocol->encoder.last_bit =
        bit_lib_get_bit(protocol->encoded_data, INDALA224_ENCODED_BIT_SIZE - 1);
    protocol->encoder.data_index = 0;
    protocol->encoder.current_polarity = true;
    protocol->encoder.pulse_phase = true;
    protocol->encoder.bit_clock_index = 0;

    return true;
};

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
};

void protocol_indala224_render_data_internal(
    ProtocolIndala224* protocol,
    FuriString* result,
    bool brief) {
    const uint32_t uid1 = bit_lib_get_bits_32(protocol->data, 0, 32);
    const uint32_t uid2 = bit_lib_get_bits_32(protocol->data, 32, 32);
    const uint32_t uid3 = bit_lib_get_bits_32(protocol->data, 64, 32);
    const uint32_t uid4 = bit_lib_get_bits_32(protocol->data, 96, 32);
    const uint32_t uid5 = bit_lib_get_bits_32(protocol->data, 128, 32);
    const uint32_t uid6 = bit_lib_get_bits_32(protocol->data, 160, 32);
    const uint32_t uid7 = bit_lib_get_bits_32(protocol->data, 192, 32);

    if(brief) {
        furi_string_printf(result, "UID: %u%u...", (unsigned int)uid1, (unsigned int)uid2);
    } else {
        furi_string_printf(
            result,
            "UID: %u%u%u%u%u%u%u",
            (unsigned int)uid1,
            (unsigned int)uid2,
            (unsigned int)uid3,
            (unsigned int)uid4,
            (unsigned int)uid5,
            (unsigned int)uid6,
            (unsigned int)uid7);
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

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] = LFRFID_T5577_BITRATE_RF_32 | LFRFID_T5577_MODULATION_PSK2 |
                                  (7 << LFRFID_T5577_MAXBLOCK_SHIFT);
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.block[3] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.block[4] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.block[5] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.block[6] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.block[7] = bit_lib_get_bits_32(protocol->data, 32, 32);
        request->t5577.blocks_to_write = 8;
        result = true;
    }
    return result;
};

//----------------------------------------------------------------
// Indala 224 Protocol
//----------------------------------------------------------------

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
