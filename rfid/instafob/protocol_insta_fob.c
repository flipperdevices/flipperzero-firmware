#include <furi.h>
#include <toolbox/protocols/protocol.h>

#include <bit_lib/bit_lib.h>

#include <toolbox/manchester_decoder.h>

// #include <lfrfid/tools/fsk_demod.h>
// #include <lfrfid/tools/fsk_osc.h>

#include "lfrfid_protocols.h"

#define TAG "InstaFob"

#define INSTAFOB_DECODED_DATA_SIZE_BYTES (8)

#define INSTAFOB_ENCODED_DATA_SIZE_BYTES ((4 * 7) + 1)
#define INSTAFOB_ENCODED_DATA_SIZE_BITS ((8 * 4 * 7) + 1)
#define INSTAFOB_ENCODED_DATA_OFFSET (7)

#define INSTAFOB_BLOCK1 (0x00107060)

#define LFRFID_FREQUENCY (125000)
#define INSTAFOB_CLOCK_PER_BIT (32)
#define INSTAFOB_READ_LONG_TIME_BASE (1000000 / (LFRFID_FREQUENCY / INSTAFOB_CLOCK_PER_BIT))
#define INSTAFOB_READ_SHORT_TIME_BASE (INSTAFOB_READ_LONG_TIME_BASE / 2)
#define INSTAFOB_READ_SEQTERM_TIME_BASE \
    (1000000 / (LFRFID_FREQUENCY * 32 / 50 / INSTAFOB_CLOCK_PER_BIT))
#define INSTAFOB_READ_JITTER_TIME_BASE (INSTAFOB_READ_SHORT_TIME_BASE * 40 / 100)
// #define INSTAFOB_CHECK_AS_WE_GO 1

typedef enum {
    ProtocolInstaFobSeqTermNone = 0,
    ProtocolInstaFobSeqTermS1,
    ProtocolInstaFobSeqTermS2,
    ProtocolInstaFobSeqTermS3,
    ProtocolInstaFobSeqTermS4,
    ProtocolInstaFobSeqTermS5,
    ProtocolInstaFobSeqTermS6,
    ProtocolInstaFobSeqTermSMax,
} ProtocolInstaFobSeqTermState;

typedef struct {
    uint8_t data[INSTAFOB_DECODED_DATA_SIZE_BYTES];

    uint8_t encoded_data[INSTAFOB_ENCODED_DATA_SIZE_BYTES];
    uint8_t encoded_data_index;
    bool encoded_polarity;
    ProtocolInstaFobSeqTermState encoded_term_state;
    FuriString* debug_string;

    ManchesterState decoder_manchester_state;
} ProtocolInstaFob;

ProtocolInstaFob* protocol_insta_fob_alloc(void) {
    ProtocolInstaFob* protocol = malloc(sizeof(ProtocolInstaFob));
    bit_lib_num_to_bytes_be(INSTAFOB_BLOCK1, 4, protocol->data);
    protocol->debug_string = furi_string_alloc();
    return (void*)protocol;
};

void protocol_insta_fob_free(ProtocolInstaFob* protocol) {
    free(protocol->debug_string);
    free(protocol);
};

uint8_t* protocol_insta_fob_get_data(ProtocolInstaFob* protocol) {
    return protocol->data;
};

void protocol_insta_fob_decoder_start(ProtocolInstaFob* protocol) {
    memset(protocol->data, 0, INSTAFOB_DECODED_DATA_SIZE_BYTES);
    memset(protocol->encoded_data, 0, INSTAFOB_ENCODED_DATA_SIZE_BYTES);
    manchester_advance(
        protocol->decoder_manchester_state,
        ManchesterEventReset,
        &protocol->decoder_manchester_state,
        NULL);
    protocol->encoded_polarity = false;
    protocol->encoded_term_state = ProtocolInstaFobSeqTermNone;
};

static const char* protocol_insta_fob_get_encoded_data(ProtocolInstaFob* protocol) {
    furi_string_reset(protocol->debug_string);
    for(size_t i = 0; i < INSTAFOB_ENCODED_DATA_SIZE_BITS; i++) {
        furi_string_cat(
            protocol->debug_string, bit_lib_get_bit(protocol->encoded_data, i) ? "1" : "0");
    }
    return furi_string_get_cstr(protocol->debug_string);
};

static bool protocol_insta_fob_can_be_decoded(ProtocolInstaFob* protocol) {
    return bit_lib_get_bits_32(protocol->encoded_data, INSTAFOB_ENCODED_DATA_OFFSET, 32) ==
           INSTAFOB_BLOCK1;
}

static void protocol_insta_fob_decode(ProtocolInstaFob* protocol) {
    bit_lib_copy_bits(protocol->data, 0, 64, protocol->encoded_data, INSTAFOB_ENCODED_DATA_OFFSET);
};

static bool protocol_insta_fob_in_range(uint32_t value, uint32_t base, uint32_t jitter) {
    return (value > (base - jitter)) && (value < (base + jitter));
};

