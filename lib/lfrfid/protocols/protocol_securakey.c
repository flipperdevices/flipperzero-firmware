#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <toolbox/hex.h>
#include <bit_lib/bit_lib.h>
#include "lfrfid_protocols.h"
#include <toolbox/manchester_decoder.h>

// #include <lfrfid/tools/fsk_demod.h>
// #include <lfrfid/tools/fsk_osc.h>

#define TAG "SECURAKEY"

#define SECURAKEY_ENCODED_BIT_SIZE (83)
// #define SECURAKEY_ENCODED_BYTE_SIZE (((SECURAKEY_ENCODED_BIT_SIZE) / 8))
#define SECURAKEY_PREAMBLE_BIT_SIZE (13)
// #define SECURAKEY_PREAMBLE_BYTE_SIZE (1)
//SECURAKEY_ENCODED_DATA_SIZE_BITS
#define SECURAKEY_ENCODED_BIT_FULL_SIZE \
    (SECURAKEY_ENCODED_BIT_SIZE + SECURAKEY_PREAMBLE_BIT_SIZE)
#define SECURAKEY_ENCODED_BYTE_FULL_SIZE ((SECURAKEY_ENCODED_BIT_FULL_SIZE) / 8)

#define SECURAKEY_BYTE_LENGTH (12) // start bit, 7 data bits, parity bit, stop bit
////////////////////////#define SECURAKEY_DATA_START_INDEX (8 + (3 * SECURAKEY_BYTE_LENGTH) + 1)

#define SECURAKEY_DECODED_DATA_SIZE_BYTES (3)
#define SECURAKEY_ENCODED_DATA_SIZE_BYTES (SECURAKEY_ENCODED_BYTE_FULL_SIZE)


//#define SECURAKEY_CYCLE_LENGTH (256)
//#define SECURAKEY_MIN_TIME (60)
//#define SECURAKEY_MAX_TIME (4000)

#define LFRFID_FREQUENCY (65000)
#define SECURAKEY_CLOCK_PER_BIT (40)
#define SECURAKEY_CLOCKS_IN_US (1000000 * (LFRFID_FREQUENCY / SECURAKEY_CLOCK_PER_BIT))
#define SECURAKEY_READ_LONG_TIME_BASE (1000000 / (LFRFID_FREQUENCY / SECURAKEY_CLOCK_PER_BIT))
#define SECURAKEY_READ_SHORT_TIME_BASE (SECURAKEY_READ_LONG_TIME_BASE / 2)
#define SECURAKEY_READ_SEQTERM_TIME_BASE \
    (1000000 / (LFRFID_FREQUENCY * 32 / 50 / SECURAKEY_CLOCK_PER_BIT))
#define SECURAKEY_READ_JITTER_TIME_BASE (SECURAKEY_READ_SHORT_TIME_BASE * 40 / 100)
// #define SECURAKEY_CHECK_AS_WE_GO 1

typedef enum {
    ProtocolSECURAKEYSeqTermNone = 0,
    ProtocolSECURAKEYSeqTermS1,
    ProtocolSECURAKEYSeqTermS2,
    ProtocolSECURAKEYSeqTermS3,
    ProtocolSECURAKEYSeqTermS4,
    ProtocolSECURAKEYSeqTermS5,
    ProtocolSECURAKEYSeqTermS6,
    ProtocolSECURAKEYSeqTermSMax,
} ProtocolSecurakeySeqTermState;

typedef struct {
    uint8_t data[SECURAKEY_DECODED_DATA_SIZE_BYTES];
    uint8_t encoded_data[SECURAKEY_ENCODED_DATA_SIZE_BYTES];
    uint8_t encoded_index;
    bool encoded_polarity;
    ProtocolSecurakeySeqTermState encoded_term_state;
    FuriString* debug_string;

    ManchesterState decoder_manchester_state;
} ProtocolSecurakey;

ProtocolSecurakey* protocol_securakey_alloc(void) {
    ProtocolSecurakey* protocol = malloc(sizeof(ProtocolSecurakey));
    protocol->debug_string = furi_string_alloc();
    return (void*)protocol;
};

