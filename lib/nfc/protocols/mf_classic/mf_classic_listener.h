#pragma once

#include "mf_classic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfClassicListener MfClassicListener;

typedef enum {
    MfClassicListenerEventTypeAuthContextPartCollected,
    MfClassicListenerEventTypeAuthContextFullCollected,
} MfClassicListenerEventType;

typedef union {
    MfClassicAuthContext auth_context;
} MfClassicListenerEventData;

typedef struct {
    MfClassicListenerEventType type;
    MfClassicListenerEventData* data;
} MfClassicListenerEvent;

typedef enum {
    MfClassicListenerStateIdle,
    MfClassicListenerStateAuthComplete,
} MfClassicListenerState;

typedef enum {
    MfClassicListenerCommandProcessed,
    MfClassicListenerCommandAck,
    MfClassicListenerCommandNack,
    MfClassicListenerCommandSilent,
    MfClassicListenerCommandSleep,
} MfClassicListenerCommand;

typedef enum {
    MfClassicListenerCommStatePlain,
    MfClassicListenerCommStateEncrypted,
} MfClassicListenerCommState;

#ifdef __cplusplus
}
#endif
