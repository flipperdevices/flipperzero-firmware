#pragma once

#include "iso14443_3a_listener_data_formatter.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_listener.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* iso14443_3a_listener_data_format_event_type(const Iso14443_3aListenerEventType event);

#ifdef __cplusplus
}
#endif
