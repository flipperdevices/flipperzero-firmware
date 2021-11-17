#pragma once
#include <stdint.h>
#include "../../pulse_protocols/pulse_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ProtocolCyfral ProtocolCyfral;

ProtocolCyfral* protocol_cyfral_alloc();
void protocol_cyfral_free(ProtocolCyfral* cyfral);
PulseProtocol* protocol_cyfral_get_protocol(ProtocolCyfral* cyfral);

#ifdef __cplusplus
}
#endif