void protocol_securakey_free(ProtocolSecurakey* protocol) {
    free(protocol->debug_string);
    free(protocol);
};

uint8_t* protocol_securakey_get_data(ProtocolSecurakey* protocol) {
    return protocol->data;
};

static bool protocol_securakey_in_range(uint32_t value, uint32_t base, uint32_t jitter) {
    return (value > (base - jitter)) && (value < (base + jitter));
};

static ManchesterEvent protocol_securakey_manchester_event(bool level, uint32_t duration_us) {
    ManchesterEvent event = ManchesterEventReset;

    if(protocol_securakey_in_range(
           duration_us, SECURAKEY_READ_SHORT_TIME_BASE, SECURAKEY_READ_JITTER_TIME_BASE)) {
        if(!level) {
            event = ManchesterEventShortHigh;
        } else {
            event = ManchesterEventShortLow;
        }
    } else if(protocol_securakey_in_range(
                  duration_us, SECURAKEY_READ_LONG_TIME_BASE, SECURAKEY_READ_JITTER_TIME_BASE)) {
        if(!level) {
            event = ManchesterEventLongHigh;
        } else {
            event = ManchesterEventLongLow;
        }
    }

    return event;
};

static bool protocol_securakey_is_sequence_terminator(uint32_t duration_us) {
    return protocol_securakey_in_range(
        duration_us, SECURAKEY_READ_SEQTERM_TIME_BASE, SECURAKEY_READ_JITTER_TIME_BASE);
};

static const char* protocol_securakey_get_encoded_data(ProtocolSecurakey* protocol) {
    furi_string_reset(protocol->debug_string);
    for(size_t i = 0; i < SECURAKEY_ENCODED_BIT_FULL_SIZE; i++) {
        furi_string_cat(
            protocol->debug_string, bit_lib_get_bit(protocol->encoded_data, i) ? "1" : "0");
    }
    return furi_string_get_cstr(protocol->debug_string);
};

static bool protocol_securakey_can_be_decoded(ProtocolSecurakey* protocol) {
    // check 11 bits preamble
    if(bit_lib_get_bits_32(protocol->encoded_data, 0, SECURAKEY_PREAMBLE_BIT_SIZE) != 0b0111111111001) {
        return false;
    } else if(bit_lib_get_bits_32(protocol->encoded_data, SECURAKEY_PREAMBLE_BIT_SIZE, 6) != 0b011010) {
        FURI_LOG_D(TAG, "Decoded %s", protocol_securakey_get_encoded_data(protocol));
        return false;
    } else {
        return false;
    }
};

static void protocol_securakey_decode(ProtocolSecurakey* protocol) {
    // 26 bit format (8-bit facility number, 16-bit card number)
    // preamble     ??bitlen   reserved        EPf   fffffffc   cccccccc   cccccccOP  CS?        CS2?
    // 0111111111 0 01011010 0 00000000 0 00000010 0 00110110 0 00111110 0 01100010 0 00001111 0 01100000 0 00000000 0 0000

    // 32 bit format (14-bit facility number, 16-bit card number)
    // preamble     ??bitlen   reserved  EPfffffff   fffffffc   cccccccc   cccccccOP  CS?        CS2?
    // 0111111111 0 01100000 0 00000000 0 10000100 0 11001010 0 01011011 0 01010110 0 00010110 0 11100000 0 00000000 0 0000

    // get bit length (26-bit or 32-bit)
    bit_lib_copy_bits(protocol->encoded_data, 13, 6, protocol->data, 0);

    // the following is only supporting 26-bit right now
    // get facility number (f)
    bit_lib_copy_bits(protocol->encoded_data, 36, 1, protocol->data, 6);
        // have to skip one spacer
    bit_lib_copy_bits(protocol->encoded_data, 38, 7, protocol->data, 7);

    // get card number (c)
    bit_lib_copy_bits(protocol->encoded_data, 45, 1, protocol->data, 14); 
        // same skips here
    bit_lib_copy_bits(protocol->encoded_data, 47, 8, protocol->data, 15);
    bit_lib_copy_bits(protocol->encoded_data, 56, 7, protocol->data, 23);
}

