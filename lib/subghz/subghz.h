#pragma once

typedef struct SubGhzProtocolDecoder SubGhzProtocolDecoder;
typedef struct SubGhzProtocolEncoder SubGhzProtocolEncoder;
typedef struct SubGhzEncoders SubGhzEncoders;

typedef void (*SubGhzProtocolDecoderCallback)(SubGhzProtocolDecoder* callback, void* context);
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
