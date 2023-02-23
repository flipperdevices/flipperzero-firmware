#pragma once

#include <toolbox/protocols/protocol.h>

#include "../ibutton_protocol_base_i.h"

typedef enum {
    iButtonProtocolMiscCyfral,
    iButtonProtocolMiscMetakom,
    iButtonProtocolMiscMax,
} iButtonProtocolMisc;

extern const ProtocolBase* ibutton_protocols_misc[];
