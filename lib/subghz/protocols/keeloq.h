#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_KEELOQ_NAME "KeeLoq"

typedef struct SubGhzProtocolDecoderKeeloq SubGhzProtocolDecoderKeeloq;
typedef struct SubGhzProtocolEncoderKeeloq SubGhzProtocolEncoderKeeloq;

extern const SubGhzProtocolDecoder subghz_protocol_keeloq_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_keeloq_encoder;
extern const SubGhzProtocol subghz_protocol_keeloq;

void* subghz_protocol_encoder_keeloq_alloc(SubGhzEnvironment* environment);
void subghz_protocol_encoder_keeloq_free(void* context);
bool subghz_protocol_encoder_keeloq_load(
    void* context,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat);
void subghz_protocol_encoder_keeloq_stop(void* context);
LevelDuration subghz_protocol_encoder_keeloq_yield(void* context);
void* subghz_protocol_decoder_keeloq_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_keeloq_free(void* context);
void subghz_protocol_decoder_keeloq_reset(void* context);
void subghz_protocol_decoder_keeloq_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_keeloq_serialization(void* context, string_t output);
bool subghz_protocol_keeloq_save_file(void* context, FlipperFormat* flipper_file);
bool subghz_protocol_keeloq_load_file(
    void* context,
    FlipperFormat* flipper_file,
    const char* file_path);
