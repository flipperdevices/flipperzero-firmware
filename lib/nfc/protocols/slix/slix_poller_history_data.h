#pragma once

#include "slix_poller.h"
#include <nfc/protocols/iso15693_3/iso15693_3_poller_history_data.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SlixPollerStateIdle,
    SlixPollerStateGetNxpSysInfo,
    SlixPollerStateReadSignature,
    SlixPollerStateCheckPrivacyPassword,
    SlixPollerStatePrivacyUnlock,
    SlixPollerStateReady,
    SlixPollerStateError,
    SlixPollerStateNum,
} SlixPollerState;

typedef struct {
    Iso15693_3PollerEventType event;
    SlixPollerState state;
    SlixError error;
    NfcCommand command;
} FURI_PACKED SlixPollerHistoryData;

#ifdef __cplusplus
}
#endif