#include <furi.h>
#include <toolbox/protocols/protocol.h>
#include <lfrfid/tools/fsk_demod.h>
#include <lfrfid/tools/fsk_osc.h>

#define JITTER_TIME (20)
#define MIN_TIME (64 - JITTER_TIME)
#define MAX_TIME (80 + JITTER_TIME)

#define IOPROXXSF_DECODED_DATA_SIZE (4)
#define IOPROXXSF_ENCODED_DATA_SIZE (8)

#define IOPROXXSF_BIT_SIZE (8)
#define IOPROXXSF_BIT_MAX_SIZE (IOPROXXSF_BIT_SIZE * IOPROXXSF_ENCODED_DATA_SIZE)

typedef struct {
    FSKDemod* fsk_demod;
} ProtocolH10301Decoder;

typedef struct {
    FSKOsc* fsk_osc;
    uint8_t encoded_index;
    uint32_t pulse;
} ProtocolH10301Encoder;

typedef struct {
    ProtocolH10301Encoder encoder;
    ProtocolH10301Decoder decoder;
    uint8_t encoded_data[IOPROXXSF_ENCODED_DATA_SIZE];
    uint8_t data[IOPROXXSF_DECODED_DATA_SIZE];
} ProtocolIOProxXSF;

ProtocolIOProxXSF* protocol_io_prox_xsf_alloc(void) {
    ProtocolIOProxXSF* protocol = malloc(sizeof(ProtocolIOProxXSF));
    protocol->decoder.fsk_demod = fsk_demod_alloc(MIN_TIME, 8, MAX_TIME, 6);
    protocol->encoder.fsk_osc = fsk_osc_alloc(8, 10, 64);
    return protocol;
};

void protocol_io_prox_xsf_free(ProtocolIOProxXSF* protocol) {
    fsk_demod_free(protocol->decoder.fsk_demod);
    fsk_osc_free(protocol->encoder.fsk_osc);
    free(protocol);
};

void protocol_io_prox_xsf_set_data(
    ProtocolIOProxXSF* protocol,
    const uint8_t* data,
    size_t data_size) {
    furi_check(data_size >= IOPROXXSF_DECODED_DATA_SIZE);
    memcpy(protocol->data, data, IOPROXXSF_DECODED_DATA_SIZE);
};

void protocol_io_prox_xsf_get_data(ProtocolIOProxXSF* protocol, uint8_t* data, size_t data_size) {
    furi_check(data_size >= IOPROXXSF_DECODED_DATA_SIZE);
    memcpy(data, protocol->data, IOPROXXSF_DECODED_DATA_SIZE);
};

size_t protocol_io_prox_xsf_get_data_size(ProtocolIOProxXSF* protocol) {
    UNUSED(protocol);
    return IOPROXXSF_DECODED_DATA_SIZE;
};

const char* protocol_io_prox_xsf_get_name(ProtocolIOProxXSF* protocol) {
    UNUSED(protocol);
    return "IoProxXSF";
};

const char* protocol_io_prox_xsf_get_manufacturer(ProtocolIOProxXSF* protocol) {
    UNUSED(protocol);
    return "Kantech";
};

void protocol_io_prox_xsf_decoder_start(ProtocolIOProxXSF* protocol) {
    memset(protocol->encoded_data, 0, IOPROXXSF_ENCODED_DATA_SIZE);
};

static uint8_t protocol_io_prox_xsf_compute_checksum(const uint8_t* data) {
    // Packet structure:
    //
    //0        1        2         3         4         5         6         7
    //v        v        v         v         v         v         v         v
    //01234567 8 9ABCDEF0 1 23456789 A BCDEF012 3 456789AB C DEF01234 5 6789ABCD EF
    //00000000 0 VVVVVVVV 1 WWWWWWWW 1 XXXXXXXX 1 YYYYYYYY 1 ZZZZZZZZ 1 CHECKSUM 11
    //
    // algorithm as observed by the proxmark3 folks
    // CHECKSUM == 0xFF - (V + W + X + Y + Z)

    uint8_t checksum = 0;

    checksum += (data[1] << 1) | (data[2] >> 7); // VVVVVVVVV
    checksum += (data[2] << 2) | (data[3] >> 6); // WWWWWWWWW
    checksum += (data[3] << 3) | (data[4] >> 5); // XXXXXXXXX
    checksum += (data[4] << 4) | (data[5] >> 4); // YYYYYYYYY
    checksum += (data[5] << 5) | (data[6] >> 3); // ZZZZZZZZZ

    return 0xFF - checksum;
}

