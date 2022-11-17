#pragma once

#include "nfca.h"
#include "nfc_iso_dep.h"

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
        NfcIsoDep nfc_iso_dep;
    };
} NfcPoller;

bool nfc_poller_detect(NfcPoller* nfc_poller);

void nfc_poller_reset(NfcPoller* nfc_poller);
