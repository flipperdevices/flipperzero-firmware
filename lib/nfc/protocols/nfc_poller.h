#pragma once

#include "nfca.h"

typedef enum {
    NfcTypeUnknown,
    NfcTypeA,
    NfcTypeB,
    NfcTypeF,
    NfcTypeV,
} NfcType;

typedef struct {
    NfcType type;
    union {
        NfcaData nfca_data;
    };
} NfcPoller;

bool nfc_poller_activate(NfcPoller* nfc_poller);

void nfc_poller_reset(NfcPoller* nfc_poller);
