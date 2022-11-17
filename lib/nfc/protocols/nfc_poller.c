#include "nfc_poller.h"

#include <furi_hal_nfc.h>

#define TAG "NfcPoller"

void nfc_poller_reset(NfcPoller* nfc_poller) {
    furi_assert(nfc_poller);
    memset(nfc_poller, 0, sizeof(NfcPoller));

    furi_hal_nfc_field_off();
}

bool nfc_poller_detect(NfcPoller* nfc_poller) {
    furi_assert(nfc_poller);
    memset(nfc_poller, 0, sizeof(NfcPoller));

    bool detected = false;

    if(nfca_poller_check_presence()) {
        FURI_LOG_T(TAG, "NFC-A detected");
        nfc_poller->type = NfcTypeA;
        nfca_poller_deactivate();
    }

    if(nfc_poller->type == NfcTypeA) {
        if(nfca_poller_activate(&nfc_poller->nfca_data)) {
            FURI_LOG_T(TAG, "NFC-A anticollision passed");
            nfca_poller_deactivate();
            detected = true;
        }
    }

    return detected;
}
