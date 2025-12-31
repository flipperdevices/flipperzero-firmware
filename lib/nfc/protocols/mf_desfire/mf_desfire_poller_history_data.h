#pragma once

#include "mf_desfire.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller.h>
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfDesfirePollerStateIdle,
    MfDesfirePollerStateReadVersion,
    MfDesfirePollerStateReadFreeMemory,
    MfDesfirePollerStateReadMasterKeySettings,
    MfDesfirePollerStateReadMasterKeyVersion,
    MfDesfirePollerStateReadApplicationIds,
    MfDesfirePollerStateReadApplications,
    MfDesfirePollerStateReadFailed,
    MfDesfirePollerStateReadSuccess,

    MfDesfirePollerStateNum,
} MfDesfirePollerState;

typedef struct {
    Iso14443_4aPollerEventType event;
    MfDesfirePollerState state;
    MfDesfireError error;
    NfcCommand command;
} FURI_PACKED MfDesfirePollerHistoryData;

#ifdef __cplusplus
}
#endif
