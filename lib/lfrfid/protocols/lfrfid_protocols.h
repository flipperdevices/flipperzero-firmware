#pragma once
#include <toolbox/protocols/protocol.h>
#include "../tools/t55xx.h"

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

typedef enum {
    LFRFIDWriteTypeT55XX,
} LFRFIDWriteType;

typedef struct {
    LFRFIDWriteType write_type;
    union {
        LFRFIDT55XX t55xx;
    };
} LFRFIDWriteRequest;