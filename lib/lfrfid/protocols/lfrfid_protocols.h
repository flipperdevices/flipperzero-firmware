#pragma once
#include <toolbox/protocols/protocol.h>

typedef enum {
    LFRFIDProtocolEM4100,
    LFRFIDProtocolH10301,
    // LFRFIDProtocolI40134,
    LFRFIDProtocolIOProxXSF,

    LFRFIDProtocolMax,
} LFRFIDProtocol;

extern const ProtocolBase* lfrfid_protocols[];