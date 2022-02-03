#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_NICE_FLOR_S_NAME "Nice FloR-S"

typedef struct SubGhzProtocolDecoderNiceFlorS SubGhzProtocolDecoderNiceFlorS;
typedef struct SubGhzProtocolEncoderNiceFlorS SubGhzProtocolEncoderNiceFlorS;

extern const SubGhzProtocolDecoder subghz_protocol_nice_flor_s_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_nice_flor_s_encoder;
extern const SubGhzProtocol subghz_protocol_nice_flor_s;

void* subghz_protocol_decoder_nice_flor_s_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_nice_flor_s_free(void* context);
void subghz_protocol_decoder_nice_flor_s_reset(void* context);
void subghz_protocol_decoder_nice_flor_s_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_nice_flor_s_serialization(void* context, string_t output);
