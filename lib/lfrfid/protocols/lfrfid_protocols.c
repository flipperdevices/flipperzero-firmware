#include "lfrfid_protocols.h"
#include "protocol_em4100.h"
#include "protocol_h10301.h"
#include "protocol_i40134.h"
#include "protocol_io_prox_xsf.h"
#include "protocol_awid.h"

const ProtocolBase* lfrfid_protocols[] = {
    [LFRFIDProtocolEM4100] = &protocol_em4100,
    [LFRFIDProtocolH10301] = &protocol_h10301,
    // [LFRFIDProtocolI40134] = &protocol_i40134,
    [LFRFIDProtocolIOProxXSF] = &protocol_io_prox_xsf,
    [LFRFIDProtocolAwid] = &protocol_awid,
};