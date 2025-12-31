#pragma once

#include "iso15693_3_listener_data_formatter.h"
#include <nfc/protocols/iso15693_3/iso15693_3_listener.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* iso15693_3_listener_data_format_event_type(const Iso15693_3ListenerEventType event);

#ifdef __cplusplus
}
#endif
