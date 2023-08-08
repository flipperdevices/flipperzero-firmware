#include "f_hal_nfc_i.h"

static FHalNfcError f_hal_nfc_iso15693_common_init(FuriHalSpiBusHandle* handle) {
    // Common NFC-V settings, 26.48 kbps

    // 1st stage zero = 12 kHz, 3rd stage zero = 80 kHz, low-pass = 600 kHz
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_RX_CONF1,
        ST25R3916_REG_RX_CONF1_z12k | ST25R3916_REG_RX_CONF1_h80 |
            ST25R3916_REG_RX_CONF1_lp_600khz);

    // Enable AGC
    // AGC Ratio 6
    // AGC algorithm with RESET (recommended for ISO15693)
    // AGC operation during complete receive period
    // Squelch automatic activation on TX end
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_RX_CONF2,
        ST25R3916_REG_RX_CONF2_agc6_3 | ST25R3916_REG_RX_CONF2_agc_m |
            ST25R3916_REG_RX_CONF2_agc_en | ST25R3916_REG_RX_CONF2_sqm_dyn);

    // HF operation, full gain on AM and PM channels
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF3, 0x00);
    // No gain reduction on AM and PM channels
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF4, 0x00);

    // Collision detection level 53%
    // AM & PM summation before digitizing on
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_CORR_CONF1,
        ST25R3916_REG_CORR_CONF1_corr_s0 | ST25R3916_REG_CORR_CONF1_corr_s1 |
            ST25R3916_REG_CORR_CONF1_corr_s4);
    // 424 kHz subcarrier stream mode on
    st25r3916_write_reg(handle, ST25R3916_REG_CORR_CONF2, ST25R3916_REG_CORR_CONF2_corr_s8);
    return FHalNfcErrorNone;
}

static FHalNfcError f_hal_nfc_iso15693_poller_init(FuriHalSpiBusHandle* handle) {
    // Enable Subcarrier Stream mode, OOK modulation
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_MODE,
        ST25R3916_REG_MODE_om_mask | ST25R3916_REG_MODE_tr_am,
        ST25R3916_REG_MODE_om_subcarrier_stream | ST25R3916_REG_MODE_tr_am_ook);

    // Subcarrier 424 kHz mode
    // 8 sub-carrier pulses in report period
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_STREAM_MODE,
        ST25R3916_REG_STREAM_MODE_scf_sc424 | ST25R3916_REG_STREAM_MODE_stx_106 |
            ST25R3916_REG_STREAM_MODE_scp_8pulses);

    // Use regulator AM, resistive AM disabled
    st25r3916_clear_reg_bits(
        handle,
        ST25R3916_REG_AUX_MOD,
        ST25R3916_REG_AUX_MOD_dis_reg_am | ST25R3916_REG_AUX_MOD_res_am);

    return f_hal_nfc_iso15693_common_init(handle);
}

static FHalNfcError f_hal_nfc_iso15693_poller_deinit(FuriHalSpiBusHandle* handle) {
    UNUSED(handle);
    return FHalNfcErrorNone;
}

static FHalNfcError f_hal_nfc_iso15693_listener_init(FuriHalSpiBusHandle* handle) {
    // TODO: Init listener
    return f_hal_nfc_iso15693_common_init(handle);
}

static FHalNfcError f_hal_nfc_iso15693_listener_deinit(FuriHalSpiBusHandle* handle) {
    UNUSED(handle);
    return FHalNfcErrorNone;
}

const FHalNfcTechBase f_hal_nfc_iso15693 = {
    .poller =
        {
            .init = f_hal_nfc_iso15693_poller_init,
            .deinit = f_hal_nfc_iso15693_poller_deinit,
        },

    .listener =
        {
            .init = f_hal_nfc_iso15693_listener_init,
            .deinit = f_hal_nfc_iso15693_listener_deinit,
        },
};
