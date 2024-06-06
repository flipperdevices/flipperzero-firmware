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
SECURAKEY_ENCODED_DATA_SIZE_BITS
#define SECURAKEY_ENCODED_BIT_FULL_SIZE \
    (SECURAKEY_ENCODED_BIT_SIZE + SECURAKEY_PREAMBLE_BIT_SIZE)
#define SECURAKEY_ENCODED_BYTE_FULL_SIZE ((SECURAKEY_ENCODED_BIT_FULL_SIZE) / 8)

#define SECURAKEY_BYTE_LENGTH (12) // start bit, 7 data bits, parity bit, stop bit
////////////////////////#define SECURAKEY_DATA_START_INDEX (8 + (3 * SECURAKEY_BYTE_LENGTH) + 1)

#define SECURAKEY_DECODED_DATA_SIZE_BYTES (3)
#define SECURAKEY_ENCODED_DATA_SIZE_BYTES (SECURAKEY_ENCODED_BYTE_FULL_SIZE)

//#define SECURAKEY_CLOCKS_IN_US (40)
//#define SECURAKEY_CYCLE_LENGTH (256)
//#define SECURAKEY_MIN_TIME (60)
//#define SECURAKEY_MAX_TIME (4000)

#define LFRFID_FREQUENCY (65000)
#define SECURAKEY_CLOCK_PER_BIT (40)
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
} ProtocolSECURAKEYSeqTermState;

typedef struct {
    uint8_t data[SECURAKEY_DECODED_DATA_SIZE_BYTES];
    uint8_t encoded_data[SECURAKEY_ENCODED_DATA_SIZE_BYTES];
    uint8_t encoded_data_index;
    bool encoded_polarity;
    ProtocolSECURAKEYSeqTermState encoded_term_state;
    FuriString* debug_string;

    ManchesterState decoder_manchester_state;
} ProtocolSECURAKEY;

int detectSecurakey(uint8_t *dest, size_t *size) {
    if (*size < 96) return -1; //make sure buffer has data
    size_t startIdx = 0;
    uint8_t preamble[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1};
    if (!preambleSearch(dest, preamble, sizeof(preamble), size, &startIdx))
        return -2; //preamble not found
    if (*size != 96) return -3; //wrong demoded size
    //return start position
    return (int)startIdx;
}

ProtocolSECURAKEY* protocol_securakey_alloc(void) {
    ProtocolSECURAKEY* protocol = malloc(sizeof(ProtocolSECURAKEY));
    protocol->debug_string = furi_string_alloc();
    return (void*)protocol;
};

void protocol_securakey_free(ProtocolSECURAKEY* protocol) {
    free(protocol->debug_string);
    free(protocol);
};

uint8_t* protocol_securakey_get_data(ProtocolSECURAKEY* protocol) {
    return protocol->data;
};

void protocol_securakey_decoder_start(ProtocolSECURAKEY* protocol) {
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

static const char* protocol_securakey_get_encoded_data(ProtocolSECURAKEY* protocol) {
    furi_string_reset(protocol->debug_string);
    for(size_t i = 0; i < SECURAKEY_ENCODED_DATA_SIZE_BITS; i++) {
        furi_string_cat(
            protocol->debug_string, bit_lib_get_bit(protocol->encoded_data, i) ? "1" : "0");
    }
    return furi_string_get_cstr(protocol->debug_string);
};

static bool protocol_securakey_can_be_decoded(ProtocolSECURAKEY* protocol) {
    // check 11 bits preamble
    if(bit_lib_get_bits_32(protocol->encoded_data, 0, SECURAKEY_PREAMBLE_BIT_SIZE) != 0b0111111111001) {
        return false
    } else if(bit_lib_get_bits_32(protocol->encoded_data, SECURAKEY_PREAMBLE_BIT_SIZE, 6) != 0b011010)
        FURI_LOG_D(TAG, "Decoded %s", protocol_securakey_get_encoded_data(protocol));
        return false
        };
    
}

