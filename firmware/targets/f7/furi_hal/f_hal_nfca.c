#include <f_hal_nfc_i.h>

#include <furi.h>
#include <lib/drivers/st25r3916_reg.h>
#include <lib/drivers/st25r3916.h>

#define TAG "FuriHalNfcA"

FHalNfcError f_hal_nfca_send_short_frame(FHalNfcaShortFrame frame) {
    FHalNfcError error = FHalNfcErrorNone;

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    // Disable crc check
    st25r3916_set_reg_bits(handle, ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx);

    st25r3916_write_reg(handle, ST25R3916_REG_NUM_TX_BYTES2, 0);
    uint32_t interrupts =
        (ST25R3916_IRQ_MASK_FWL | ST25R3916_IRQ_MASK_TXE | ST25R3916_IRQ_MASK_RXS |
         ST25R3916_IRQ_MASK_RXE | ST25R3916_IRQ_MASK_PAR | ST25R3916_IRQ_MASK_CRC |
         ST25R3916_IRQ_MASK_ERR1 | ST25R3916_IRQ_MASK_ERR2 | ST25R3916_IRQ_MASK_NRE);
    // Clear interrupts
    st25r3916_get_irq(handle);
    // Enable interrupts
    st25r3916_mask_irq(handle, ~interrupts);
    if(frame == FHalNfcaShortFrameAllReq) {
        st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_REQA);
    } else {
        st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WUPA);
    }

    furi_hal_spi_release(handle);

    return error;
}

FHalNfcError f_hal_nfca_send_sdd_frame(uint8_t* tx_data, uint16_t tx_bits) {
    FHalNfcError error = FHalNfcErrorNone;
    // TODO Set anticollision parameters
    error = f_hal_nfc_poller_tx(tx_data, tx_bits);

    return error;
}

FHalNfcError
    f_hal_nfca_receive_sdd_frame(uint8_t* rx_data, uint16_t rx_data_size, uint16_t* rx_bits) {
    FHalNfcError error = FHalNfcErrorNone;
    UNUSED(rx_data);
    UNUSED(rx_bits);
    UNUSED(rx_data_size);

    error = f_hal_nfc_poller_rx(rx_data, rx_data_size, rx_bits);
    // TODO reset anticollision parameters here

    return error;
}

FHalNfcError
    furi_hal_nfca_set_col_res_data(uint8_t* uid, uint8_t uid_len, uint8_t* atqa, uint8_t sak) {
    furi_assert(uid);
    furi_assert(atqa);
    UNUSED(uid_len);
    UNUSED(sak);
    FHalNfcError error = FHalNfcErrorNone;

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    // Set 4 or 7 bytes UID
    if(uid_len == 4) {
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_AUX,
            ST25R3916_REG_AUX_nfc_id_mask,
            ST25R3916_REG_AUX_nfc_id_4bytes);
    } else {
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_AUX,
            ST25R3916_REG_AUX_nfc_id_mask,
            ST25R3916_REG_AUX_nfc_id_7bytes);
    }
    // Write PT Memory
    uint8_t pt_memory[15] = {};
    memcpy(pt_memory, uid, uid_len);
    pt_memory[10] = atqa[0];
    pt_memory[11] = atqa[1];
    if(uid_len == 4) {
        pt_memory[12] = sak & ~0x04;
    } else {
        pt_memory[12] = 0x04;
    }
    pt_memory[13] = sak & ~0x04;
    pt_memory[14] = sak & ~0x04;

    st25r3916_write_pta_mem(handle, pt_memory, sizeof(pt_memory));
    furi_hal_spi_release(handle);

    return error;
}