static bool protocol_securakey_decoder_seq_terminator(
    ProtocolSecurakey* protocol,
    bool level,
    uint32_t duration_us) {
    bool decoded_signal = false;
    ManchesterEvent event = protocol_securakey_manchester_event(level, duration_us);

    if(level && protocol_securakey_is_sequence_terminator(duration_us)) {
        protocol->encoded_term_state = ProtocolSECURAKEYSeqTermS3;
    } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS3) {
        if(event == ManchesterEventShortHigh) {
            // We found expected next part of sequence terminator.
            protocol->encoded_term_state = ProtocolSECURAKEYSeqTermS4;
        } else {
            // Unexpected signal, start scanning the card again.
            protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;
        }
    } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS4) {
        if(event == ManchesterEventLongLow) {
            protocol->encoded_term_state = ProtocolSECURAKEYSeqTermS6;
        } else {
            // Unexpected signal, start scanning the card again.
            protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;
        }
    } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS6) {
        // We found the last part of sequence terminator!

        // Check if our data is valid.
        if(protocol_securakey_can_be_decoded(protocol)) {
            protocol_securakey_decode(protocol);
            decoded_signal = true;

            FURI_LOG_D(TAG, "Decoded signal");

            // Clear the encoded data, reset to the first bit.
            memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);
            protocol->encoded_polarity = false;
        } else {
            FURI_LOG_D(TAG, "Failed decoding signal");
        }

        protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;
    } else {
        // Unknown signal
        FURI_LOG_D(TAG, "seq terminator fail");
    }

    return decoded_signal;
}

void protocol_securakey_decoder_start(ProtocolSecurakey* protocol) {
    memset(protocol->data, 0, SECURAKEY_DECODED_DATA_SIZE_BYTES);
    memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);
    manchester_advance(
        protocol->decoder_manchester_state,
        ManchesterEventReset,
        &protocol->decoder_manchester_state,
        NULL);
    protocol->encoded_polarity = false;
    protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;
};

bool protocol_securakey_decoder_feed(ProtocolSecurakey* protocol, bool level, uint32_t duration_us) {
    bool decoded_signal = false;
    ManchesterEvent event = protocol_securakey_manchester_event(level, duration_us);

    if(protocol->encoded_term_state || event == ManchesterEventReset) {
        // FURI_LOG_D(TAG, "Manch reset or term state = 0");
        return protocol_securakey_decoder_seq_terminator(protocol, level, duration_us);
    }

    bool data;
    bool data_ok = manchester_advance(
        protocol->decoder_manchester_state, event, &protocol->decoder_manchester_state, &data);
    // If data_ok is false, it means we only decoded the first half of the bit.
    // When data_ok is true, we decoded both halves of the bit and "data" variable is set.

    if(data_ok) {
        // Mark that we read the second half of signal
        protocol->encoded_polarity = false;
        FURI_LOG_D(TAG, "Data OK");
        bit_lib_push_bit(protocol->encoded_data, SECURAKEY_ENCODED_DATA_SIZE_BYTES, data);

#ifdef SECURAKEY_CHECK_AS_WE_GO
        if(protocol_securakey_can_be_decoded(protocol)) {
            protocol_securakey_decode(protocol);
            decoded_signal = true;

            memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);
            protocol->encoded_polarity = false;
        }
#endif
    } else {
        // encoded_polarity should be false when we are at the first half of the signal.
        // If it is true, then we are actually at the beginning of the first set bit.

        if(protocol->encoded_polarity) {
            memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);
            bit_lib_push_bit(protocol->encoded_data, SECURAKEY_ENCODED_DATA_SIZE_BYTES, 1);
        }

        // Mark that we read the first half of signal
        protocol->encoded_polarity = true;
    }

    return decoded_signal;
};

