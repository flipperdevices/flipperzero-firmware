#include "ibutton_protocols_misc.h"

#include "cyfral.h"
#include "metakom.h"

const ProtocolBase* ibutton_protocols_misc[] = {
    [iButtonProtocolMiscCyfral] = &protocol_cyfral,
    [iButtonProtocolMiscMetakom] = &protocol_metakom,
};
