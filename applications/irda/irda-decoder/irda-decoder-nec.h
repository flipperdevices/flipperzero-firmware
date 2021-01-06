#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    WAIT_PREAMBULA_HIGH,
    WAIT_PREAMBULA_LOW,
    WAIT_RETRY_HIGH,
    WAIT_BIT_HIGH,
    WAIT_BIT_LOW,
    WAIT_BIT_STOP_HIGH,
} IrDANecDecoderState;

typedef struct {
    uint32_t data;
    uint8_t current_data_index;
    IrDANecDecoderState state;
} IrDANecDecoder;

bool process_decoder_nec(IrDANecDecoder* decoder, bool polarity, uint32_t time);