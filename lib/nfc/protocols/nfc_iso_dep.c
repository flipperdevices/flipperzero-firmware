#include "nfc_iso_dep.h"
#include <furi_hal_nfc.h>

#define TAG "NfcIsoDep"

bool nfc_iso_dep_check_tag(uint8_t sak) {
    return FURI_BIT(sak, 5) == 1;
}

bool nfc_iso_dep_nfca_activate(NfcIsoDep* nfc_iso_dep) {
    furi_assert(nfc_iso_dep);

    FuriHalNfcReturn ret = FuriHalNfcReturnOk;

    bool acitivated = false;
    do {
        if(!nfca_poller_activate(&nfc_iso_dep->nfc_data)) {
            FURI_LOG_D(TAG, "Failed to activate NFC-A tag");
            break;
        }
        rfalIsoDepInitialize();
        ret = rfalIsoDepPollAHandleActivation(
            (rfalIsoDepFSxI)RFAL_ISODEP_FSDI_DEFAULT,
            RFAL_ISODEP_NO_DID,
            RFAL_BR_KEEP,
            &nfc_iso_dep->iso_dev);
        if(ret != FuriHalNfcReturnOk) {
            FURI_LOG_D(TAG, "Failed to activate IsoDep layer");
            break;
        }

        acitivated = true;
    } while(false);

    return acitivated;
}

void nfc_iso_dep_deactivate() {
    rfalIsoDepDeselect();
}
