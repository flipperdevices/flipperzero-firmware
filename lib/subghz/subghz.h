#pragma once

typedef struct SubGhzProtocolDecoder SubGhzProtocolDecoder;

typedef void (*SubGhzProtocolDecoderCallback)(SubGhzProtocolDecoder* callback, void* context);
// typedef void (*SubGhzProtocolSpecificationEndDataCallback)(SubGhzEncoderDecoder* callback, void* context);