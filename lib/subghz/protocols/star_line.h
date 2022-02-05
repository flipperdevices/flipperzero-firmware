#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_STAR_LINE_NAME "Star Line"

typedef struct SubGhzProtocolDecoderStarLine SubGhzProtocolDecoderStarLine;
typedef struct SubGhzProtocolEncoderStarLine SubGhzProtocolEncoderStarLine;

extern const SubGhzProtocolDecoder subghz_protocol_star_line_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_star_line_encoder;
extern const SubGhzProtocol subghz_protocol_star_line;

void* subghz_protocol_decoder_star_line_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_star_line_free(void* context);
void subghz_protocol_decoder_star_line_reset(void* context);
void subghz_protocol_decoder_star_line_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_star_line_serialization(void* context, string_t output);