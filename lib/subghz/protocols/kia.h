#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_KIA_NAME "KIA Seed"

typedef struct SubGhzProtocolDecoderKIA SubGhzProtocolDecoderKIA;
typedef struct SubGhzProtocolEncoderKIA SubGhzProtocolEncoderKIA;

extern const SubGhzProtocolDecoder subghz_protocol_kia_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_kia_encoder;
extern const SubGhzProtocol subghz_protocol_kia;

void* subghz_protocol_decoder_kia_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_kia_free(void* context);
void subghz_protocol_decoder_kia_reset(void* context);
void subghz_protocol_decoder_kia_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_kia_serialization(void* context, string_t output);