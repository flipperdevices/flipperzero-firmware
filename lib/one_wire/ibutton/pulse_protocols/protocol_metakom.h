#pragma once
#include <stdint.h>
#include "../../pulse_protocols/pulse_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ProtocolMetakom ProtocolMetakom;

ProtocolMetakom* protocol_metakom_alloc();
void protocol_metakom_free(ProtocolMetakom* metakom);
PulseProtocol* protocol_metakom_get_protocol(ProtocolMetakom* metakom);

#ifdef __cplusplus
}
#endif