static ManchesterEvent protocol_insta_fob_manchester_event(bool level, uint32_t duration_us) {
    ManchesterEvent event = ManchesterEventReset;

    if(protocol_insta_fob_in_range(
           duration_us, INSTAFOB_READ_SHORT_TIME_BASE, INSTAFOB_READ_JITTER_TIME_BASE)) {
        if(!level) {
            event = ManchesterEventShortHigh;
        } else {
            event = ManchesterEventShortLow;
        }
    } else if(protocol_insta_fob_in_range(
                  duration_us, INSTAFOB_READ_LONG_TIME_BASE, INSTAFOB_READ_JITTER_TIME_BASE)) {
        if(!level) {
            event = ManchesterEventLongHigh;
        } else {
            event = ManchesterEventLongLow;
        }
    }

    return event;
};

static bool protocol_insta_fob_is_sequence_terminator(uint32_t duration_us) {
    return protocol_insta_fob_in_range(
        duration_us, INSTAFOB_READ_SEQTERM_TIME_BASE, INSTAFOB_READ_JITTER_TIME_BASE);
};

static bool protocol_insta_fob_decoder_seq_terminator(
    ProtocolInstaFob* protocol,
    bool level,
    uint32_t duration_us) {
    bool decoded_signal = false;
    ManchesterEvent event = protocol_insta_fob_manchester_event(level, duration_us);

    if(level && protocol_insta_fob_is_sequence_terminator(duration_us)) {
        protocol->encoded_term_state = ProtocolInstaFobSeqTermS3;
    } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS3) {
        if(event == ManchesterEventShortHigh) {
            // We found expected next part of sequence terminator.
            protocol->encoded_term_state = ProtocolInstaFobSeqTermS4;
        } else {
            // Unexpected signal, start scanning the card again.
            protocol->encoded_term_state = ProtocolInstaFobSeqTermNone;
        }
    } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS4) {
        if(event == ManchesterEventLongLow) {
            protocol->encoded_term_state = ProtocolInstaFobSeqTermS6;
        } else {
            // Unexpected signal, start scanning the card again.
            protocol->encoded_term_state = ProtocolInstaFobSeqTermNone;
        }
    } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS6) {
        // We found the last part of sequence terminator!

        // Check if our data is valid.
        if(protocol_insta_fob_can_be_decoded(protocol)) {
            protocol_insta_fob_decode(protocol);
            decoded_signal = true;

            FURI_LOG_D(TAG, "Decoded %s", protocol_insta_fob_get_encoded_data(protocol));

            // Clear the encoded data, reset to the first bit.
            memset(protocol->encoded_data, 0, INSTAFOB_ENCODED_DATA_SIZE_BYTES);
            protocol->encoded_polarity = false;
        } else {
            FURI_LOG_D(TAG, "Failed decoding %s", protocol_insta_fob_get_encoded_data(protocol));
        }

        protocol->encoded_term_state = ProtocolInstaFobSeqTermNone;
    } else {
        // Unknown signal, ignore.
    }

    return decoded_signal;
}

bool protocol_insta_fob_decoder_feed(ProtocolInstaFob* protocol, bool level, uint32_t duration_us) {
    bool decoded_signal = false;
    ManchesterEvent event = protocol_insta_fob_manchester_event(level, duration_us);

    if(protocol->encoded_term_state || event == ManchesterEventReset) {
        return protocol_insta_fob_decoder_seq_terminator(protocol, level, duration_us);
    }

    bool data;
    bool data_ok = manchester_advance(
        protocol->decoder_manchester_state, event, &protocol->decoder_manchester_state, &data);
    // If data_ok is false, it means we only decoded the first half of the bit.
    // When data_ok is true, we decoded both halves of the bit and "data" variable is set.

    if(data_ok) {
        // Mark that we read the second half of signal
        protocol->encoded_polarity = false;

        bit_lib_push_bit(protocol->encoded_data, INSTAFOB_ENCODED_DATA_SIZE_BYTES, data);

#ifdef INSTAFOB_CHECK_AS_WE_GO
        if(protocol_insta_fob_can_be_decoded(protocol)) {
            protocol_insta_fob_decode(protocol);
            decoded_signal = true;

            memset(protocol->encoded_data, 0, INSTAFOB_ENCODED_DATA_SIZE_BYTES);
            protocol->encoded_polarity = false;
        }
#endif
    } else {
        // encoded_polarity should be false when we are at the first half of the signal.
        // If it is true, then we are actually at the beginning of the first set bit.

        if(protocol->encoded_polarity) {
            memset(protocol->encoded_data, 0, INSTAFOB_ENCODED_DATA_SIZE_BYTES);
            bit_lib_push_bit(protocol->encoded_data, INSTAFOB_ENCODED_DATA_SIZE_BYTES, 1);
        }

        // Mark that we read the first half of signal
        protocol->encoded_polarity = true;
    }

    return decoded_signal;
};

