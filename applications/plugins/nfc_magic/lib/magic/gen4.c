#include "gen4.h"

#include <furi_hal_nfc.h>

#define TAG "Magic"

#define MAGIC_CMD_PREFIX (0xCF)

#define MAGIC_CMD_GET_CFG (0xC6)
#define MAGIC_CMD_WRITE (0xCD)
#define MAGIC_CMD_READ (0xCE)
#define MAGIC_CMD_SET_CFG (0xF0)
#define MAGIC_CMD_FUSE_CFG (0xF1)
#define MAGIC_CMD_SET_PWD (0xFE)

#define MAGIC_BUFFER_SIZE (40)

bool magic_gen4_get_cfg(uint32_t pwd, uint8_t* config) {
    bool is_valid_config_len = false;
    uint8_t tx_data[MAGIC_BUFFER_SIZE] = {};
    uint8_t rx_data[MAGIC_BUFFER_SIZE] = {};
    uint16_t rx_len = 0;
    FuriHalNfcReturn ret = 0;

    FURI_LOG_I(TAG, "magic_gen4_get_cfg pwd %lX", pwd);
    do {
        // Start communication
        tx_data[0] = MAGIC_CMD_PREFIX;
        tx_data[1] = (uint8_t)pwd;
        tx_data[2] = (uint8_t)(pwd >> 8);
        tx_data[3] = (uint8_t)(pwd >> 16);
        tx_data[4] = (uint8_t)(pwd >> 24);
        tx_data[5] = MAGIC_CMD_GET_CFG;
        FURI_LOG_I(TAG, "tx %02X%02X%02X%02X%02X%02X%02X%02X", tx_data[0],tx_data[1],tx_data[2],tx_data[3],tx_data[4],tx_data[5],tx_data[6],tx_data[7]);
        ret = furi_hal_nfc_ll_txrx(
            tx_data,
            6,
            rx_data,
            sizeof(rx_data),
            &rx_len,
            FURI_HAL_NFC_TXRX_DEFAULT,
            furi_hal_nfc_ll_ms2fc(20));
        if(ret != FuriHalNfcReturnOk) break;
        if(rx_len != 30 && rx_len != 32) break;
        memcpy(config, rx_data, rx_len);
        is_valid_config_len = true;
    } while(false);

    return is_valid_config_len;
}