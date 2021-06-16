#include "api-hal-nfc.h"
#include <st25r3916.h>

const uint32_t clocks_in_ms = 64 * 1000;

ReturnCode api_hal_nfc_init() {
    // Check if Nfc worker was started
    if(rfalNfcGetState() > RFAL_NFC_STATE_NOTINIT) {
        return ERR_NONE;
    }
    return rfalNfcInitialize();
}

bool api_hal_nfc_is_busy() {
    return rfalNfcGetState() > RFAL_NFC_STATE_IDLE;
}

void api_hal_nfc_field_on() {
    api_hal_nfc_exit_sleep();
    st25r3916TxRxOn();
}

void api_hal_nfc_field_off() {
    st25r3916TxRxOff();
    api_hal_nfc_start_sleep();
}

void api_hal_nfc_start_sleep() {
    rfalLowPowerModeStart();
}

void api_hal_nfc_exit_sleep() {
    rfalLowPowerModeStop();
}

bool api_hal_nfc_detect(rfalNfcDevice **dev_list, uint8_t* dev_cnt, uint32_t timeout, bool deactivate) {
    furi_assert(dev_list);
    furi_assert(dev_cnt);

    rfalLowPowerModeStop();
    rfalNfcState state = rfalNfcGetState();
    if(state == RFAL_NFC_STATE_NOTINIT) {
        rfalNfcInitialize();
    }
    rfalNfcDiscoverParam params;
    params.compMode = RFAL_COMPLIANCE_MODE_EMV;
    params.techs2Find = RFAL_NFC_POLL_TECH_A | RFAL_NFC_POLL_TECH_B | RFAL_NFC_POLL_TECH_F |
                        RFAL_NFC_POLL_TECH_V | RFAL_NFC_POLL_TECH_AP2P | RFAL_NFC_POLL_TECH_ST25TB;
    params.totalDuration = 1000;
    params.devLimit = 3;
    params.wakeupEnabled = false;
    params.wakeupConfigDefault = true;
    params.nfcfBR = RFAL_BR_212;
    params.ap2pBR = RFAL_BR_424;
    params.maxBR = RFAL_BR_KEEP;
    params.GBLen = RFAL_NFCDEP_GB_MAX_LEN;
    params.notifyCb = NULL;

    uint32_t start = DWT->CYCCNT;
    rfalNfcDiscover(&params);
    while(state != RFAL_NFC_STATE_ACTIVATED) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        FURI_LOG_D("HAL NFC", "Current state %d", state);
        if(state == RFAL_NFC_STATE_POLL_ACTIVATION) {
            start = DWT->CYCCNT;
            continue;
        }
        if(DWT->CYCCNT - start > timeout * clocks_in_ms) {
            rfalNfcDeactivate(true);
            FURI_LOG_D("HAL NFC", "Timeout");
            return false;
        }
        osThreadYield();
    }
    rfalNfcGetDevicesFound(dev_list, dev_cnt);
    if(deactivate) {
        rfalNfcDeactivate(false);
        rfalLowPowerModeStart();
    }
    return true;
}

bool api_hal_nfc_listen(uint32_t timeout) {
    api_hal_nfc_exit_sleep();

    rfalNfcState state = rfalNfcGetState();
    if(state == RFAL_NFC_STATE_NOTINIT) {
        rfalNfcInitialize();
    } else if(state >= RFAL_NFC_STATE_ACTIVATED) {
        rfalNfcDeactivate(false);
    }
    rfalNfcDiscoverParam params;
    params.compMode = RFAL_COMPLIANCE_MODE_EMV;
    params.techs2Find = RFAL_NFC_LISTEN_TECH_A;
    params.totalDuration = 1000;
    params.devLimit = 1;
    params.wakeupEnabled = false;
    params.wakeupConfigDefault = true;
    params.nfcfBR = RFAL_BR_212;
    params.ap2pBR = RFAL_BR_424;
    params.maxBR = RFAL_BR_KEEP;
    params.GBLen = RFAL_NFCDEP_GB_MAX_LEN;
    params.notifyCb = NULL;

    params.lmConfigPA.nfcidLen = RFAL_LM_NFCID_LEN_07;
    params.lmConfigPA.nfcid[0] = 0X36;
    params.lmConfigPA.nfcid[1] = 0x9C;
    params.lmConfigPA.nfcid[2] = 0xE7;
    params.lmConfigPA.nfcid[3] = 0xB1;
    params.lmConfigPA.nfcid[4] = 0x0A;
    params.lmConfigPA.nfcid[5] = 0xC1;
    params.lmConfigPA.nfcid[6] = 0x34;
    params.lmConfigPA.SENS_RES[0] = 0x44;
    params.lmConfigPA.SENS_RES[1] = 0x00;
    params.lmConfigPA.SEL_RES = 0x00;

    uint32_t start = DWT->CYCCNT;
    rfalNfcDiscover(&params);
    while(state != RFAL_NFC_STATE_ACTIVATED) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        FURI_LOG_D("HAL NFC", "Current state %d", state);
        if(DWT->CYCCNT - start > timeout * clocks_in_ms) {
            rfalNfcDeactivate(true);
            return false;
        }
        if(state == RFAL_NFC_STATE_LISTEN_ACTIVATION) {
            start = DWT->CYCCNT;
            continue;
        }
        osThreadYield();
    }
    return true;
}

ReturnCode api_hal_nfc_data_exchange(uint8_t* tx_buff, uint16_t tx_len, uint8_t** rx_buff, uint16_t** rx_len, bool deactivate) {
    furi_assert(rx_buff);
    furi_assert(rx_len);

    ReturnCode ret;
    rfalNfcState state = RFAL_NFC_STATE_ACTIVATED;
    ret = rfalNfcDataExchangeStart(tx_buff, tx_len, rx_buff, rx_len, 0);
    if(ret != ERR_NONE) {
        return ret;
    }
    uint32_t start = DWT->CYCCNT;
    while(state != RFAL_NFC_STATE_DATAEXCHANGE_DONE) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        ret = rfalNfcDataExchangeGetStatus();
        FURI_LOG_D("HAL NFC", "Nfc st: %d Data st: %d", state, ret);
        if(ret > ERR_SLEEP_REQ) {
            return ret;
        }
        if(ret == ERR_BUSY) {
            if(DWT->CYCCNT - start > 1000 * clocks_in_ms) {
                return ERR_TIMEOUT;
            }
            continue;
        } else {
            start = DWT->CYCCNT;
        }
        taskYIELD();
    }
    if(deactivate) {
        rfalNfcDeactivate(false);
        rfalLowPowerModeStart();
    }
    return ERR_NONE;
}

void api_hal_nfc_deactivate() {
    rfalNfcDeactivate(false);
    rfalLowPowerModeStart();
}
