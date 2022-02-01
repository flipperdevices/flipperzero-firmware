#pragma once

#include "../types.h"

typedef struct SubGhzProtocolDecoderBase SubGhzProtocolDecoderBase;

typedef void (
    *SubGhzProtocolDecoderBaseRxCallback)(SubGhzProtocolDecoderBase* instance, void* context);

typedef void (
    *SubGhzProtocolDecoderBaseSerialize)(SubGhzProtocolDecoderBase* decoder_base, string_t output);

struct SubGhzProtocolDecoderBase {
    // Decoder general section
    const SubGhzProtocol* protocol;

    // Callback section
    SubGhzProtocolDecoderBaseRxCallback callback;
    void* context;
};

void subghz_protocol_decoder_base_set_decoder_callback(
    SubGhzProtocolDecoderBase* decoder_base,
    SubGhzProtocolDecoderBaseRxCallback callback,
    void* context);

bool subghz_protocol_decoder_base_serialize(
    SubGhzProtocolDecoderBase* decoder_base,
    string_t output);

// Encoder Base
typedef struct SubGhzProtocolEncoderBase SubGhzProtocolEncoderBase;

struct SubGhzProtocolEncoderBase {
    // Decoder general section
    const SubGhzProtocol* protocol;

    // Callback section
};