static void protocol_io_prox_xsf_store_data(ProtocolIOProxXSF* protocol, bool data) {
    for(int i = 0; i < 7; ++i) {
        protocol->encoded_data[i] = (protocol->encoded_data[i] << 1) |
                                    ((protocol->encoded_data[i + 1] >> 7) & 1);
    }
    protocol->encoded_data[7] = (protocol->encoded_data[7] << 1) | data;
}

static bool protocol_io_prox_xsf_can_be_decoded(const uint8_t* encoded_data) {
    // Packet framing
    //
    //0        1        2        3        4        5        6        7
    //v        v        v        v        v        v        v        v
    //01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF
    //-----------------------------------------------------------------------
    //00000000 01______ _1______ __1_____ ___1____ ____1___ _____1XX XXXXXX11
    //
    // _ = variable data
    // 0 = preamble 0
    // 1 = framing 1
    // X = checksum

    // Validate the packet preamble is there...
    if(encoded_data[0] != 0b00000000) {
        return false;
    }
    if((encoded_data[1] >> 6) != 0b01) {
        return false;
    }

    // ... check for known ones...
    if((encoded_data[2] & 0b01000000) == 0) {
        return false;
    }
    if((encoded_data[3] & 0b00100000) == 0) {
        return false;
    }
    if((encoded_data[4] & 0b00010000) == 0) {
        return false;
    }
    if((encoded_data[5] & 0b00001000) == 0) {
        return false;
    }
    if((encoded_data[6] & 0b00000100) == 0) {
        return false;
    }
    if((encoded_data[7] & 0b00000011) == 0) {
        return false;
    }

    // ... and validate our checksums.
    uint8_t checksum = protocol_io_prox_xsf_compute_checksum(encoded_data);
    uint8_t checkval = (encoded_data[6] << 6) | (encoded_data[7] >> 2);

    if(checksum != checkval) {
        return false;
    }

    return true;
}

void protocol_io_prox_xsf_decode(const uint8_t* encoded_data, uint8_t* decoded_data) {
    // Packet structure:
    // (Note: the second word seems fixed; but this may not be a guarantee;
    //  it currently has no meaning.)
    //
    //0        1        2        3        4        5        6        7
    //v        v        v        v        v        v        v        v
    //01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF
    //-----------------------------------------------------------------------
    //00000000 01111000 01FFFFFF FF1VVVVV VVV1CCCC CCCC1CCC CCCCC1XX XXXXXX11
    //
    // F = facility code
    // V = version
    // C = code
    // X = checksum

    // Facility code
    decoded_data[0] = (encoded_data[2] << 2) | (encoded_data[3] >> 6);

    // Version code.
    decoded_data[1] = (encoded_data[3] << 3) | (encoded_data[4] >> 5);

    // Code bytes.
    decoded_data[2] = (encoded_data[4] << 4) | (encoded_data[5] >> 4);
    decoded_data[3] = (encoded_data[5] << 5) | (encoded_data[6] >> 3);
}

bool protocol_io_prox_xsf_decoder_feed(ProtocolIOProxXSF* protocol, bool level, uint32_t duration) {
    bool result = false;

    uint32_t count;
    bool value;

    fsk_demod_feed(protocol->decoder.fsk_demod, level, duration, &value, &count);
    for(size_t i = 0; i < count; i++) {
        protocol_io_prox_xsf_store_data(protocol, value);
        if(protocol_io_prox_xsf_can_be_decoded(protocol->encoded_data)) {
            protocol_io_prox_xsf_decode(protocol->encoded_data, protocol->data);
            result = true;
            break;
        }
    }

    return result;
};

void protocol_io_prox_xsf_decoder_reset(ProtocolIOProxXSF* protocol) {
    protocol_io_prox_xsf_decoder_start(protocol);
};

static void write_bit(bool bit, uint8_t position, uint8_t* data) {
    if(bit) {
        data[position / 8] |= 1UL << (7 - (position % 8));
    } else {
        data[position / 8] &= ~(1UL << (7 - (position % 8)));
    }
}

/**
 * Writes up to eight contiguous bits into the output buffer.
 */
