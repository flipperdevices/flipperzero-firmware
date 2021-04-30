#include "api-hal-nfc.h"
#include <st25r3916.h>

static bool dev_is_found = false;

ReturnCode api_hal_nfc_init() {
    return rfalNfcInitialize();
}

void api_hal_nfc_field_on() {
    st25r3916TxRxOn();
}

void api_hal_nfc_field_off() {
    rfalFieldOff();
}

void api_hal_nfc_start_sleep() {
    rfalLowPowerModeStart();
}

void api_hal_nfc_exit_sleep() {
    rfalLowPowerModeStop();
}

static void api_hal_nfc_change_state_cb(rfalNfcState st) {
    FURI_LOG_D("HAL NFC", "NFC worker state: %d", st);
    if(st >= RFAL_NFC_STATE_POLL_SELECT) {
        dev_is_found = true;
    }
}

bool api_hal_nfc_detect(rfalNfcDevice **dev_list, uint8_t* dev_cnt, uint32_t timeout) {
    furi_assert(dev_list);
    furi_assert(dev_cnt);

    rfalNfcDiscoverParam params;
    params.compMode = RFAL_COMPLIANCE_MODE_EMV;
    params.techs2Find = RFAL_NFC_POLL_TECH_A | RFAL_NFC_POLL_TECH_B | RFAL_NFC_POLL_TECH_F |
                        RFAL_NFC_POLL_TECH_V | RFAL_NFC_POLL_TECH_AP2P | RFAL_NFC_POLL_TECH_ST25TB;
    params.totalDuration = timeout;
    params.devLimit = 3;
    params.wakeupEnabled = false;
    params.wakeupConfigDefault = true;
    params.nfcfBR = RFAL_BR_212;
    params.ap2pBR = RFAL_BR_424;
    params.maxBR = RFAL_BR_KEEP;
    params.GBLen = RFAL_NFCDEP_GB_MAX_LEN;
    params.notifyCb = api_hal_nfc_change_state_cb;

    dev_is_found = false;
    rfalNfcDiscover(&params);
    while(--timeout) {
        rfalNfcWorker();
        FURI_LOG_I("HAL NFC", "Current state %d", rfalNfcGetState());
        if(dev_is_found) {
            rfalNfcGetDevicesFound(dev_list, dev_cnt);
            FURI_LOG_D("HAL NFC", "Found %d devices", dev_cnt);
            break;
        }
        osThreadYield();
    }
    rfalNfcDeactivate(false);
    rfalFieldOff();
    if(!timeout) {
        FURI_LOG_D("HAL NFC", "Timeout");
        return false;
    }

    return true;
}
