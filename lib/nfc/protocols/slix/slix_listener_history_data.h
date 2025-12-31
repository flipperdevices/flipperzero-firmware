#pragma once

#include "slix_listener.h"
#include <nfc/protocols/iso15693_3/iso15693_3_listener_history_data.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t random;
    bool password_match[SlixPasswordTypeCount];
} SlixListenerSessionState;

typedef struct {
    Iso15693_3ListenerEventType event;
    SlixListenerSessionState session_state;
    SlixError error;
    NfcCommand command;
} FURI_PACKED SlixListenerHistoryData;

#ifdef __cplusplus
}
#endif
