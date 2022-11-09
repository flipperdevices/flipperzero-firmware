#include <limits.h>
#include "nfcv.h"
#include "nfc_util.h"
#include <furi.h>
#include "furi_hal_nfc.h"

#define TAG "NfcV"

ReturnCode nfcv_inventory(uint8_t* uid) {
    uint16_t received = 0;
    rfalNfcvInventoryRes res;

    /* TODO: needs proper abstraction via fury_hal(_ll)_* */
    ReturnCode ret = rfalNfcvPollerInventory(RFAL_NFCV_NUM_SLOTS_1, 0, NULL, &res, &received);

    if(ret == ERR_NONE) {
        if(uid != NULL) {
            memcpy(uid, res.UID, 8);
        }
    }

    return ret;
}

ReturnCode nfcv_read_blocks(
    NfcVReader* reader,
    NfcVData* data) {
    
    reader->blocks_read = 0;

    uint16_t received = 0;
    for(size_t block = 0; block < data->block_num; block++) {
        uint8_t rxBuf[32];
        FURI_LOG_D(TAG, "Reading block %d", block);

        ReturnCode ret = rfalNfcvPollerReadSingleBlock(
            RFAL_NFCV_REQ_FLAG_DEFAULT, NULL, block, 
            rxBuf, sizeof(rxBuf), &received);

        if(ret != ERR_NONE) {
            FURI_LOG_D(TAG, "failed to read: %d", ret);
            return ret;
        }
        memcpy(&(data->data[block * data->block_size]), &rxBuf[1], data->block_size);
        FURI_LOG_D(TAG, "  %02X %02X %02X %02X", 
            data->data[block * data->block_size + 0], data->data[block * data->block_size + 1], 
            data->data[block * data->block_size + 2], data->data[block * data->block_size + 3]);

        reader->blocks_read++;
    }

    FURI_LOG_D(TAG, "Read %d blocks", reader->blocks_read);

    return ERR_NONE;
}

ReturnCode nfcv_read_sysinfo(FuriHalNfcDevData* nfc_data, NfcVData* data) {
    uint8_t rxBuf[32];
    uint16_t received = 0;

    FURI_LOG_D(TAG, "Read SystemInformation...");

    ReturnCode ret = rfalNfcvPollerGetSystemInformation(
            RFAL_NFCV_REQ_FLAG_DEFAULT, NULL, 
            rxBuf, sizeof(rxBuf), &received);

    if(ret == ERR_NONE) {
        nfc_data->type = FuriHalNfcTypeV;
        nfc_data->uid_len = 8;
        /* UID is stored reversed in this structure */
        for(int pos = 0; pos < nfc_data->uid_len; pos++) {
            nfc_data->uid[pos] = rxBuf[2 + (7 - pos)];
        }
        data->dsfid = rxBuf[10];
        data->afi = rxBuf[11];
        data->block_num = rxBuf[12] + 1;
        data->block_size = rxBuf[13] + 1;
        data->ic_ref = rxBuf[14];
        FURI_LOG_D(TAG, "  DSFID %d, AFI %d, Blocks %d, Size %d, IC Ref %d", data->dsfid, data->afi, data->block_num, data->block_size, data->ic_ref);
        return ret;
    }
    FURI_LOG_D(TAG, "Failed: %d", ret);

    return ret;
}