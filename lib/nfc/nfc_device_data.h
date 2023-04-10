#pragma once

#include "protocols/nfca.h"
#include "protocols/mf_ultralight.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcDevProtocolNfca,
    NfcDevProtocolNfcb,
    NfcDevProtocolNfcf,
    NfcDevProtocolNfcv,
    NfcDevProtocolMfUltralight,
    NfcDevProtocolMfClassic,
    NfcDevProtocolMfDesfire,
} NfcDevProtocol;

typedef struct {
    NfcDevProtocol protocol;
    union {
        NfcaData nfca_data;
        MfUltralightData mf_ul_data;
    };
} NfcDevData;

#ifdef __cplusplus
}
#endif

