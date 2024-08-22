#pragma once
#include <toolbox/protocols/protocol.h>
#include "../tools/t5577.h"

typedef enum {
    LFRFIDFeatureASK = 1 << 0, /** ASK Demodulation */
    LFRFIDFeaturePSK = 1 << 1, /** PSK Demodulation */
} LFRFIDFeature;

typedef enum {
    LFRFIDProtocolAwid,
    LFRFIDProtocolElectra,
    LFRFIDProtocolEM4100,
    LFRFIDProtocolEM410016,
    LFRFIDProtocolEM410032,
    LFRFIDProtocolFDXA,
    LFRFIDProtocolFDXB,
    LFRFIDProtocolGallagher,
    LFRFIDProtocolGProxII,
    LFRFIDProtocolH10301,
    LFRFIDProtocolHidExGeneric,
    LFRFIDProtocolHidGeneric,
    LFRFIDProtocolIdteck,
    LFRFIDProtocolIndala26,
    LFRFIDProtocolIOProxXSF,
    LFRFIDProtocolJablotron,
    LFRFIDProtocolKeri,
    LFRFIDProtocolNexwatch,
    LFRFIDProtocolPACStanley,
    LFRFIDProtocolParadox,
    LFRFIDProtocolPyramid,
    LFRFIDProtocolSecurakey,
    LFRFIDProtocolViking,
    LFRFIDProtocolMax,
} LFRFIDProtocol;

extern const ProtocolBase* lfrfid_protocols[];

typedef enum {
    LFRFIDWriteTypeT5577,
} LFRFIDWriteType;

typedef struct {
    LFRFIDWriteType write_type;
    union {
        LFRFIDT5577 t5577;
    };
} LFRFIDWriteRequest;
