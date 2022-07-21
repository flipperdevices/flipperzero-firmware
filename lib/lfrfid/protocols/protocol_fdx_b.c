#include <furi.h>
#include "toolbox/level_duration.h"
#include "protocol_fdx_b.h"
#include <toolbox/manchester_decoder.h>
#include <lfrfid/tools/bit_lib.h>

#define FDX_B_ENCODED_BIT_SIZE (128)
#define FDX_B_ENCODED_BYTE_SIZE (((FDX_B_ENCODED_BIT_SIZE) / 8))
#define FDX_B_PREAMBLE_BIT_SIZE (11)
#define FDX_B_PREAMBLE_BYTE_SIZE (2)
#define FDX_B_ENCODED_BYTE_FULL_SIZE (FDX_B_ENCODED_BYTE_SIZE + FDX_B_PREAMBLE_BYTE_SIZE)

typedef struct {
    bool last_short;
} ProtocolFDXBDecoder;

typedef struct {
    ProtocolFDXBDecoder decoder;
    // ProtocolFDXBEncoder encoder;
    uint8_t encoded_data[FDX_B_ENCODED_BYTE_FULL_SIZE];
    // uint8_t data[FDXB_DECODED_DATA_SIZE];
} ProtocolFDXB;

ProtocolFDXB* protocol_fdx_b_alloc(void) {
    ProtocolFDXB* protocol = malloc(sizeof(ProtocolFDXB));
    return protocol;
};

void protocol_fdx_b_free(ProtocolFDXB* protocol) {
    free(protocol);
};

void protocol_fdx_b_set_data(ProtocolFDXB* protocol, const uint8_t* data, size_t data_size) {
    UNUSED(protocol);
    UNUSED(data);
    UNUSED(data_size);
};

void protocol_fdx_b_get_data(ProtocolFDXB* protocol, uint8_t* data, size_t data_size) {
    UNUSED(protocol);
    UNUSED(data);
    UNUSED(data_size);
};

size_t protocol_fdx_b_get_data_size(ProtocolFDXB* protocol) {
    UNUSED(protocol);
    return 11;
};

const char* protocol_fdx_b_get_name(ProtocolFDXB* protocol) {
    UNUSED(protocol);
    return "FDX-B";
};

const char* protocol_fdx_b_get_manufacturer(ProtocolFDXB* protocol) {
    UNUSED(protocol);
    return "FDX-B";
};

void protocol_fdx_b_decoder_start(ProtocolFDXB* protocol) {
    // memset(protocol->data, 0, EM4100_DECODED_DATA_SIZE);
    memset(protocol->encoded_data, 0, FDX_B_ENCODED_BYTE_FULL_SIZE);
    protocol->decoder.last_short = false;
};

#define FDX_B_SHORT_TIME (128)
#define FDX_B_LONG_TIME (256)
#define FDX_B_JITTER_TIME (60)

#define FDX_B_SHORT_TIME_LOW (FDX_B_SHORT_TIME - FDX_B_JITTER_TIME)
#define FDX_B_SHORT_TIME_HIGH (FDX_B_SHORT_TIME + FDX_B_JITTER_TIME)
#define FDX_B_LONG_TIME_LOW (FDX_B_LONG_TIME - FDX_B_JITTER_TIME)
#define FDX_B_LONG_TIME_HIGH (FDX_B_LONG_TIME + FDX_B_JITTER_TIME)

