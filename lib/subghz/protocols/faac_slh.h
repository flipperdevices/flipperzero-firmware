#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_FAAC_SLH_NAME "Faac SLH"

typedef struct SubGhzProtocolDecoderFaacSLH SubGhzProtocolDecoderFaacSLH;
typedef struct SubGhzProtocolEncoderFaacSLH SubGhzProtocolEncoderFaacSLH;

extern const SubGhzProtocolDecoder subghz_protocol_faac_slh_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_faac_slh_encoder;
extern const SubGhzProtocol subghz_protocol_faac_slh;

void* subghz_protocol_decoder_faac_slh_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_faac_slh_free(void* context);
void subghz_protocol_decoder_faac_slh_reset(void* context);
void subghz_protocol_decoder_faac_slh_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_faac_slh_serialization(void* context, string_t output);