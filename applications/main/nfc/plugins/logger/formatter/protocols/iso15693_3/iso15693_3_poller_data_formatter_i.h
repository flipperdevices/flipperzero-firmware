#pragma once

#include "iso15693_3_poller_data_formatter.h"
#include <nfc/protocols/iso15693_3/iso15693_3_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* iso15693_3_poller_data_format_event_type(const Iso15693_3PollerEventType event);

#ifdef __cplusplus
}
#endif