bool protocol_insta_fob_encoder_start(ProtocolInstaFob* protocol) {
    uint32_t block1 = bit_lib_get_bits_32(protocol->data, 0, 32);
    if(block1 != INSTAFOB_BLOCK1) {
        FURI_LOG_E(
            TAG,
            "Block 1 has wrong data (%08lx). Updating to %08lx.",
            block1,
            (uint32_t)INSTAFOB_BLOCK1);

        bit_lib_num_to_bytes_be(INSTAFOB_BLOCK1, 4, protocol->data);
    }

    // You can add parity or other data manipulation here.

    // Set all of our encoded_data bits to zeros.
    memset(protocol->encoded_data, 0, INSTAFOB_ENCODED_DATA_SIZE_BYTES);

    // Copy the data to the beginning of our data onto the encoded_data
    bit_lib_copy_bits(
        protocol->encoded_data, 0, 8 * INSTAFOB_DECODED_DATA_SIZE_BYTES, protocol->data, 0);

    // Note: For sending we start at bit 0.
    protocol->encoded_data_index = 0;
    protocol->encoded_polarity = true;
    protocol->encoded_term_state = ProtocolInstaFobSeqTermNone;

    return true;
}

LevelDuration protocol_insta_fob_encoder_yield(ProtocolInstaFob* protocol) {
    bool level = bit_lib_get_bit(protocol->encoded_data, protocol->encoded_data_index);
    uint32_t duration_cycles = INSTAFOB_CLOCK_PER_BIT / 2;

    if(protocol->encoded_term_state == ProtocolInstaFobSeqTermNone) {
        if(protocol->encoded_polarity) {
            protocol->encoded_polarity = false;
        } else {
            level = !level;

            protocol->encoded_polarity = true;
            protocol->encoded_data_index++;
            if(protocol->encoded_data_index == (INSTAFOB_ENCODED_DATA_SIZE_BITS - 1)) {
                // Next yield call will be the start of the sequence terminator.
                protocol->encoded_term_state = ProtocolInstaFobSeqTermS1;
            }
        }
    } else {
        // States 1-2 will send a "1" bit.  This is what I observed on the ProxMark3.
        // States 3-4 will send a "." bit due to invalid duration (Really long high then short low).
        // States 5-6 will send a "." bit due to invalid duration (A long high).
        if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS1) {
            level = true;
        } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS2) {
            level = !true;
        } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS3) {
            level = true;
            duration_cycles = INSTAFOB_CLOCK_PER_BIT * 50 / 32;
        } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS4) {
            level = false;
        } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS5) {
            level = true;
        } else if(protocol->encoded_term_state == ProtocolInstaFobSeqTermS6) {
            level = true;
        }

        // Move to the next state.
        protocol->encoded_term_state++;

        if(protocol->encoded_term_state == ProtocolInstaFobSeqTermSMax) {
            // We are sending the end of the sequence terminator.
            // Set variables so next yield call will send the first half of the first bit.
            protocol->encoded_term_state = ProtocolInstaFobSeqTermNone;
            protocol->encoded_data_index = 0;
            protocol->encoded_polarity = true;
        }
    }

    return level_duration_make(level, duration_cycles);
};

void protocol_insta_fob_render_data(ProtocolInstaFob* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "InstaFob\nBlk[1]: %08lX\nBlk[2]: %08lX",
        bit_lib_get_bits_32(protocol->data, 0, 32),
        bit_lib_get_bits_32(protocol->data, 32, 32));
};

void protocol_insta_fob_render_brief_data(ProtocolInstaFob* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "Fob %08lX %08lX",
        bit_lib_get_bits_32(protocol->data, 0, 32),
        bit_lib_get_bits_32(protocol->data, 32, 32));
};

bool protocol_insta_fob_write_data(ProtocolInstaFob* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    // Encode data
    protocol_insta_fob_encoder_start(protocol);

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] =
            (LFRFID_T5577_MODULATION_MANCHESTER | LFRFID_T5577_BITRATE_RF_32 |
             LFRFID_T5577_ST_TERMINATOR | (7 << LFRFID_T5577_MAXBLOCK_SHIFT));
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->encoded_data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->encoded_data, 32, 32);
        // Clear the rest of the blocks
        for(int i = 3; i < 8; i++) {
            request->t5577.block[i] = 0;
        }
        request->t5577.blocks_to_write = 8;
        result = true;

        FURI_LOG_D(
            TAG,
            "block[0]: %08lx block[1]: %08lx block[2]: %08lx",
            request->t5577.block[0],
            request->t5577.block[1],
            request->t5577.block[2]);
    }
    return result;
};

const ProtocolBase protocol_insta_fob = {
    .name = "InstaFob",
    .manufacturer = "Hillman Group",
    .data_size = INSTAFOB_DECODED_DATA_SIZE_BYTES,
    .features = LFRFIDFeatureASK,
    .validate_count = 3,
    .alloc = (ProtocolAlloc)protocol_insta_fob_alloc,
    .free = (ProtocolFree)protocol_insta_fob_free,
    .get_data = (ProtocolGetData)protocol_insta_fob_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_insta_fob_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_insta_fob_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_insta_fob_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_insta_fob_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_insta_fob_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_insta_fob_render_brief_data,
    .write_data = (ProtocolWriteData)protocol_insta_fob_write_data,
};
