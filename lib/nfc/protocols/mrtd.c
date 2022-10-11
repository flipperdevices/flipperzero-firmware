#include "mrtd.h"

#define TAG "Mrtd"

//TODO: Check EF.DIR first? Before LDS1
//TODO: ICAO 9303 p11 §4.2 steps
//- Read EF.CardAccess (REQUIRED)
//  If not available or does not contain PACE params, try BAC
//- Read EF.DIR (OPTIONAL)
//  Check list of applications present
//- PACE (CONDITIONAL)
//- BAC (CONDITIONAL)

static void mrtd_trace(FuriHalNfcTxRxContext* tx_rx, const char* message) {
    if(furi_log_get_level() == FuriLogLevelTrace) {
        FURI_LOG_T(TAG, "%s", message);
        printf("TX: ");
        for(size_t i = 0; i < tx_rx->tx_bits / 8; i++) {
            printf("%02X ", tx_rx->tx_data[i]);
        }
        printf("\r\nRX: ");
        for(size_t i = 0; i < tx_rx->rx_bits / 8; i++) {
            printf("%02X ", tx_rx->rx_data[i]);
        }
        printf("\r\n");
    }
}

uint16_t mrtd_decode_response(uint8_t* buffer, size_t len) {
    if(len != 2) {
        FURI_LOG_E(TAG, "Expecting 2 byte responses only");
        return 0xffff;
    }

    return (buffer[0] << 8) | buffer[1];
}

bool mrtd_select_efcardaccess(FuriHalNfcTxRxContext* tx_rx, MrtdApplication* mrtd_app) {
    UNUSED(mrtd_app);

    // Chris: short: 0x6700, long: yes (0x9000)
    // Anna: short: 0x6700, long: yes (0x9000)

    // ICAO 9303 p10 §3.6.2
    uint8_t select_efcardaccess_cmd[] = {
        0x00, // CLA
        0xA4, // INS
        0x02, // P1
        0x0C, // P2
        0x02, // Lc: Data length
        0x01, 0x1C, // Data: EF.CardAccess File ID
        0x00, // Le
    };

    memcpy(tx_rx->tx_data, select_efcardaccess_cmd, sizeof(select_efcardaccess_cmd));
    tx_rx->tx_bits = sizeof(select_efcardaccess_cmd) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    bool efcardaccess_success = false;

    FURI_LOG_D(TAG, "Send select EF.CardAccess");
    if(furi_hal_nfc_tx_rx(tx_rx, 300)) {
        mrtd_trace(tx_rx, "Select EF.CardAccess:");
        if(mrtd_decode_response(tx_rx->rx_data, tx_rx->rx_bits / 8) == 0x9000) {
            efcardaccess_success = true;
        } else {
            FURI_LOG_D(TAG, "Select EF.CardAccess response is not 0x9000");
        }
    } else {
        FURI_LOG_E(TAG, "Failed select EF.CardAccess");
    }
    return efcardaccess_success;
}

bool mrtd_select_efdir(FuriHalNfcTxRxContext* tx_rx, MrtdApplication* mrtd_app) {
    UNUSED(mrtd_app);

    // Chris: short: no, long: no (0x6A82)
    // Anna: short: no, long: yes (0x9000)

    uint8_t select_efdir_cmd[] = {
        0x00, // CLA
        0xA4, // INS
        0x02, // P1
        0x0C, // P2
        0x02, // Lc: Data length
        0x2F, 0x00, // Data: EF.DIR File ID
        0x00, // Le
    };

    memcpy(tx_rx->tx_data, select_efdir_cmd, sizeof(select_efdir_cmd));
    tx_rx->tx_bits = sizeof(select_efdir_cmd) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    bool efdir_success = false;

    FURI_LOG_D(TAG, "Send select EF.DIR");
    if(furi_hal_nfc_tx_rx(tx_rx, 300)) {
        mrtd_trace(tx_rx, "Select EF.DIR:");
        if(mrtd_decode_response(tx_rx->rx_data, tx_rx->rx_bits / 8) == 0x9000) {
            efdir_success = true;
        } else {
            FURI_LOG_D(TAG, "Select EF.DIR response is not 0x9000");
        }
    } else {
        FURI_LOG_E(TAG, "Failed select EF.DIR");
    }
    return efdir_success;
}

bool mrtd_select_lds1(FuriHalNfcTxRxContext* tx_rx, MrtdApplication* mrtd_app) {
    UNUSED(mrtd_app);

    uint8_t select_emrtd_cmd[] = {
        0x00, // CLA
        0xA4, // INS
        0x04, // P1
        0x0C, // P2: DF
        0x07, // Lc: Data length
        0xa0,
        0x00,
        0x00,
        0x02,
        0x47,
        0x10,
        0x01, // Data: LDS1 eMRTD Application
        0x00, // Le
    };

    memcpy(tx_rx->tx_data, select_emrtd_cmd, sizeof(select_emrtd_cmd));
    tx_rx->tx_bits = sizeof(select_emrtd_cmd) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    bool lds1_success = false;

    FURI_LOG_D(TAG, "Send select LDS1 eMRTD");
    if(furi_hal_nfc_tx_rx(tx_rx, 300)) {
        mrtd_trace(tx_rx, "Select LDS1:");
        if(mrtd_decode_response(tx_rx->rx_data, tx_rx->rx_bits / 8) == 0x9000) {
            lds1_success = true;
        } else {
            FURI_LOG_D(TAG, "Select LDS1 eMRTD response is not 0x9000");
        }
    } else {
        FURI_LOG_E(TAG, "Failed select LDS1");
    }

    return lds1_success;
}

int mrtd_bac_keyhandshake(FuriHalNfcTxRxContext* tx_rx, MrtdApplication* mrtd_app) {
    UNUSED(tx_rx);
    UNUSED(mrtd_app);

    return 0;
}

bool mrtd_read(FuriHalNfcTxRxContext* tx_rx, MrtdApplication* mrtd_app) {
    furi_assert(tx_rx);
    furi_assert(mrtd_app);
    bool mrtd_read = false;

    memset(mrtd_app, 0, sizeof(MrtdApplication));

    mrtd_read = mrtd_bac_keyhandshake(tx_rx, mrtd_app);
    return mrtd_read;
}
