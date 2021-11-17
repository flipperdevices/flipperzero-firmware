#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "pulse_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PulseDecoder PulseDecoder;

PulseDecoder* pulse_decoder_alloc();
void pulse_decoder_free(PulseDecoder* decoder);
void pulse_decoder_add_protocol(PulseDecoder* decoder, PulseProtocol* protocol, int32_t index);
void pulse_decoder_process_pulse(PulseDecoder* decoder, bool polarity, uint32_t length);
int32_t pulse_decoder_get_decoded_index(PulseDecoder* decoder);
void pulse_decoder_reset(PulseDecoder* decoder);
void pulse_decoder_get_data(PulseDecoder* decoder, int32_t index, uint8_t* data, size_t length);

#ifdef __cplusplus
}
#endif