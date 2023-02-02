#pragma once
#include <toolbox/protocols/protocol.h>

typedef enum {
    iButtonProtocolMiscCyfral,
    iButtonProtocolMiscMetakom,

    iButtonProtocoMisclMax,
} iButtonProtocolMisc;

extern const ProtocolBase* ibutton_protocols_misc[];
