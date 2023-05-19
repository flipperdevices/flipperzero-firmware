#pragma once

#include "mf_desfire_poller.h"
#include <lib/nfc/protocols/nfca/nfca_poller_i.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MfDesfirePoller {
    NfcaPoller* nfca_poller;
    NfcPollerBuffer* buffer;
    MfDesfireData* data;
    MfDesfirePollerCallback callback;
    MfDesfireError error;
    void* context;
};

#ifdef __cplusplus
}
#endif
