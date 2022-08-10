#pragma once
#include <toolbox/protocols/protocol.h>

typedef enum {
    LFRFIDFeatureASK = 1 << 0,
    LFRFIDFeaturePSK = 1 << 1,
} LFRFIDFeature;

typedef enum {
    LFRFIDProtocolEM4100,
    LFRFIDProtocolH10301,
    LFRFIDProtocolIndala26,
    LFRFIDProtocolIOProxXSF,
    LFRFIDProtocolAwid,
    LFRFIDProtocolFDXB,

    LFRFIDProtocolMax,
} LFRFIDProtocol;

extern const ProtocolBase* lfrfid_protocols[];

#define LFRFID_T5577_BLOCK_COUNT 10

typedef enum {
    LFRFIDWriteTypeT5577,
} LFRFIDWriteType;

typedef struct {
    uint32_t block[LFRFID_T5577_BLOCK_COUNT];
    uint32_t blocks_to_write;
} LFRFIDT5577;

typedef struct {
    LFRFIDWriteType write_type;
    union {
        LFRFIDT5577 t5577;
    };
} LFRFIDWriteRequest;