#pragma once

#include "iso14443_4a_poller_data_formatter.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* iso14443_4a_poller_data_format_event_type(const Iso14443_4aPollerEventType event);

#ifdef __cplusplus
}
#endif