static void protocol_securakey_decode(ProtocolSECURAKEY* protocol) {
    bit_lib_copy_bits(protocol->data, 0, 64, protocol->encoded_data, SECURAKEY_ENCODED_DATA_OFFSET);
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

static bool protocol_securakey_decoder_seq_terminator(
    ProtocolSECURAKEY* protocol,
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

            FURI_LOG_D(TAG, "Decoded %s", protocol_securakey_get_encoded_data(protocol));

            // Clear the encoded data, reset to the first bit.
            memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);
            protocol->encoded_polarity = false;
        } else {
            FURI_LOG_D(TAG, "Failed decoding %s", protocol_securakey_get_encoded_data(protocol));
        }

        protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;
    } else {
        // Unknown signal, ignore.
    }

    return decoded_signal;
}

bool protocol_securakey_decoder_feed(ProtocolSECURAKEY* protocol, bool level, uint32_t duration_us) {
    bool decoded_signal = false;
    ManchesterEvent event = protocol_securakey_manchester_event(level, duration_us);

    if(protocol->encoded_term_state || event == ManchesterEventReset) {
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

bool protocol_securakey_encoder_start(ProtocolSECURAKEY* protocol) {
    uint32_t block1 = bit_lib_get_bits_32(protocol->data, 0, 32);
    if(block1 != SECURAKEY_BLOCK1) {
        FURI_LOG_E(
            TAG,
            "Block 1 has wrong data (%08lx). Updating to %08lx.",
            block1,
            (uint32_t)SECURAKEY_BLOCK1);

        bit_lib_num_to_bytes_be(SECURAKEY_BLOCK1, 4, protocol->data);
    }

    // You can add parity or other data manipulation here.

    // Set all of our encoded_data bits to zeros.
    memset(protocol->encoded_data, 0, SECURAKEY_ENCODED_DATA_SIZE_BYTES);

    // Copy the data to the beginning of our data onto the encoded_data
    bit_lib_copy_bits(
        protocol->encoded_data, 0, 8 * SECURAKEY_DECODED_DATA_SIZE_BYTES, protocol->data, 0);

    // Note: For sending we start at bit 0.
    protocol->encoded_data_index = 0;
    protocol->encoded_polarity = true;
    protocol->encoded_term_state = ProtocolSECURAKEYSeqTermNone;

    return true;
}

LevelDuration protocol_securakey_encoder_yield(ProtocolSECURAKEY* protocol) {
    bool level = bit_lib_get_bit(protocol->encoded_data, protocol->encoded_data_index);
    uint32_t duration_cycles = SECURAKEY_CLOCK_PER_BIT / 2;

    if(protocol->encoded_term_state == ProtocolSECURAKEYSeqTermNone) {
        if(protocol->encoded_polarity) {
            protocol->encoded_polarity = false;
        } else {
            level = !level;

            protocol->encoded_polarity = true;
            protocol->encoded_data_index++;
            if(protocol->encoded_data_index == (SECURAKEY_ENCODED_DATA_SIZE_BITS - 1)) {
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
            protocol->encoded_data_index = 0;
            protocol->encoded_polarity = true;
        }
    }

    return level_duration_make(level, duration_cycles);
};

void protocol_securakey_render_data(ProtocolSECURAKEY* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "SECURAKEY\nBlk[1]: %08lX\nBlk[2]: %08lX",
        bit_lib_get_bits_32(protocol->data, 0, 32),
        bit_lib_get_bits_32(protocol->data, 32, 32));
};

void protocol_securakey_render_brief_data(ProtocolSECURAKEY* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "Fob %08lX %08lX",
        bit_lib_get_bits_32(protocol->data, 0, 32),
        bit_lib_get_bits_32(protocol->data, 32, 32));
};

bool protocol_securakey_write_data(ProtocolSECURAKEY* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    // Encode data
    protocol_securakey_encoder_start(protocol);

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

const ProtocolBase protocol_securakey = {
    .name = "SECURAKEY",
    .manufacturer = "Hillman Group",
    .data_size = SECURAKEY_DECODED_DATA_SIZE_BYTES,
    .features = LFRFIDFeatureASK,
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
    .render_brief_data = (ProtocolRenderData)protocol_securakey_render_brief_data,
    .write_data = (ProtocolWriteData)protocol_securakey_write_data,
};


//////////////////////////////////////////////////////////////////
typedef struct {
    bool inverted;
    bool got_preamble;
    size_t encoded_index;
    uint8_t encoded_data[SECURAKEY_ENCODED_BYTE_FULL_SIZE];
    uint8_t data[SECURAKEY_DECODED_DATA_SIZE];
} ProtocolSecurakey;

ProtocolSecurakey* protocol_SECURAKEY_alloc(void) {
    ProtocolSecurakey* protocol = malloc(sizeof(ProtocolSecurakey));
    return (void*)protocol;
}

void protocol_SECURAKEY_free(ProtocolSecurakey* protocol) {
    free(protocol);
}

uint8_t* protocol_SECURAKEY_get_data(ProtocolSecurakey* protocol) {
    return protocol->data;
}

static void protocol_SECURAKEY_decode(ProtocolSecurakey* protocol) {
    uint8_t asciiCardId[8];
    for(size_t idx = 0; idx < 8; idx++) {
        uint8_t byte = bit_lib_reverse_8_fast(bit_lib_get_bits(
            protocol->encoded_data,
            SECURAKEY_DATA_START_INDEX + (SECURAKEY_BYTE_LENGTH * idx),
            8));
        asciiCardId[idx] = byte & 0x7F; // discard the parity bit
    }

    hex_chars_to_uint8((char*)asciiCardId, protocol->data);
}

static bool protocol_SECURAKEY_can_be_decoded(ProtocolSecurakey* protocol) {
    // Check preamble
    if(bit_lib_get_bits(protocol->encoded_data, 0, 8) != 0b11111111) return false;
    if(bit_lib_get_bit(protocol->encoded_data, 8) != 0) return false;
    if(bit_lib_get_bit(protocol->encoded_data, 9) != 0) return false;
    if(bit_lib_get_bit(protocol->encoded_data, 10) != 1) return false;
    if(bit_lib_get_bits(protocol->encoded_data, 11, 8) != 0b00000010) return false;

    // Check next preamble
    if(bit_lib_get_bits(protocol->encoded_data, 128, 8) != 0b11111111) return false;

    // Checksum
    uint8_t checksum = 0;
    uint8_t stripped_byte;
    for(size_t idx = 0; idx < 9; idx++) {
        uint8_t byte = bit_lib_reverse_8_fast(bit_lib_get_bits(
            protocol->encoded_data,
            SECURAKEY_DATA_START_INDEX + (SECURAKEY_BYTE_LENGTH * idx),
            8));
        stripped_byte = byte & 0x7F; // discard the parity bit
        if(bit_lib_test_parity_32(stripped_byte, BitLibParityOdd) != (byte & 0x80) >> 7) {
            return false;
        }
        if(idx < 8) checksum ^= stripped_byte;
    }
    if(stripped_byte != checksum) return false;
    return true;
}

void protocol_SECURAKEY_decoder_start(ProtocolSecurakey* protocol) {
    memset(protocol->data, 0, SECURAKEY_DECODED_DATA_SIZE);
    protocol->inverted = false;
    protocol->got_preamble = false;
}

bool protocol_SECURAKEY_decoder_feed(ProtocolSecurakey* protocol, bool level, uint32_t duration) {
    bool pushed = false;

    if(duration > SECURAKEY_MAX_TIME) return false;

    uint8_t pulses = (uint8_t)round((float)duration / SECURAKEY_CYCLE_LENGTH);

    // Handle last stopbit & preamble (1 sb, 8 bit preamble)
    if(pulses >= 9 && !protocol->got_preamble) {
        pulses = 8;
        protocol->got_preamble = true;
        protocol->inverted = !level;
    } else if(pulses >= 9 && protocol->got_preamble) {
        protocol->got_preamble = false;
    } else if(pulses == 0 && duration > SECURAKEY_MIN_TIME) {
        pulses = 1;
    }

    if(pulses) {
        for(uint8_t i = 0; i < pulses; i++) {
            bit_lib_push_bit(
                protocol->encoded_data,
                SECURAKEY_ENCODED_BYTE_FULL_SIZE,
                level ^ protocol->inverted);
        }
        pushed = true;
    }

    if(pushed && protocol_SECURAKEY_can_be_decoded(protocol)) {
        protocol_SECURAKEY_decode(protocol);
        return true;
    }

    return false;
}

bool protocol_SECURAKEY_encoder_start(ProtocolSecurakey* protocol) {
    memset(protocol->encoded_data, 0, sizeof(protocol->encoded_data));

    uint8_t idbytes[10];
    idbytes[0] = '2';
    idbytes[1] = '0';

    uint8_to_hex_chars(protocol->data, &idbytes[2], 8);

    // insert start and stop bits
    for(size_t i = 0; i < 16; i++) protocol->encoded_data[i] = 0x40 >> ((i + 3) % 5 * 2);

    protocol->encoded_data[0] = 0xFF; // mark + stop
    protocol->encoded_data[1] = 0x20; // start + reflect8(STX)

    uint8_t checksum = 0;
    for(size_t i = 2; i < 13; i++) {
        uint8_t shift = 7 - (i + 3) % 4 * 2;
        uint8_t index = i + (i - 1) / 4;

        uint16_t pattern;
        if(i < 12) {
            pattern = bit_lib_reverse_8_fast(idbytes[i - 2]);
            pattern |= bit_lib_test_parity_32(pattern, BitLibParityOdd);
            if(i > 3) checksum ^= idbytes[i - 2];
        } else {
            pattern = (bit_lib_reverse_8_fast(checksum) & 0xFE) |
                      (bit_lib_test_parity_32(checksum, BitLibParityOdd));
        }
        pattern <<= shift;

        protocol->encoded_data[index] |= pattern >> 8 & 0xFF;
        protocol->encoded_data[index + 1] |= pattern & 0xFF;
    }

    protocol->encoded_index = 0;
    return true;
}

LevelDuration protocol_SECURAKEY_encoder_yield(ProtocolSecurakey* protocol) {
    uint16_t length = SECURAKEY_CLOCKS_IN_US;
    bool bit = bit_lib_get_bit(protocol->encoded_data, protocol->encoded_index);
    bit_lib_increment_index(protocol->encoded_index, SECURAKEY_ENCODED_BIT_SIZE);
    while(bit_lib_get_bit(protocol->encoded_data, protocol->encoded_index) == bit) {
        length += SECURAKEY_CLOCKS_IN_US;
        bit_lib_increment_index(protocol->encoded_index, SECURAKEY_ENCODED_BIT_SIZE);
    }

    return level_duration_make(bit, length);
}

bool protocol_SECURAKEY_write_data(ProtocolSecurakey* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;

    // Correct protocol data by redecoding
    protocol_SECURAKEY_encoder_start(protocol);
    protocol_SECURAKEY_decode(protocol);

    protocol_SECURAKEY_encoder_start(protocol);

    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] = LFRFID_T5577_MODULATION_DIRECT | LFRFID_T5577_BITRATE_RF_32 |
                                  (4 << LFRFID_T5577_MAXBLOCK_SHIFT);
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->encoded_data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->encoded_data, 32, 32);
        request->t5577.block[3] = bit_lib_get_bits_32(protocol->encoded_data, 64, 32);
        request->t5577.block[4] = bit_lib_get_bits_32(protocol->encoded_data, 96, 32);
        request->t5577.blocks_to_write = 5;
        result = true;
    }
    return result;
}

void protocol_SECURAKEY_render_data(ProtocolSecurakey* protocol, FuriString* result) {
    furi_string_printf(result, "CIN: %08lX", bit_lib_get_bits_32(protocol->data, 0, 32));
}

const ProtocolBase protocol_SECURAKEY = {
    .name = "PAC/Stanley",
    .manufacturer = "N/A",
    .data_size = SECURAKEY_DECODED_DATA_SIZE,
    .features = LFRFIDFeatureASK,
    .validate_count = 3,
    .alloc = (ProtocolAlloc)protocol_SECURAKEY_alloc,
    .free = (ProtocolFree)protocol_SECURAKEY_free,
    .get_data = (ProtocolGetData)protocol_SECURAKEY_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_SECURAKEY_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_SECURAKEY_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_SECURAKEY_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_SECURAKEY_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_SECURAKEY_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_SECURAKEY_render_data,
    .write_data = (ProtocolWriteData)protocol_SECURAKEY_write_data,
};