static bool protocol_fdx_b_can_be_decoded(ProtocolFDXB* protocol) {
    bool result = false;

    /*
msb		lsb
0   10000000000	  Header pattern. 11 bits.
11    1nnnnnnnn	
20    1nnnnnnnn	  38 bit (12 digit) National code.
29    1nnnnnnnn	  eg. 000000001008 (decimal).
38    1nnnnnnnn	
47    1nnnnnncc	  10 bit (3 digit) Country code.
56    1cccccccc	  eg. 999 (decimal).
64    1s-------	  1 bit data block status flag.
73    1-------a	  1 bit animal application indicator.
82    1xxxxxxxx	  16 bit checksum.
91    1xxxxxxxx	
100   1eeeeeeee	  24 bits of extra data if present.
109   1eeeeeeee	  eg. $123456.
118   1eeeeeeee	
*/

    do {
        // check 11 bits preamble
        if(bit_lib_get_bits_16(protocol->encoded_data, 0, 11) != 0b10000000000) break;
        // check next 11 bits preamble
        if(bit_lib_get_bits_16(protocol->encoded_data, 128, 11) != 0b10000000000) break;
        // check control bits
        if(!bit_lib_test_parity(protocol->encoded_data, 3, 13 * 9, BitLibParityAlways1, 9)) break;

        result = true;
    } while(false);

    return result;
}

void protocol_fdx_b_decode(ProtocolFDXB* protocol) {
    UNUSED(protocol);

    printf("\r\n");
    for(size_t i = 0; i < FDX_B_ENCODED_BIT_SIZE; i++) {
        printf("%d", bit_lib_get_bit(protocol->encoded_data, i));
        if((i % 8) == 7) printf(" ");
    }

    bit_lib_remove_bit_every_nth(protocol->encoded_data, 3, 13 * 9, 9);

    for(size_t i = 0; i < 11; i++)
        bit_lib_push_bit(protocol->encoded_data, FDX_B_ENCODED_BYTE_FULL_SIZE, 0);

    printf("\r\n");
    for(size_t i = 0; i < FDX_B_ENCODED_BIT_SIZE; i++) {
        printf("%d", bit_lib_get_bit(protocol->encoded_data, i));
        if((i % 8) == 7) printf(" ");
    }

    // 0  nnnnnnnn
    // 8  nnnnnnnn	  38 bit (12 digit) National code.
    // 16 nnnnnnnn	  eg. 000000001008 (decimal).
    // 24 nnnnnnnn
    // 32 nnnnnnnn	  10 bit (3 digit) Country code.
    // 40 cccccccc	  eg. 999 (decimal).
    // 48 s-------	  1 bit data block status flag.
    // 56 -------a	  1 bit animal application indicator.
    // 64 xxxxxxxx	  16 bit checksum.
    // 72 xxxxxxxx
    // 80 eeeeeeee	  24 bits of extra data if present.
    // 88 eeeeeeee	  eg. $123456.
    // 92 eeeeeeee

    // TODO: 38 bits of national code
    uint32_t national_code = bit_lib_get_bits_32(protocol->encoded_data, 0, 32);
    bit_lib_reverse_bits((uint8_t*)&national_code, 0, 32);

    // 10 bit of country code
    uint16_t country_code = bit_lib_get_bits_16(protocol->encoded_data, 38, 10) << 6;
    bit_lib_reverse_bits((uint8_t*)&country_code, 0, 16);

    bool block_status = bit_lib_get_bit(protocol->encoded_data, 48);
    bool rudi_bit = bit_lib_get_bit(protocol->encoded_data, 49);
    uint8_t reserved = bit_lib_get_bits(protocol->encoded_data, 50, 5);
    uint8_t user_info = bit_lib_get_bits(protocol->encoded_data, 55, 5);
    uint8_t replacement_number = bit_lib_get_bits(protocol->encoded_data, 60, 3);
    bool animal_flag = bit_lib_get_bit(protocol->encoded_data, 63);

    uint32_t extended = bit_lib_get_bits_32(protocol->encoded_data, 80, 24) << 8;
    bit_lib_reverse_bits((uint8_t*)&extended, 0, 32);

    uint8_t ex_parity = (extended & 0x100) >> 8;
    uint8_t ex_temperature = extended & 0xff;
    uint8_t ex_calc_parity = bit_lib_test_parity_u32(ex_temperature, BitLibParityOdd);
    bool ex_temperature_present = (ex_calc_parity == ex_parity) && !(extended & 0xe00);

    printf("\r\n");
    printf("National code: %ld\r\n", national_code);
    printf("Country code: %d\r\n", country_code);
    printf("Block status: %d\r\n", block_status);
    printf("Rudi bit: %d\r\n", rudi_bit);
    printf("Reserved: %d\r\n", reserved);
    printf("User info: %d\r\n", user_info);
    printf("Replacement number: %d\r\n", replacement_number);
    printf("Animal flag: %d\r\n", animal_flag);

    if(ex_temperature_present) {
        float temerature_f = 74 + ex_temperature * 0.2;
        float temerature_c = (temerature_f - 32) / 1.8;
        printf("Temperature: %.2f F / %.2f C\r\n", (double)temerature_f, (double)temerature_c);
    }
}

