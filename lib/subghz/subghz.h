#pragma once
#include "subghz_protocol_blocks.h"

typedef struct SubGhzProtocolDecoder SubGhzProtocolDecoder;
typedef struct SubGhzProtocolEncoder SubGhzProtocolEncoder;
typedef struct SubGhzEncoders SubGhzEncoders;

typedef void (
    *SubGhzProtocolDecoderCallback)(SubGhzProtocolBlockDecoder* block_decoder, void* context);
// typedef void (*SubGhzProtocolSpecificationEndDataCallback)(SubGhzEncoderDecoder* callback, void* context);

typedef enum {
    SubGhzProtocolCommonTypeUnknown_ = 0,
    SubGhzProtocolCommonTypeStatic_,
    SubGhzProtocolCommonTypeDynamic_,
    SubGhzProtocolCommonTypeRAW_,
    //.....
} SubGhzProtocolCommonType_;

typedef struct {
    const char* name;
    SubGhzProtocolCommonType_ type;
    //SubGhzProtocolCommonflags_ flag;
} SubGhzProtocolSpecification;

struct SubGhzProtocolDecoder {
    void** protocol;
    SubGhzProtocolDecoderCallback callback;
    void* context;
};

struct SubGhzProtocolEncoder {
    void* handler;
    const SubGhzEncoders* encoder;
};

SubGhzProtocolDecoder* subghz_protocol_decoder_alloc(void);
void subghz_protocol_decoder_free(SubGhzProtocolDecoder* instance);
void subghz_protocol_decoder_decode(SubGhzProtocolDecoder* instance, bool level, uint32_t duration);
void subghz_protocol_decoder_reset(SubGhzProtocolDecoder* instance);
void subghz_protocol_decoder_set_rx_callback(
    SubGhzProtocolDecoder* instance,
    SubGhzProtocolDecoderCallback callback,
    void* context);