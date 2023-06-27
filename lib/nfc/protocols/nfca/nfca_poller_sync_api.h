#pragma once

#include "nfca.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

NfcaError nfca_poller_read(Nfc* nfc, NfcaData* nfca_data);

#ifdef __cplusplus
}
#endif
