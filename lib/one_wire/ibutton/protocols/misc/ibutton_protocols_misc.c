#include "ibutton_protocols_misc.h"

#include "cyfral.h"
#include "metakom.h"

const ProtocolBase* ibutton_protocols_misc[] = {
    [iButtonProtocolMiscCyfral] = &ibutton_protocol_misc_cyfral,
    [iButtonProtocolMiscMetakom] = &ibutton_protocol_misc_metakom,
};
