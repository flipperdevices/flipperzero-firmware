#pragma once

#include "ibutton_protocol_base.h"

typedef enum {
    iButtonProtocolDS1990,
    iButtonProtocolDS1992,
    iButtonProtocolDS1996,
    iButtonProtocolDSGeneric,
    iButtonProtocolMax
} iButtonProtocol;

extern const iButtonProtocolBase* ibutton_protocols[];
