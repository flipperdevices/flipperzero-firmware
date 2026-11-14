#pragma once

#include "ask_ctx.h"

#include <nfc/protocols/nfc_generic_event.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AskCtxPoller AskCtxPoller;

typedef enum {
    AskCtxPollerEventTypeReadFailed,
    AskCtxPollerEventTypeReadSuccess,
} AskCtxPollerEventType;

typedef union {
    AskCtxError error;
} AskCtxPollerEventData;

typedef struct {
    AskCtxPollerEventType type;
    AskCtxPollerEventData* data;
} AskCtxPollerEvent;

#ifdef __cplusplus
}
#endif
