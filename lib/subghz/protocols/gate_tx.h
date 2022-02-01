#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_GATE_TX_NAME "GateTX"

typedef struct SubGhzProtocolDecoderGateTx SubGhzProtocolDecoderGateTx;
typedef struct SubGhzProtocolEncoderGateTx SubGhzProtocolEncoderGateTx;

extern const SubGhzProtocolDecoder subghz_protocol_gate_tx_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_gate_tx_encoder;
extern const SubGhzProtocol subghz_protocol_gate_tx;

void* subghz_protocol_encoder_gate_tx_alloc(SubGhzEnvironment* environment);
void subghz_protocol_encoder_gate_tx_free(void* context);
bool subghz_protocol_encoder_gate_tx_load(
    void* context,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat);
void subghz_protocol_encoder_gate_tx_stop(void* context);
LevelDuration subghz_protocol_encoder_gate_tx_yield(void* context);
void* subghz_protocol_decoder_gate_tx_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_gate_tx_free(void* context);
void subghz_protocol_decoder_gate_tx_reset(void* context);
void subghz_protocol_decoder_gate_tx_feed(void* context, bool level, uint32_t duration);
void subghz_protocol_decoder_gate_tx_serialization(void* context, string_t output);
bool subghz_protocol_gate_tx_save_file(void* context, FlipperFile* flipper_file);
bool subghz_protocol_gate_tx_load_file(
    void* context,
    FlipperFile* flipper_file,
    const char* file_path);