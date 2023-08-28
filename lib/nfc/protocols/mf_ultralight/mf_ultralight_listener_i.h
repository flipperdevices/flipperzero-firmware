#pragma once

#include "mf_ultralight_listener.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener.h>
#include <nfc/protocols/nfc_generic_event.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfUltralightListenerAuthStateIdle,
    MfUltralightListenerAuthStateSuccess,
} MfUltralightListenerAuthState;

typedef enum {
    MfUltraligthListenerStateIdle,
} MfUltraligthListenerState;

typedef struct {
    uint8_t enabled;
    uint8_t ascii_offset;
    uint8_t ascii_end;
    uint8_t mirror_last_page;
    MfUltralightMirrorConf actual_mode;
    FuriString* ascii_mirror_data;
} MfUltralightMirrorMode;

struct MfUltralightListener {
    Iso14443_3aListener* iso14443_3a_listener;
    MfUltralightListenerAuthState auth_state;
    MfUltraligthListenerState state;

    MfUltralightData* data;
    BitBuffer* tx_buffer;
    MfUltralightFeatureSupport features;
    MfUltralightConfigPages* config;

    NfcGenericEvent generic_event;
    MfUltralightListenerEvent mfu_event;
    MfUltralightListenerEventData mfu_event_data;
    NfcGenericCallback callback;
    MfUltralightMirrorMode mirror;
    void* context;
};

#ifdef __cplusplus
}
#endif
