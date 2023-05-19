#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MfDesfireErrorNone,
} MfDesfireError;

typedef struct {
    NfcaData nfca_data;
} MfDesfireData;

bool mf_desfire_detect_protocol(NfcaData* nfca_data);

#ifdef __cplusplus
}
#endif
