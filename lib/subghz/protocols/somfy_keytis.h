#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_SOMFY_KEYTIS_NAME "Somfy Keytis"

typedef struct SubGhzProtocolDecoderSomfyKeytis SubGhzProtocolDecoderSomfyKeytis;
typedef struct SubGhzProtocolEncoderSomfyKeytis SubGhzProtocolEncoderSomfyKeytis;

extern const SubGhzProtocolDecoder subghz_protocol_somfy_keytis_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_somfy_keytis_encoder;
extern const SubGhzProtocol subghz_protocol_somfy_keytis;

void* subghz_protocol_decoder_somfy_keytis_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_somfy_keytis_free(void* context);
void subghz_protocol_decoder_somfy_keytis_reset(void* context);
void subghz_protocol_decoder_somfy_keytis_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_somfy_keytis_serialization(void* context, string_t output);
