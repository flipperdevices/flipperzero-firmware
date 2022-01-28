#pragma once

//#include <m-string.h>
#include <furi_hal.h>
//#include <stdint.h>

typedef struct SubGhzProtocolBlockDecoder SubGhzProtocolBlockDecoder;

typedef void (*SubGhzProtocolDecoderRxCallback)(SubGhzProtocolBlockDecoder* decoder, void* context);
typedef LevelDuration (*SubGhzProtocolCommonEncoderCallback)(void* context);
typedef void (*SubGhzProtocolCommonEncoderCallbackEnd)(void* context);

typedef struct {
    uint16_t te_long;
    uint16_t te_short;
    uint16_t te_delta;
    uint8_t code_min_count_bit_for_found;
} SubGhzProtocolBlockConst;

typedef struct {
    uint64_t code_found;
    uint8_t code_count_bit;
    uint8_t code_last_count_bit;
    uint64_t code_last_found;
    uint8_t btn;
    uint32_t serial;
    uint16_t cnt;
} SubGhzProtocolBlockRuntime;

struct SubGhzProtocolBlockDecoder {
    uint32_t parser_step;
    uint8_t header_count;
    uint32_t te_last;
    SubGhzProtocolDecoderRxCallback callback;
    void* context;
};

typedef struct {
    bool start;
    size_t repeat;
    size_t front;
    size_t size_upload;
    LevelDuration* upload;

    SubGhzProtocolCommonEncoderCallback callback;
    SubGhzProtocolCommonEncoderCallbackEnd callback_end;
    void* context;
    void* context_end;
} SubGhzProtocolBlockEncoders;

void subghz_protocol_blocks_add_bit(SubGhzProtocolBlockRuntime* runtime, uint8_t bit);

void subghz_protocol_blocks_set_decoder_callback(
    SubGhzProtocolBlockDecoder* block_decoder,
    SubGhzProtocolDecoderRxCallback callback,
    void* context);
