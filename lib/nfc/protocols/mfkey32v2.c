#include "mfkey32v2.h"

#include "nfc_util.h"

#define TAG "Mfkey32v2"

bool mfkey32v2_collect_nonce(FuriHalNfcTxRxContext* tx_rx, Mfkey32v2Nonce* nonce) {
    if(!((tx_rx->rx_data[0] == 0x60) || (tx_rx->rx_data[0] == 0x61))) {
        FURI_LOG_E(TAG, "Not AUTH command");
        for(size_t i = 0; i < tx_rx->rx_bits / 8; i++) {
            printf("%02X ", tx_rx->rx_data[i]);
        }
        printf("\n");
        return false;
    }
    uint8_t block = tx_rx->rx_data[1];
    mfkey_params.sector = mf_classic_get_sector_by_block(block);
    if(tx_rx->rx_data[0] == 0x60) {
        mfkey_params.key_type = MfClassicKeyA;
    } else {
        mfkey_params.key_type = MfClassicKeyB;
    }

    mfkey_params.nt0 = prng_successor(DWT->CYCCNT, 32);
    nfc_util_num2bytes(mfkey_params.nt0, 4, nt);
    memcpy(tx_rx->tx_data, nt, sizeof(nt));
    tx_rx->tx_parity[0] = 0;
    for(size_t i = 0; i < sizeof(nt); i++) {
        tx_rx->tx_parity[0] |= nfc_util_odd_parity8(nt[i]) << (7 - i);
    }
    tx_rx->tx_bits = sizeof(nt) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTransparent;
    if(!furi_hal_nfc_tx_rx(tx_rx, 500)) {
        FURI_LOG_E(TAG, "Error in NT exchange");
        return false;
    }

    if(tx_rx->rx_bits != 64) {
        FURI_LOG_W(TAG, "Incorrect nr + ar");
        return false;
    }

    mfkey_params.nr0 = nfc_util_bytes2num(tx_rx->rx_data, 4);
    mfkey_params.ar0 = nfc_util_bytes2num(&tx_rx->rx_data[4], 4);

    FURI_LOG_I(TAG, "Reset emulation to collect 2nd pair ar + nr");

    furi_hal_nfc_stop_cmd();

}

