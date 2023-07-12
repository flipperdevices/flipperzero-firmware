#pragma once

#include "mf_classic_listener.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener_i.h>
#include <nfc/protocols/nfc_generic_event.h>
#include "crypto1.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfClassicListenerCommandProcessed,
    MfClassicListenerCommandAck,
    MfClassicListenerCommandNack,
    MfClassicListenerCommandSilent,
} MfClassicListenerCommand;

typedef enum {
    MfClassicListenerStateIdle,
    MfClassicListenerStateAuthComplete,
} MfClassicListenerState;

typedef enum {
    MfClassicListenerAuthStateIdle,
    MfClassicListenerAuthStateStarted,
    MfClassicListenerAuthStatePollerSecretReceived,
} MfClassicListenerAuthState;

typedef enum {
    MfClassicListenerCommStatePlain,
    MfClassicListenerCommStateEncrypted,
} MfClassicListenerCommState;

typedef enum {
    MfClassicListenerCommandTypeOnePart,
    MfClassicListenerCommandTypeTwoParts,
} MfClassicListenerCommandType;

typedef enum {
    MfClassicListenerCommandSecondPartAuth,
    MfClassicListenerCommandSecondPartWrite,
    MfClassicListenerCommandSecondPartInc,
    MfClassicListenerCommandSecondPartDec,

    MfClassicListenerCommandSecondPartNum,
} MfClassicListenerCommandSecondPart;

struct MfClassicListener {
    Iso14443_3aListener* iso14443_3a_listener;
    MfClassicListenerState state;
    MfClassicListenerAuthState auth_state;
    MfClassicListenerCommState comm_state;
    MfClassicListenerCommandType cmd_type;
    MfClassicListenerCommandSecondPart second_part;

    MfClassicData* data;
    BitBuffer* tx_plain_buffer;
    BitBuffer* tx_encrypted_buffer;
    BitBuffer* rx_plain_buffer;

    Crypto1* crypto;
    MfClassicAuthContext auth_context;

    NfcGenericEvent generic_event;
    MfClassicListenerEvent mfc_event;
    MfClassicListenerEventData mfc_event_data;
    NfcGenericCallback callback;
    void* context;

    size_t total_block_num;
};

#ifdef __cplusplus
}
#endif
