#pragma once

#include <furi_hal.h>
#include <lib/flipper_file/flipper_file.h>

#define bit_read(value, bit) (((value) >> (bit)) & 0x01)
#define bit_set(value, bit) ((value) |= (1UL << (bit)))
#define bit_clear(value, bit) ((value) &= ~(1UL << (bit)))
#define bit_write(value, bit, bitvalue) (bitvalue ? bit_set(value, bit) : bit_clear(value, bit))
#define DURATION_DIFF(x, y) ((x < y) ? (y - x) : (x - y))

typedef struct SubGhzProtocolBlockDecoder SubGhzProtocolBlockDecoder;
typedef struct SubGhzProtocolBlockGeneric SubGhzProtocolBlockGeneric;

typedef void (*SubGhzProtocolDecoderRxCallback)(SubGhzProtocolBlockGeneric* generic, void* context);
typedef LevelDuration (*SubGhzProtocolCommonEncoderCallback)(void* context);
typedef void (*SubGhzProtocolCommonEncoderCallbackEnd)(void* context);

typedef struct {
    const uint16_t te_long;
    const uint16_t te_short;
    const uint16_t te_delta;
    const uint8_t min_count_bit_for_found;
} SubGhzProtocolBlockConst;

struct SubGhzProtocolBlockGeneric {
    const char* protocol_name;
    uint64_t data;
    uint32_t serial;
    uint8_t data_count_bit;
    uint8_t btn;
    uint16_t cnt;
};

struct SubGhzProtocolBlockDecoder {
    uint32_t parser_step;
    //uint8_t header_count;
    uint32_t te_last;
    uint64_t decode_data;
    uint8_t decode_count_bit;
    SubGhzProtocolDecoderRxCallback callback;
    void* context;
};

typedef struct {
    bool is_runing;
    size_t repeat;
    size_t front;
    size_t size_upload;
    LevelDuration* upload;

    // SubGhzProtocolCommonEncoderCallback callback;
    // SubGhzProtocolCommonEncoderCallbackEnd callback_end;
    // void* context;
    // void* context_end;
} SubGhzProtocolBlockEncoder;

void subghz_protocol_blocks_add_bit(SubGhzProtocolBlockDecoder* decoder, uint8_t bit);

uint64_t subghz_protocol_blocks_reverse_key(uint64_t key, uint8_t count_bit);

void subghz_protocol_blocks_set_decoder_callback(
    SubGhzProtocolBlockDecoder* block_decoder,
    SubGhzProtocolDecoderRxCallback callback,
    void* context);
bool subghz_protocol_block_save_file(
    SubGhzProtocolBlockGeneric* instance,
    FlipperFile* flipper_file);
bool subghz_protocol_block_load_file(
    SubGhzProtocolBlockGeneric* instance,
    FlipperFile* flipper_file);
