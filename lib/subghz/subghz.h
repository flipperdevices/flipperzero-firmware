#pragma once
#include "subghz_protocol_blocks.h"

typedef struct SubGhzProtocolDecoder SubGhzProtocolDecoder;
typedef struct SubGhzProtocolEncoder SubGhzProtocolEncoder;
//typedef struct SubGhzEncoders SubGhzEncoders;

typedef void (
    *SubGhzProtocolDecoderCallback)(SubGhzProtocolDecoder* decoder, void* context, string_t decoder_name);
    //*SubGhzProtocolDecoderCallback)(string_t decoder_name, void* context);
// typedef void (*SubGhzProtocolSpecificationEndDataCallback)(SubGhzEncoderDecoder* callback, void* context);

typedef enum {
    SubGhzProtocolCommonTypeUnknown_ = 0,
    SubGhzProtocolCommonTypeStatic_,
    SubGhzProtocolCommonTypeDynamic_,
    SubGhzProtocolCommonTypeRAW_,
    //.....
} SubGhzProtocolCommonType_;

typedef enum {
    SubGhzProtocolStatusNoProtocol = -1,
    SubGhzProtocolStatusUnknown = 0,
    SubGhzProtocolStatusNoInit,
    SubGhzProtocolStatusOk,
} SubGhzProtocolStatus;

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
    int index_protocol;
    //const SubGhzEncoders* encoder;
};

SubGhzProtocolDecoder* subghz_protocol_decoder_alloc(void);
void subghz_protocol_decoder_free(SubGhzProtocolDecoder* instance);
void subghz_protocol_decoder_decode(SubGhzProtocolDecoder* instance, bool level, uint32_t duration);
void subghz_protocol_decoder_reset(SubGhzProtocolDecoder* instance);
void subghz_protocol_decoder_set_rx_callback(
    SubGhzProtocolDecoder* instance,
    SubGhzProtocolDecoderCallback callback,
    void* context);
SubGhzProtocolStatus subghz_protocol_decoder_serialization(
    SubGhzProtocolDecoder* instance,
    const char* protocol_name,
    string_t output);

SubGhzProtocolEncoder* subghz_protocol_encoder_alloc_init(const char* protocol_name);
void subghz_protocol_encoder_free(SubGhzProtocolEncoder* instance);
SubGhzProtocolStatus subghz_protocol_encoder_stop(SubGhzProtocolEncoder* instance);
SubGhzProtocolStatus subghz_protocol_encoder_load(
    SubGhzProtocolEncoder* instance,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat);
LevelDuration subghz_protocol_encoder_yield(void* context);
