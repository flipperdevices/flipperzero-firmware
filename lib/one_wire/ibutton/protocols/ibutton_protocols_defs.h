#pragma once

#include "ibutton_protocols_base.h"

typedef enum {
    iButtonProtocolGroupDallas,
    iButtonProtocolGroupMisc,
    iButtonProtocolGroupMax
} iButtonProtocolGroup;

extern const iButtonProtocolGroupBase* ibutton_protocol_groups[];
