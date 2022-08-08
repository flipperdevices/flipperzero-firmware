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