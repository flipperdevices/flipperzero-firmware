#pragma once

#include "iso14443_3a_poller_data_formatter.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* iso14443_3a_poller_data_format_event_type(const Iso14443_3aPollerEventType event);

#ifdef __cplusplus
}
#endif
