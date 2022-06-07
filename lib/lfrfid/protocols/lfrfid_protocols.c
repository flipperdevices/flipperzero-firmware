#include "lfrfid_protocols.h"
#include "protocol_em4100.h"
#include "protocol_h10301.h"
#include "protocol_i40134.h"

const ProtocolBase* lfrfid_protocols[] = {
    [LFRFIDProtocolEM4100] = &protocol_em4100,
    [LFRFIDProtocolH10301] = &protocol_h10301,
    [LFRFIDProtocolI40134] = &protocol_i40134,
};