#pragma once
#include <toolbox/protocols/protocol_dict.h>
#include "protocols/lfrfid_protocols.h"

#ifdef __cplusplus
extern "C" {
#endif

bool lfrfid_dict_file_save(ProtocolDict* dict, ProtocolId protocol, const char* filename);

ProtocolId lfrfid_dict_file_load(ProtocolDict* dict, const char* filename);

#ifdef __cplusplus
}
#endif