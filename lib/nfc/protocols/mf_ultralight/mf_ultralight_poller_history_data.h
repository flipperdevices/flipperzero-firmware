#pragma once
#include "mf_ultralight_poller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfUltralightPollerStateIdle,
    MfUltralightPollerStateRequestMode,
    MfUltralightPollerStateReadVersion,
    MfUltralightPollerStateDetectMfulC,
    MfUltralightPollerStateDetectNtag203,
    MfUltralightPollerStateGetFeatureSet,
    MfUltralightPollerStateReadSignature,
    MfUltralightPollerStateReadCounters,
    MfUltralightPollerStateReadTearingFlags,
    MfUltralightPollerStateAuth,
    MfUltralightPollerStateAuthMfulC,
    MfUltralightPollerStateReadPages,
    MfUltralightPollerStateTryDefaultPass,
    MfUltralightPollerStateCheckMfulCAuthStatus,
    MfUltralightPollerStateReadFailed,
    MfUltralightPollerStateReadSuccess,
    MfUltralightPollerStateRequestWriteData,
    MfUltralightPollerStateWritePages,
    MfUltralightPollerStateWriteFail,
    MfUltralightPollerStateWriteSuccess,

    MfUltralightPollerStateNum,
} MfUltralightPollerState;

typedef struct {
    Iso14443_3aPollerEventType event;
    MfUltralightPollerState state;
    MfUltralightError error;
    NfcCommand command;
} FURI_PACKED MfUltralightPollerHistoryData;

#ifdef __cplusplus
}
#endif
