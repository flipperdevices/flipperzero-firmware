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

static void mrtd_trace(MrtdApplication* app) {
    FuriHalNfcTxRxContext* tx_rx = app->tx_rx;
    if(furi_log_get_level() == FuriLogLevelTrace) {
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
    // Last two bytes are return code
    return (buffer[len-2] << 8) | buffer[len-1];
}

struct EFFormat EF = {
    .ATR          = {.file_id = 0x2F01, .short_id = 0x01 },
    .DIR          = {.file_id = 0x2F00, .short_id = 0x1E },
    .CardAccess   = {.file_id = 0x011C, .short_id = 0x1C },
    .CardSecurity = {.file_id = 0x011D, .short_id = 0x1D },
    .COM          = {.file_id = 0x011E, .short_id = 0x1E },
    .SOD          = {.file_id = 0X011D, .short_id = 0X1D },
    .DG1          = {.file_id = 0X0101, .short_id = 0X01 },
    .DG2          = {.file_id = 0X0102, .short_id = 0X02 },
    .DG3          = {.file_id = 0X0103, .short_id = 0X03 },
    .DG4          = {.file_id = 0X0104, .short_id = 0X04 },
    .DG5          = {.file_id = 0X0105, .short_id = 0X05 },
    .DG6          = {.file_id = 0X0106, .short_id = 0X06 },
    .DG7          = {.file_id = 0X0107, .short_id = 0X07 },
    .DG8          = {.file_id = 0X0108, .short_id = 0X08 },
    .DG9          = {.file_id = 0X0109, .short_id = 0X09 },
    .DG10         = {.file_id = 0X010A, .short_id = 0X0A },
    .DG11         = {.file_id = 0X010B, .short_id = 0X0B },
    .DG12         = {.file_id = 0X010C, .short_id = 0X0C },
    .DG13         = {.file_id = 0X010D, .short_id = 0X0D },
    .DG14         = {.file_id = 0X010E, .short_id = 0X0E },
    .DG15         = {.file_id = 0X010F, .short_id = 0X0F },
    .DG16         = {.file_id = 0X0110, .short_id = 0X10 },
};

/*bool mrtd_send_apdu(MrtdApplication* app, uint8_t* buffer, size_t length) {
    FuriHalNfcTxRxContext* tx_rx = app->tx_rx;

    memcpy(tx_rx->tx_data, buffer, length);
    tx_rx->tx_bits = length * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    //TODO: timeout as param?
    if(furi_hal_nfc_tx_rx(tx_rx, 300)) {
        mrtd_trace(app);
        uint16_t ret_code = mrtd_decode_response(tx_rx->rx_data, tx_rx->rx_bits / 8);
        if(ret_code == 0x9000) {
            return true;
        } else {
            FURI_LOG_E(TAG, "APDU answer is not 0x9000, but 0x%04X", ret_code);
            return false;
        }
    }
    return false;
}*/

bool mrtd_send_apdu(MrtdApplication* app, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t lc, const void* data, int16_t le) {
    FuriHalNfcTxRxContext* tx_rx = app->tx_rx;

    FURI_LOG_D(TAG, "Send APDU, lc: %d, le: %d", lc, le);

    size_t idx = 0;
    tx_rx->tx_data[idx++] = cla;
    tx_rx->tx_data[idx++] = ins;
    tx_rx->tx_data[idx++] = p1;
    tx_rx->tx_data[idx++] = p2;
    if(lc > 0) {
        tx_rx->tx_data[idx++] = lc;
        memcpy(tx_rx->tx_data + idx, data, lc);
        idx += lc;
    }
    if(le >= 0) {
        tx_rx->tx_data[idx++] = le&0xff;
    }

    tx_rx->tx_bits = idx * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    //TODO: timeout as param?
    if(furi_hal_nfc_tx_rx(tx_rx, 300)) {
        mrtd_trace(app);
        uint16_t ret_code = mrtd_decode_response(tx_rx->rx_data, tx_rx->rx_bits / 8);
        //TODO: handle other return codes?
        if(ret_code == 0x9000) {
            return true;
        } else {
            FURI_LOG_E(TAG, "APDU answer is not 0x9000, but 0x%04X", ret_code);
            return false;
        }
    }
    return false;
}

bool mrtd_select(MrtdApplication* app, EFFile file) {
    uint8_t data[] = {file.file_id >> 8, file.file_id & 0xff};
    FURI_LOG_D(TAG, "Send select EF: 0x%04X", file.file_id);
    if(!mrtd_send_apdu(app, 0x00, 0xA4, 0x02, 0x0C, 0x02, data, -1)) {
        FURI_LOG_E(TAG, "Failed select EF 0x%04X", file.file_id);
        return false;
    }

    return true;
}

size_t mrtd_read_binary(MrtdApplication* app, uint8_t* buffer, size_t bufsize, size_t offset) {
    UNUSED(buffer);
    UNUSED(bufsize);
    // 00 B0 offst -
    FURI_LOG_D(TAG, "Read binary, offset: %d", offset);
    if(!mrtd_send_apdu(app, 0x00, 0xB0, offset>>8, offset&0xff, 0x00, NULL, 0)) {
        FURI_LOG_E(TAG, "Failed to read");
        return 0;
    }

    //TODO: copy data to buffer
    //TODO: return read amount

    return 0;
}

void mrtd_read_dump(MrtdApplication* app, EFFile file, const char* descr) {
    FURI_LOG_D(TAG, "Read and dump %s:", descr);
    if(!mrtd_select(app, file)) {
        return;
    }
    uint8_t data[2048];
    size_t read = 0;
    do {
        read = mrtd_read_binary(app, data+read, sizeof(data)-read, read);
    } while(read > 0);
}

void mrtd_test(MrtdApplication* app) {
    FURI_LOG_D(TAG, "Mrtd Test");
    mrtd_read_dump(app, EF.ATR, "EF.ATR");
    mrtd_read_dump(app, EF.DIR, "EF.DIR");
    mrtd_read_dump(app, EF.CardAccess, "EF.CardAccess");
    mrtd_read_dump(app, EF.CardSecurity, "EF.CardSecurity");
}

MrtdApplication* mrtd_alloc_init(FuriHalNfcTxRxContext* tx_rx) {
    MrtdApplication* app = malloc(sizeof(MrtdApplication));

    app->tx_rx = tx_rx;

    return app;
}

void mrtd_free(MrtdApplication* app) {
    furi_assert(app);
    free(app);
}

/*
size_t mrtd_read_data(MrtdApplication* app, uint8_t* buffer, size_t bufsize) {
    //TODO: fill buffer with data from file

    //TODO: use protected APDU if authenticated



    return read;
}
*/

bool mrtd_select_efcardaccess(MrtdApplication* app) {
    FuriHalNfcTxRxContext* tx_rx = app->tx_rx;

    // Chris: short: 0x6700, long: yes (0x9000)
    // Anna: short: 0x6700, long: yes (0x9000)

    EFFile file = EF.CardAccess;

    // ICAO 9303 p10 §3.6.2
    uint8_t select_efcardaccess_cmd[] = {
        0x00, // CLA
        0xA4, // INS
        0x02, // P1
        0x0C, // P2
        0x02, // Lc: Data length
        file.file_id >> 8 | (file.file_id & 0xFF),
        //0x01, 0x1C, // Data: EF.CardAccess File ID
        0x00, // Le
    };

    memcpy(tx_rx->tx_data, select_efcardaccess_cmd, sizeof(select_efcardaccess_cmd));
    tx_rx->tx_bits = sizeof(select_efcardaccess_cmd) * 8;
    tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

    bool efcardaccess_success = false;

    FURI_LOG_D(TAG, "Send select EF.CardAccess");
    if(furi_hal_nfc_tx_rx(tx_rx, 300)) {
        mrtd_trace(app);
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

bool mrtd_select_efdir(MrtdApplication* app) {
    FuriHalNfcTxRxContext* tx_rx = app->tx_rx;

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
        mrtd_trace(app);
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

bool mrtd_select_lds1(MrtdApplication* app) {
    FuriHalNfcTxRxContext* tx_rx = app->tx_rx;

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
        mrtd_trace(app);
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

int mrtd_bac_keyhandshake(MrtdApplication* app) {
    UNUSED(app);

    return 0;
}

bool mrtd_read(MrtdApplication* app) {
    bool mrtd_read = false;

    memset(app, 0, sizeof(MrtdApplication));

    mrtd_read = mrtd_bac_keyhandshake(app);
    return mrtd_read;
}