/*
rfid raw_analyze /ext/fdxb.raw
*/

bool protocol_fdx_b_decoder_feed(ProtocolFDXB* protocol, bool level, uint32_t duration) {
    bool result = false;
    UNUSED(protocol);
    UNUSED(level);
    UNUSED(duration);

    bool pushed = false;

    // Bi-Phase Manchester decoding
    if(duration >= FDX_B_SHORT_TIME_LOW && duration <= FDX_B_SHORT_TIME_HIGH) {
        if(protocol->decoder.last_short == false) {
            protocol->decoder.last_short = true;
        } else {
            pushed = true;
            bit_lib_push_bit(protocol->encoded_data, FDX_B_ENCODED_BYTE_FULL_SIZE, false);
            protocol->decoder.last_short = false;
        }
    } else if(duration >= FDX_B_LONG_TIME_LOW && duration <= FDX_B_LONG_TIME_HIGH) {
        if(protocol->decoder.last_short == false) {
            pushed = true;
            bit_lib_push_bit(protocol->encoded_data, FDX_B_ENCODED_BYTE_FULL_SIZE, true);
        } else {
            // reset
        }
    } else {
        // reset
    }

    if(pushed && protocol_fdx_b_can_be_decoded(protocol)) {
        protocol_fdx_b_decode(protocol);
        result = true;
    }

    return result;
};

void protocol_fdx_b_decoder_reset(ProtocolFDXB* protocol) {
    protocol_fdx_b_decoder_start(protocol);
};

bool protocol_fdx_b_encoder_start(ProtocolFDXB* protocol) {
    UNUSED(protocol);
    return true;
};

LevelDuration protocol_fdx_b_encoder_yield(ProtocolFDXB* protocol) {
    UNUSED(protocol);
    return level_duration_reset();
};

void protocol_fdx_b_encoder_reset(ProtocolFDXB* protocol) {
    UNUSED(protocol);
};

const ProtocolBase protocol_fdx_b = {
    .alloc = (ProtocolAlloc)protocol_fdx_b_alloc,
    .free = (ProtocolFree)protocol_fdx_b_free,
    .set_data = (ProtocolSetData)protocol_fdx_b_set_data,
    .get_data = (ProtocolGetData)protocol_fdx_b_get_data,
    .get_data_size = (ProtocolGetDataSize)protocol_fdx_b_get_data_size,
    .get_name = (ProtocolGetName)protocol_fdx_b_get_name,
    .get_manufacturer = (ProtocolGetManufacturer)protocol_fdx_b_get_manufacturer,
    .decoder =
        {
            .start = (ProtocolDecoderStart)protocol_fdx_b_decoder_start,
            .feed = (ProtocolDecoderFeed)protocol_fdx_b_decoder_feed,
            .reset = (ProtocolDecoderReset)protocol_fdx_b_decoder_reset,
        },
    .encoder =
        {
            .start = (ProtocolEncoderStart)protocol_fdx_b_encoder_start,
            .yield = (ProtocolEncoderYield)protocol_fdx_b_encoder_yield,
            .reset = (ProtocolEncoderReset)protocol_fdx_b_encoder_reset,
        },
};