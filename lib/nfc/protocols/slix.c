
#include <limits.h>
#include "nfcv.h"
#include "slix.h"
#include "nfc_util.h"
#include <furi.h>
#include "furi_hal_nfc.h"

bool slix_check_card_type(FuriHalNfcDevData* nfc_data) {
    if((nfc_data->uid[0] == 0xE0) && (nfc_data->uid[1] == 0x04) && (nfc_data->uid[2] == 0x01) &&
       (((nfc_data->uid[3] >> 4) & 3) == 2)) {
        return true;
    }
    return false;
}

bool slix2_check_card_type(FuriHalNfcDevData* nfc_data) {
    if((nfc_data->uid[0] == 0xE0) && (nfc_data->uid[1] == 0x04) && (nfc_data->uid[2] == 0x01) &&
       (((nfc_data->uid[3] >> 4) & 3) == 1)) {
        return true;
    }
    return false;
}

bool slix_s_check_card_type(FuriHalNfcDevData* nfc_data) {
    if((nfc_data->uid[0] == 0xE0) && (nfc_data->uid[1] == 0x04) && (nfc_data->uid[2] == 0x02)) {
        return true;
    }
    return false;
}

bool slix_l_check_card_type(FuriHalNfcDevData* nfc_data) {
    if((nfc_data->uid[0] == 0xE0) && (nfc_data->uid[1] == 0x04) && (nfc_data->uid[2] == 0x03)) {
        return true;
    }
    return false;
}

ReturnCode slix_l_get_random(NfcVData* data) {
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
        if(data != NULL) {
            data->sub_data.slix_l.rand[0] = rxBuf[2];
            data->sub_data.slix_l.rand[1] = rxBuf[1];
        }
    }

    return ret;
}

ReturnCode slix_l_unlock(NfcVData* data, uint32_t password_id) {
    furi_assert(rand);

    uint16_t received = 0;
    uint8_t rxBuf[32];
    uint8_t cmd_set_pass[] = {
        password_id,
        data->sub_data.slix_l.rand[1],
        data->sub_data.slix_l.rand[0],
        data->sub_data.slix_l.rand[1],
        data->sub_data.slix_l.rand[0]};
    uint8_t* password = NULL;

    switch(password_id) {
    case 4:
        password = data->sub_data.slix_l.key_privacy;
        break;
    case 8:
        password = data->sub_data.slix_l.key_destroy;
        break;
    case 10:
        password = data->sub_data.slix_l.key_eas;
        break;
    default:
        break;
    }

    if(!password) {
        return ERR_NOTSUPP;
    }

    for(int pos = 0; pos < 4; pos++) {
        cmd_set_pass[1 + pos] ^= password[3 - pos];
    }

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
