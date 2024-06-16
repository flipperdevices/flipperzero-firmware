#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <toolbox/hex.h>
#include <bit_lib/bit_lib.h>
#include "lfrfid_protocols.h"
#include <toolbox/manchester_decoder.h>

#define TAG "RadioKeyKTH"
#define RKKTH_ENCODED_SIZE_BITS (53)
#define RKKTH_PREAMBLE_SIZE_BITS (11)
#define RKKTH_ENCODED_FULL_SIZE_BITS (RKKTH_ENCODED_SIZE_BITS + RKKTH_PREAMBLE_SIZE_BITS)
#define RKKTH_ENCODED_FULL_SIZE_BYTE (RKKTH_ENCODED_FULL_SIZE_BITS / 8)
#define RKKTH_DECODED_DATA_SIZE_BITS \
    (24) // 16-bit for facility code/number, 16-bit for card number, 16-bit for two checksum
#define RKKTH_DECODED_DATA_SIZE_BYTES (RKKTH_DECODED_DATA_SIZE_BITS / 8)
#define LFRFID_FREQUENCY (125000)
#define RKKTH_CLOCK_PER_BIT (40) // RF/40
#define RKKTH_READ_LONG_TIME \
    (1000000 / (LFRFID_FREQUENCY / RKKTH_CLOCK_PER_BIT)) // 1000000 micro sec / sec
#define RKKTH_READ_SHORT_TIME (RKKTH_READ_LONG_TIME / 2)
#define RKKTH_READ_JITTER_TIME (RKKTH_READ_SHORT_TIME * 40 / 100) // 40% jitter tolerance
#define RKKTH_READ_SHORT_TIME_LOW \
    (RKKTH_READ_SHORT_TIME - RKKTH_READ_JITTER_TIME) // these are used for manchester decoding
#define RKKTH_READ_SHORT_TIME_HIGH (RKKTH_READ_SHORT_TIME + RKKTH_READ_JITTER_TIME)
#define RKKTH_READ_LONG_TIME_LOW (RKKTH_READ_LONG_TIME - RKKTH_READ_JITTER_TIME)
#define RKKTH_READ_LONG_TIME_HIGH (RKKTH_READ_LONG_TIME + RKKTH_READ_JITTER_TIME)

typedef struct {
    uint8_t data[RKKTH_DECODED_DATA_SIZE_BYTES];
    uint8_t encoded_data[RKKTH_ENCODED_FULL_SIZE_BYTE];
    uint8_t encoded_data_index;
    bool encoded_polarity;
    ManchesterState decoder_manchester_state;
} ProtocolSecurakeyRKKTH;

ProtocolSecurakeyRKKTH* protocol_rkkth_alloc(void) {
    ProtocolSecurakeyRKKTH* protocol = malloc(sizeof(ProtocolSecurakeyRKKTH));
    return (void*)protocol;
};

void protocol_rkkth_free(ProtocolSecurakeyRKKTH* protocol) {
    free(protocol);
};

uint8_t* protocol_rkkth_get_data(ProtocolSecurakeyRKKTH* protocol) {
    return protocol->data;
};

