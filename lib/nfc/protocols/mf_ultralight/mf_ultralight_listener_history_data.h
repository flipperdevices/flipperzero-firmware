#pragma once
#include "mf_ultralight_listener.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfUltralightCommandNotFound,
    MfUltralightCommandProcessed,
    MfUltralightCommandProcessedACK,
    MfUltralightCommandProcessedSilent,
    MfUltralightCommandNotProcessedNAK,
    MfUltralightCommandNotProcessedSilent,
    MfUltralightCommandNotProcessedAuthNAK,
} MfUltralightCommand;

typedef struct {
    Iso14443_3aListenerEventType event;
    MfUltralightCommand mfu_command;
    NfcCommand command;
} FURI_PACKED MfUltralightListenerHistoryData;

#ifdef __cplusplus
}
#endif