static void write_bits(uint8_t byte, uint8_t position, uint8_t* data, uint8_t length) {
    furi_check(length <= 8);
    furi_check(length > 0);

    for(uint8_t i = 0; i < length; ++i) {
        uint8_t shift = 7 - i;
        write_bit((byte >> shift) & 1, position + i, data);
    }
}

static void protocol_io_prox_xsf_encode(const uint8_t* decoded_data, uint8_t* encoded_data) {
    // Packet to transmit:
    //
    // 0           10          20          30          40          50          60
    // v           v           v           v           v           v           v
    // 01234567 8 90123456 7 89012345 6 78901234 5 67890123 4 56789012 3 45678901 23
    // -----------------------------------------------------------------------------
    // 00000000 0 11110000 1 facility 1 version_ 1 code-one 1 code-two 1 checksum 11

    // Preamble.
    write_bits(0b00000000, 0, encoded_data, 8);
    write_bit(0, 8, encoded_data);

    write_bits(0b11110000, 9, encoded_data, 8);
    write_bit(1, 17, encoded_data);

    // Facility code.
    write_bits(decoded_data[0], 18, encoded_data, 8);
    write_bit(1, 26, encoded_data);

    // Version
    write_bits(decoded_data[1], 27, encoded_data, 8);
    write_bit(1, 35, encoded_data);

    // Code one
    write_bits(decoded_data[2], 36, encoded_data, 8);
    write_bit(1, 44, encoded_data);

    // Code two
    write_bits(decoded_data[3], 45, encoded_data, 8);
    write_bit(1, 53, encoded_data);

    // Checksum
    write_bits(protocol_io_prox_xsf_compute_checksum(encoded_data), 54, encoded_data, 8);
    write_bit(1, 62, encoded_data);
    write_bit(1, 63, encoded_data);
}

bool protocol_io_prox_xsf_encoder_start(ProtocolIOProxXSF* protocol) {
    protocol_io_prox_xsf_encode(protocol->data, (uint8_t*)protocol->encoded_data);
    protocol->encoder.encoded_index = 0;
    protocol->encoder.pulse = 0;

    return true;
};

LevelDuration protocol_io_prox_xsf_encoder_yield(ProtocolIOProxXSF* protocol) {
    bool level = 0;
    uint32_t duration = 0;

    // if pulse is zero, we need to output high, otherwise we need to output low
    if(protocol->encoder.pulse == 0) {
        // get bit
        uint8_t bit =
            (protocol->encoded_data[protocol->encoder.encoded_index / IOPROXXSF_BIT_SIZE] >>
             ((IOPROXXSF_BIT_SIZE - 1) - (protocol->encoder.encoded_index % IOPROXXSF_BIT_SIZE))) &
            1;

        // get pulse from oscillator
        bool advance = fsk_osc_next(protocol->encoder.fsk_osc, bit, &duration);

        if(advance) {
            protocol->encoder.encoded_index++;
            if(protocol->encoder.encoded_index >= (IOPROXXSF_BIT_MAX_SIZE)) {
                protocol->encoder.encoded_index = 0;
            }
        }

        // duration diveded by 2 because we need to output high and low
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

void protocol_io_prox_xsf_encoder_reset(ProtocolIOProxXSF* protocol) {
    protocol_io_prox_xsf_encoder_start(protocol);
};

const ProtocolBase protocol_io_prox_xsf = {
    .alloc = (ProtocolAlloc)protocol_io_prox_xsf_alloc,
    .free = (ProtocolFree)protocol_io_prox_xsf_free,
    .set_data = (ProtocolSetData)protocol_io_prox_xsf_set_data,
    .get_data = (ProtocolGetData)protocol_io_prox_xsf_get_data,
    .get_data_size = (ProtocolGetDataSize)protocol_io_prox_xsf_get_data_size,
    .get_name = (ProtocolGetName)protocol_io_prox_xsf_get_name,
    .get_manufacturer = (ProtocolGetManufacturer)protocol_io_prox_xsf_get_manufacturer,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_io_prox_xsf_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_io_prox_xsf_decoder_feed,
            .reset = (ProtocolDecoderReset)protocol_io_prox_xsf_decoder_reset,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_io_prox_xsf_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_io_prox_xsf_encoder_yield,
            .reset = (ProtocolEncoderReset)protocol_io_prox_xsf_encoder_reset,
        },
};