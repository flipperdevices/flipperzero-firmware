#include "nfc_poller.h"

#include <furi.h>

#include "nfc.h"
#include "protocols/nfca_poller.h"
#include "protocols/mf_ultralight.h"

typedef enum {
    NfcPollerStateIdle,
    NfcPollerStateCheckPresenceNfca,
    NfcPollerStateCheckPresenceNfcb,
    NfcPollerStateCheckPresenceNfcf,
    NfcPollerStateCheckPresenceNfcv,
} NfcPollerState;

struct NfcPoller {
    Nfc* nfc;
    NfcPollerState state;
    NfcPollerEventCallback callback;
    void* context;
};

NfcPoller* nfc_poller_alloc() {
    NfcPoller* instance = malloc(sizeof(NfcPoller));
    
    return instance;
}

void nfc_poller_free(NfcPoller* instance) {
    furi_assert(instance);
}

void nfc_poller_start(NfcPoller* instance, NfcPollerEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}
