#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <lib/nfc/protocols/mf_classic/mf_classic.h>
#include <lib/nfc/protocols/nfcb/nfcb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcDevProtocolNfca,
    // NfcDevProtocolNfcb,
    // NfcDevProtocolNfcf,
    // NfcDevProtocolNfcv,
    NfcDevProtocolMfUltralight,
    NfcDevProtocolMfClassic,
    NfcDevProtocolMfDesfire,

    NfcDevProtocolNum,
} NfcDevProtocol;

// TODO rename to NfcData?
typedef struct {
    NfcDevProtocol protocol;
    union {
        NfcaData nfca_data;
        MfUltralightData mf_ul_data;
        MfClassicData mf_classic_data;
    };
} NfcDevData;

#ifdef __cplusplus
}
#endif
