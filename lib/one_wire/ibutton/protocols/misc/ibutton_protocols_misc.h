#pragma once

#include <toolbox/protocols/protocol.h>

#include "../ibutton_protocol_base.h"

typedef enum {
    iButtonProtocolMiscCyfral,
    iButtonProtocolMiscMetakom,
    iButtonProtocolMiscMax,
} iButtonProtocolMisc;

extern const ProtocolBase* ibutton_protocols_misc[];

// extern const iButtonProtocolBase ibutton_protocol_cyfral;
// extern const iButtonProtocolBase ibutton_protocol_metakom;