void protocol_securakey_render_data(ProtocolSecurakey* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "SECURAKEY\nBlk[1]: %08lX\nBlk[2]: %08lX",
        bit_lib_get_bits_32(protocol->data, 0, 32),
        bit_lib_get_bits_32(protocol->data, 32, 32));
};

void protocol_securakey_render_brief_data(ProtocolSecurakey* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "Fob %08lX %08lX",
        bit_lib_get_bits_32(protocol->data, 0, 32),
        bit_lib_get_bits_32(protocol->data, 32, 32));
};

//////////////////////////////////////////////////////////////////
bool protocol_securakey_encoder_start(ProtocolSecurakey* protocol) {
//    uint32_t block1 = bit_lib_get_bits_32(protocol->data, 0, 32);
//    if(block1 != SECURAKEY_BLOCK1) {
//        FURI_LOG_E(
//            TAG,
//            "Block 1 has wrong data (%08lx). Updating to %08lx.",
//            block1,
//            (uint32_t)SECURAKEY_BLOCK1);
//
//        bit_lib_num_to_bytes_be(SECURAKEY_BLOCK1, 4, protocol->data);
//    }

    // You can add parity or other data manipulation here.

    // Set all of our encoded_data bits to zeros.
    memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);

    // Copy the data to the beginning of our data onto the encoded_data
    
    // Note: For sending we start at bit 0.
    protocol->encoded_index = 0;
    protocol->encoded_polarity = true;
    protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;

    return true;
}

LevelDuration protocol_securakey_encoder_yield(ProtocolSecurakey* protocol) {
    bool level = bit_lib_get_bit(protocol->encoded_data, protocol->encoded_index);
    uint32_t duration_cycles = SECURAKEY_CLOCK_PER_BIT / 2;

    if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermNone) {
        if(protocol->encoded_polarity) {
            protocol->encoded_polarity = false;
        } else {
            level = !level;

            protocol->encoded_polarity = true;
            protocol->encoded_index++;
            if(protocol->encoded_index == (SECURAKEY_ENCODED_BIT_FULL_SIZE - 1)) {
                // Next yield call will be the start of the sequence terminator.
                protocol->encoded_term_state = ProtocolSECURAKEYSeqTermS1;
            }
        }
    } else {
        // States 1-2 will send a "1" bit.  This is what I observed on the ProxMark3.
        // States 3-4 will send a "." bit due to invalid duration (Really long high then short low).
        // States 5-6 will send a "." bit due to invalid duration (A long high).
        if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS1) {
            level = true;
        } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS2) {
            level = !true;
        } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS3) {
            level = true;
            duration_cycles = SECURAKEY_CLOCK_PER_BIT * 50 / 32;
        } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS4) {
            level = false;
        } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS5) {
            level = true;
        } else if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermS6) {
            level = true;
        }

        // Move to the next state.
        protocol->encoded_term_state++;

        if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermSMax) {
            // We are sending the end of the sequence terminator.
            // Set variables so next yield call will send the first half of the first bit.
            protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;
            protocol->encoded_index = 0;
            protocol->encoded_polarity = true;
        }
    }

    return level_duration_make(level, duration_cycles);
};

bool protocol_securakey_write_data(ProtocolSecurakey* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;
    // Encode data

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] =
            (LFRFID_T5577_MODULATION_MANCHESTER | LFRFID_T5577_BITRATE_RF_40 |
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

const ProtocolBase protocol_securakey = {
    .name = "Radiokey",
    .manufacturer = "Securakey",
    .data_size = SECURAKEY_DECODED_DATA_SIZE_BYTES,
    .features = LFRFIDFeaturePSK,
    .validate_count = 3,
    .alloc = (ProtocolAlloc)protocol_securakey_alloc,
    .free = (ProtocolFree)protocol_securakey_free,
    .get_data = (ProtocolGetData)protocol_securakey_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_securakey_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_securakey_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_securakey_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_securakey_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_securakey_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_securakey_render_data,
    .write_data = (ProtocolWriteData)protocol_securakey_write_data,
};