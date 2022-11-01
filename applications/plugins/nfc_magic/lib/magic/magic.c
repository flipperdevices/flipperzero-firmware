#include "magic.h"

#include <furi_hal_nfc.h>

#define TAG "Magic"

#define MAGIC_CMD_WUPA (0x40)
#define MAGIC_CMD_WIPE (0x41)
#define MAGIC_CMD_READ (0x43)
#define MAGIC_CMD_WRITE (0x43)

#define MAGIC_ACK (0x0A)

#define MAGIC_BUFFER_SIZE (32)

bool magic_wupa() {
    bool magic_activated = false;
    uint8_t tx_data[MAGIC_BUFFER_SIZE] = {};
    uint8_t rx_data[MAGIC_BUFFER_SIZE] = {};
    uint16_t rx_len = 0;
    FuriHalNfcReturn ret = 0;

    do {
        // Setup nfc poller
        furi_hal_nfc_ll_set_mode(
            FuriHalNfcModePollNfca, FuriHalNfcBitrate106, FuriHalNfcBitrate106);
        furi_hal_nfc_ll_set_fdt_listen(FURI_HAL_NFC_LL_FDT_LISTEN_NFCA_POLLER);
        furi_hal_nfc_ll_set_fdt_poll(FURI_HAL_NFC_LL_FDT_POLL_NFCA_POLLER);
        furi_hal_nfc_ll_txrx_on();

        // Start communication
        tx_data[0] = MAGIC_CMD_WUPA;
        ret = furi_hal_nfc_ll_txrx(
            tx_data,
            7,
            rx_data,
            sizeof(rx_data),
            &rx_len,
            FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_TX_MANUAL | FURI_HAL_NFC_LL_TXRX_FLAGS_AGC_ON |
                +FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_RX_KEEP,
            furi_hal_nfc_ll_ms2fc(20));
        FURI_LOG_I(TAG, "Ret: %d", ret);
        if(ret != FuriHalNfcReturnIncompleteByte) break;
        // if(rx_len != 4) break;
        if(rx_data[0] != MAGIC_ACK) {
            FURI_LOG_E(TAG, "Wrong ack");
            break;
        }
        magic_activated = true;
    } while(false);

    if(!magic_activated) {
        furi_hal_nfc_ll_txrx_off();
    }

    return magic_activated;
}

bool magic_read_block(uint8_t block_num, MfClassicBlock* data) {
    furi_assert(data);
    UNUSED(block_num);

    return true;
}

bool magic_write_blk(uint8_t block_num, MfClassicBlock* data) {
    furi_assert(data);
    UNUSED(block_num);

    return true;
}

bool magic_wipe() {
    return true;
}