static bool protocol_rkkth_can_be_decoded(ProtocolSecurakeyRKKTH* protocol) {
    // check 11 bits preamble
    if(bit_lib_get_bits_16(protocol->encoded_data, 0, RKKTH_PREAMBLE_SIZE_BITS) == 0b01111111110) {
        if(bit_lib_get_bits(protocol->encoded_data, 13, 6) == 26 ||
           bit_lib_get_bits(protocol->encoded_data, 13, 6) == 32) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
};

static void protocol_rkkth_decode(ProtocolSecurakeyRKKTH* protocol) {
    memset(protocol->data, 0, RKKTH_DECODED_DATA_SIZE_BYTES);
    // RKKTH-02 encoded data sometimes look like this
    // N10002? format (preamble and 34-bit card number)
    // preamble     unknown    unknown    unknown    cccccccc   cccccccc   cccccccc
    // 0          1            2           3           4           5           6
    // 0123456789 0 12345678 9 01234567 8 90123456 7 89012345 6 78901234 5 67890123
    // 0111111111 0 00000000 0 00000000 0 00000000 0 00011101 0 00000100 0 01001010
    FURI_LOG_D(TAG, "Securakey RKKTH-02 detected");
    // get card number (c)
    bit_lib_copy_bits(protocol->data, 0, 8, protocol->encoded_data, 38);
    // skip spacers (0s)
    bit_lib_copy_bits(protocol->data, 8, 8, protocol->encoded_data, 47);
    bit_lib_copy_bits(protocol->data, 16, 8, protocol->encoded_data, 56);

    // (decoded) data looks like this
    // cccccccc cccccccc cccccccc
    // 00011101 00000100 01001010
};

void protocol_rkkth_decoder_start(ProtocolSecurakeyRKKTH* protocol) {
    memset(protocol->encoded_data, 0, RKKTH_ENCODED_FULL_SIZE_BYTE);
    manchester_advance(
        protocol->decoder_manchester_state,
        ManchesterEventReset,
        &protocol->decoder_manchester_state,
        NULL);
};

bool protocol_rkkth_decoder_feed(ProtocolSecurakeyRKKTH* protocol, bool level, uint32_t duration) {
    bool result = false;
    // this is where we do manchester demodulation on already ASK-demoded data
    ManchesterEvent event = ManchesterEventReset;
    if(duration > RKKTH_READ_SHORT_TIME_LOW && duration < RKKTH_READ_SHORT_TIME_HIGH) {
        if(!level) {
            event = ManchesterEventShortHigh;
        } else {
            event = ManchesterEventShortLow;
        }
    } else if(duration > RKKTH_READ_LONG_TIME_LOW && duration < RKKTH_READ_LONG_TIME_HIGH) {
        if(!level) {
            event = ManchesterEventLongHigh;
        } else {
            event = ManchesterEventLongLow;
        }
    }
    // append a new bit to the encoded bit stream
    if(event != ManchesterEventReset) {
        bool data;
        bool data_ok = manchester_advance(
            protocol->decoder_manchester_state, event, &protocol->decoder_manchester_state, &data);
        if(data_ok) {
            bit_lib_push_bit(protocol->encoded_data, RKKTH_ENCODED_FULL_SIZE_BYTE, data);
            if(protocol_rkkth_can_be_decoded(protocol)) {
                protocol_rkkth_decode(protocol);
                result = true;
            }
        }
    }
    return result;
};

void protocol_rkkth_render_data(ProtocolSecurakeyRKKTH* protocol, FuriString* result) {
    furi_string_printf(
        result,
        "Card number: %llu",
        bit_lib_get_bits_64(protocol->data, 0, RKKTH_DECODED_DATA_SIZE_BITS));
};

bool protocol_rkkth_encoder_start(ProtocolSecurakeyRKKTH* protocol) {
    // set all of our encoded_data bits to zeros.
    memset(protocol->encoded_data, 0, RKKTH_ENCODED_FULL_SIZE_BYTE);

    // write the preamble to the beginning of the encoded_data
    bit_lib_set_bits(protocol->encoded_data, 0, 0b01111111, 8);
    bit_lib_set_bits(protocol->encoded_data, 8, 0b110, 3);

    // write card number (c)
    bit_lib_copy_bits(protocol->encoded_data, 38, 8, protocol->data, 0);
    // skip spacers (they are zero already by memset)
    bit_lib_copy_bits(protocol->encoded_data, 47, 8, protocol->data, 8);
    bit_lib_copy_bits(protocol->encoded_data, 56, 8, protocol->data, 16);

    // for sending we start at bit 0.
    protocol->encoded_data_index = 0;
    protocol->encoded_polarity = true;
    return true;
};

LevelDuration protocol_rkkth_encoder_yield(ProtocolSecurakeyRKKTH* protocol) {
    bool level = bit_lib_get_bit(protocol->encoded_data, protocol->encoded_data_index);
    uint32_t duration = RKKTH_CLOCK_PER_BIT / 2;
    if(protocol->encoded_polarity) {
        protocol->encoded_polarity = false;
    } else {
        level = !level;
        protocol->encoded_polarity = true;
        bit_lib_increment_index(protocol->encoded_data_index, RKKTH_ENCODED_FULL_SIZE_BITS);
    }
    return level_duration_make(level, duration);
};

bool protocol_rkkth_write_data(ProtocolSecurakeyRKKTH* protocol, void* data) {
    LFRFIDWriteRequest* request = (LFRFIDWriteRequest*)data;
    bool result = false;
    // Correct protocol data by redecoding
    protocol_rkkth_encoder_start(protocol);
    protocol_rkkth_decode(protocol);
    protocol_rkkth_encoder_start(protocol);
    // Write T5577
    if(request->write_type == LFRFIDWriteTypeT5577) {
        request->t5577.block[0] =
            (LFRFID_T5577_MODULATION_MANCHESTER | LFRFID_T5577_BITRATE_RF_40 |
             (2
              << LFRFID_T5577_MAXBLOCK_SHIFT)); // we only need 2 32-bit blocks for our 64-bit encoded data
        request->t5577.block[1] = bit_lib_get_bits_32(protocol->encoded_data, 0, 32);
        request->t5577.block[2] = bit_lib_get_bits_32(protocol->encoded_data, 32, 32);
        request->t5577.blocks_to_write = 3;
        result = true;
    }
    return result;
};

const ProtocolBase protocol_radiokey_kth = {
    .name = "RKKTH",
    .manufacturer = "Securakey",
    .data_size = RKKTH_DECODED_DATA_SIZE_BYTES,
    .features = LFRFIDFeatureASK,
    .validate_count = 3,
    .alloc = (ProtocolAlloc)protocol_rkkth_alloc,
    .free = (ProtocolFree)protocol_rkkth_free,
    .get_data = (ProtocolGetData)protocol_rkkth_get_data,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_rkkth_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_rkkth_decoder_feed,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_rkkth_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_rkkth_encoder_yield,
        },
    .render_data = (ProtocolRenderData)protocol_rkkth_render_data,
    .render_brief_data = (ProtocolRenderData)protocol_rkkth_render_data,
    .write_data = (ProtocolWriteData)protocol_rkkth_write_data,
};