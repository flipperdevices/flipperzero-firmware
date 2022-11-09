
#include <limits.h>
#include "nfcv.h"
#include "slix_l.h"
#include "nfc_util.h"
#include <furi.h>
#include "furi_hal_nfc.h"


bool slix_l_check_card_type(uint8_t UID0, uint8_t UID1, uint8_t UID2) {
    if((UID0 == 0xE0) && (UID1 == 0x04) && (UID2 == 0x03)) {
        return true;
    }
    return false;
}

bool slix_l_read_card(
    NfcVReader* reader,
    FuriHalNfcDevData* nfc_data,
    NfcVData* nfcv_data) {
    furi_assert(reader);
    furi_assert(nfcv_data);

    if(nfcv_read_sysinfo(nfc_data, nfcv_data) != ERR_NONE) {
        return false;
    }

    reader->blocks_to_read = nfcv_data->block_num;
    return (nfcv_read_blocks(reader, nfcv_data) == ERR_NONE);
}

ReturnCode slix_l_get_random(uint8_t* rand) {
    uint16_t received = 0;
    uint8_t rxBuf[32];

    ReturnCode ret = rfalNfcvPollerTransceiveReq(
        ISO15693_CMD_NXP_GET_RANDOM_NUMBER, 
        RFAL_NFCV_REQ_FLAG_DEFAULT, 
        ISO15693_MANUFACTURER_NXP, 
        NULL, 
        NULL,
        0, 
        rxBuf, 
        sizeof(rxBuf), 
        &received);

    if(ret == ERR_NONE) {
        if(received != 3) {
            return ERR_PROTO;
        }
        if(rand != NULL) {
            memcpy(rand, &rxBuf[1], 2);
        }
    }

    return ret;
}

ReturnCode slix_l_unlock(uint32_t id, uint8_t* rand, uint32_t password) {
    furi_assert(rand);
    
    uint16_t received = 0;
    uint8_t rxBuf[32];
    uint8_t cmd_set_pass[] = { 
        id,
        rand[0] ^ ((password>>0) & 0xFF),
        rand[1] ^ ((password>>8) & 0xFF),
        rand[0] ^ ((password>>16) & 0xFF),
        rand[1] ^ ((password>>24) & 0xFF) 
    };

    ReturnCode ret = rfalNfcvPollerTransceiveReq(
        ISO15693_CMD_NXP_SET_PASSWORD, 
        RFAL_NFCV_REQ_FLAG_DATA_RATE, 
        ISO15693_MANUFACTURER_NXP, 
        NULL, 
        cmd_set_pass,
        sizeof(cmd_set_pass), 
        rxBuf, 
        sizeof(rxBuf), 
        &received);

    return ret;
}