bool mfkey32v2_collect(FuriHalNfcTxRxContext* tx_rx, Mfkey32v2Params* params) {
    FuriHalNfcDevData nfc_data = {
        .uid = {0x36, 0x9C, 0xe7, 0xb1, 0x0A, 0xC1, 0x34},
        .uid_len = 7,
        .atqa = {0x44, 0x00},
        .sak = 0x08,
        .type = FuriHalNfcTypeA,
    };
    Mfkey32v2Params mfkey_params = {
        .cuid = 0xb10AC134,
    };
    uint8_t nt[4];

    FURI_LOG_I(TAG, "Start emulation to collect 1st pair ar + nr");

    furi_hal_nfc_listen_start(&nfc_data);
    if(!furi_hal_nfc_listen_rx(tx_rx, 300)) {
        FURI_LOG_D(TAG, "Rx timeout");
        return false;
    }

    bool received_auth = true;
    while(!((tx_rx->rx_data[0] == 0x60) || (tx_rx->rx_data[0] == 0x61))) {
        if(!furi_hal_nfc_listen_rx(tx_rx, 1000)) {
            received_auth = false;
            break;
        }
    }
    if(!received_auth) {
        FURI_LOG_E(TAG, "RX timeout");
        return false;
    }

    if(tx_rx->rx_bits != 4 * 8) {
        FURI_LOG_E(TAG, "Incorrect auth. Expected 3 bytes, received %d", tx_rx->rx_bits / 8);
        return false;
    }
    if(!((tx_rx->rx_data[0] == 0x60) || (tx_rx->rx_data[0] == 0x61))) {
        FURI_LOG_E(TAG, "Not AUTH command");
        for(size_t i = 0; i < tx_rx->rx_bits / 8; i++) {
            printf("%02X ", tx_rx->rx_data[i]);
        }
        printf("\n"); 
        return false;
    }

    uint8_t block = tx_rx->rx_data[1];
    mfkey_params.sector = mf_classic_get_sector_by_block(block);
    if(tx_rx->rx_data[0] == 0x60) {
        mfkey_params.key_type = MfClassicKeyA;
    } else {
        mfkey_params.key_type = MfClassicKeyB;
    }

    mfkey_params.nt0 = prng_successor(DWT->CYCCNT, 32);
    nfc_util_num2bytes(mfkey_params.nt0, 4, nt);
    memcpy(tx_rx->tx_data, nt, sizeof(nt));
    tx_rx->tx_parity[0] = 0;
    for(size_t i = 0; i < sizeof(nt); i++) {
        tx_rx->tx_parity[0] |= nfc_util_odd_parity8(nt[i]) << (7 - i);
    }
    tx_rx->tx_bits = sizeof(nt) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTransparent;
    if(!furi_hal_nfc_tx_rx(tx_rx, 500)) {
        FURI_LOG_E(TAG, "Error in NT exchange");
        return false;
    }

    if(tx_rx->rx_bits != 64) {
        FURI_LOG_W(TAG, "Incorrect nr + ar");
        return false;
    }

    mfkey_params.nr0 = nfc_util_bytes2num(tx_rx->rx_data, 4);
    mfkey_params.ar0 = nfc_util_bytes2num(&tx_rx->rx_data[4], 4);

    FURI_LOG_I(TAG, "Reset emulation to collect 2nd pair ar + nr");

    furi_hal_nfc_stop_cmd();
    furi_delay_ms(5);

    furi_hal_nfc_listen_start(&nfc_data);
    if(!furi_hal_nfc_listen_rx(tx_rx, 300)) {
        FURI_LOG_D(TAG, "Rx timeout");
        return false;
    }

    received_auth = true;
    while(!((tx_rx->rx_data[0] == 0x60) || (tx_rx->rx_data[0] == 0x61))) {
        if(!furi_hal_nfc_listen_rx(tx_rx, 10000)) {
            received_auth = false;
            break;
        }
    }
    if(!received_auth) {
        FURI_LOG_E(TAG, "RX timeout");
        return false;
    }

    if(tx_rx->rx_bits != 4 * 8) {
        FURI_LOG_E(TAG, "Incorrect auth. Expected 3 bytes, received %d", tx_rx->rx_bits / 8);
        return false;
    }
    if(!((tx_rx->rx_data[0] == 0x60) || (tx_rx->rx_data[0] == 0x61))) {
        FURI_LOG_E(TAG, "Not AUTH command");
        return false;
    }

    block = tx_rx->rx_data[1];
    mfkey_params.sector = mf_classic_get_sector_by_block(block);
    // if(plain_data[0] == 0x60) {
    //     mfkey_params.key_type = MfClassicKeyA;
    // } else {
    //     mfkey_params.key_type = MfClassicKeyB;
    // }

    mfkey_params.nt1 = prng_successor(DWT->CYCCNT, 32);
    nfc_util_num2bytes(mfkey_params.nt1, 4, nt);
    memcpy(tx_rx->tx_data, nt, sizeof(nt));
    tx_rx->tx_parity[0] = 0;
    for(size_t i = 0; i < sizeof(nt); i++) {
        tx_rx->tx_parity[0] |= nfc_util_odd_parity8(nt[i]) << (7 - i);
    }
    tx_rx->tx_bits = sizeof(nt) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTransparent;
    if(!furi_hal_nfc_tx_rx(tx_rx, 500)) {
        FURI_LOG_E(TAG, "Error in NT exchange");
        return false;
    }

    if(tx_rx->rx_bits != 64) {
        FURI_LOG_W(TAG, "Incorrect nr + ar");
        return false;
    }

    mfkey_params.nr1 = nfc_util_bytes2num(tx_rx->rx_data, 4);
    mfkey_params.ar1 = nfc_util_bytes2num(&tx_rx->rx_data[4], 4);

    *params = mfkey_params;
    furi_hal_nfc_stop_cmd();

    return true;
}

bool mfkey32v2_get_key(Mfkey32v2Params* params, uint64_t* key) {
    UNUSED(key);
    FURI_LOG_D(
        TAG,
        "Start mfkey32v2 for secor%d, key %c, cuid: %08x %08x %08x %08x %08x %08x %08x",
        params->sector,
        params->key_type == MfClassicKeyA ? 'A' : 'B',
        params->cuid,
        params->nt0,
        params->nr0,
        params->ar0,
        params->nt1,
        params->nr1,
        params->ar1);

    return true;
}
