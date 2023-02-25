#include "ibutton_protocols_defs.h"

#include "dallas/ibutton_protocols_dallas.h"
#include "misc/ibutton_protocols_misc.h"

const iButtonProtocolsBase* ibutton_protocols[] = {
    [iButtonProtocolGroupDallas] = &ibutton_protocol_group_dallas,
    [iButtonProtocolGroupMisc] = &ibutton_protocol_group_misc,
};
