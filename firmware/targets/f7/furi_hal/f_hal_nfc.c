#include "f_hal_nfc_i.h"

#include <lib/drivers/st25r3916.h>

#include <furi.h>
#include <furi_hal_spi.h>

#define TAG "FHalNfc"

static const FHalNfcTechBase* f_hal_nfc_tech[FHalNfcTechNum] = {
    [FHalNfcTechIso14443a] = &f_hal_nfc_iso14443a,
    [FHalNfcTechIso14443b] = &f_hal_nfc_iso14443b,
    [FHalNfcTechIso15693] = &f_hal_nfc_iso15693,
    [FHalNfcTechFelica] = &f_hal_nfc_felica,
};

FHalNfc f_hal_nfc;

static FHalNfcError f_hal_nfc_turn_on_osc(FuriHalSpiBusHandle* handle) {
    FHalNfcError error = FHalNfcErrorNone;

    if(!st25r3916_check_reg(
           handle,
           ST25R3916_REG_OP_CONTROL,
           ST25R3916_REG_OP_CONTROL_en,
           ST25R3916_REG_OP_CONTROL_en)) {
        st25r3916_mask_irq(handle, ~ST25R3916_IRQ_MASK_OSC);
        st25r3916_set_reg_bits(handle, ST25R3916_REG_OP_CONTROL, ST25R3916_REG_OP_CONTROL_en);
        f_hal_nfc_event_wait_for_specific_irq(handle, ST25R3916_IRQ_MASK_OSC, 10);
    }
    // Disable IRQs
    st25r3916_mask_irq(handle, ST25R3916_IRQ_MASK_ALL);

    bool osc_on = st25r3916_check_reg(
        handle,
        ST25R3916_REG_AUX_DISPLAY,
        ST25R3916_REG_AUX_DISPLAY_osc_ok,
        ST25R3916_REG_AUX_DISPLAY_osc_ok);
    if(!osc_on) {
        error = FHalNfcErrorOscillator;
    }

    return error;
}

FHalNfcError f_hal_nfc_is_hal_ready() {
    FHalNfcError error = FHalNfcErrorNone;

    do {
        error = f_hal_nfc_acquire();
        if(error != FHalNfcErrorNone) break;

        FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
        uint8_t chip_id = 0;
        st25r3916_read_reg(handle, ST25R3916_REG_IC_IDENTITY, &chip_id);
        if((chip_id & ST25R3916_REG_IC_IDENTITY_ic_type_mask) !=
           ST25R3916_REG_IC_IDENTITY_ic_type_st25r3916) {
            FURI_LOG_E(TAG, "Wrong chip id");
            error = FHalNfcErrorCommunication;
        }

        f_hal_nfc_release();
    } while(false);

    return error;
}

FHalNfcError f_hal_nfc_init() {
    furi_assert(f_hal_nfc.mutex == NULL);

    f_hal_nfc.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    FHalNfcError error = FHalNfcErrorNone;

    f_hal_nfc_event_init();
    f_hal_nfc_event_start();

    do {
        error = f_hal_nfc_acquire();
        if(error != FHalNfcErrorNone) break;

        FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
        // Set default state
        st25r3916_direct_cmd(handle, ST25R3916_CMD_SET_DEFAULT);
        // Increase IO driver strength of MISO and IRQ
        st25r3916_write_reg(handle, ST25R3916_REG_IO_CONF2, ST25R3916_REG_IO_CONF2_io_drv_lvl);
        // Check chip ID
        uint8_t chip_id = 0;
        st25r3916_read_reg(handle, ST25R3916_REG_IC_IDENTITY, &chip_id);
        if((chip_id & ST25R3916_REG_IC_IDENTITY_ic_type_mask) !=
           ST25R3916_REG_IC_IDENTITY_ic_type_st25r3916) {
            FURI_LOG_E(TAG, "Wrong chip id");
            error = FHalNfcErrorCommunication;
        }
        // Clear interrupts
        st25r3916_get_irq(handle);
        // Mask all interrupts
        st25r3916_mask_irq(handle, ST25R3916_IRQ_MASK_ALL);
        // Enable interrupts
        f_hal_nfc_init_gpio_isr();
        // Disable internal overheat protection
        st25r3916_change_test_reg_bits(handle, 0x04, 0x10, 0x10);

        error = f_hal_nfc_turn_on_osc(handle);
        if(error != FHalNfcErrorNone) break;

        // Measure voltage
        // Set measure power supply voltage source
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_REGULATOR_CONTROL,
            ST25R3916_REG_REGULATOR_CONTROL_mpsv_mask,
            ST25R3916_REG_REGULATOR_CONTROL_mpsv_vdd);
        // Enable timer and interrupt register
        st25r3916_mask_irq(handle, ~ST25R3916_IRQ_MASK_DCT);
        st25r3916_direct_cmd(handle, ST25R3916_CMD_MEASURE_VDD);
        f_hal_nfc_event_wait_for_specific_irq(handle, ST25R3916_IRQ_MASK_DCT, 100);
        st25r3916_mask_irq(handle, ST25R3916_IRQ_MASK_ALL);
        uint8_t ad_res = 0;
        st25r3916_read_reg(handle, ST25R3916_REG_AD_RESULT, &ad_res);
        uint16_t mV = ((uint16_t)ad_res) * 23U;
        mV += (((((uint16_t)ad_res) * 4U) + 5U) / 10U);

        if(mV < 3600) {
            st25r3916_change_reg_bits(
                handle,
                ST25R3916_REG_IO_CONF2,
                ST25R3916_REG_IO_CONF2_sup3V,
                ST25R3916_REG_IO_CONF2_sup3V_3V);
        } else {
            st25r3916_change_reg_bits(
                handle,
                ST25R3916_REG_IO_CONF2,
                ST25R3916_REG_IO_CONF2_sup3V,
                ST25R3916_REG_IO_CONF2_sup3V_5V);
        }

        // Disable MCU CLK
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_IO_CONF1,
            ST25R3916_REG_IO_CONF1_out_cl_mask | ST25R3916_REG_IO_CONF1_lf_clk_off,
            0x07);
        // Disable MISO pull-down
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_IO_CONF2,
            ST25R3916_REG_IO_CONF2_miso_pd1 | ST25R3916_REG_IO_CONF2_miso_pd2,
            0x00);
        // Set tx driver resistance to 1 Om
        st25r3916_change_reg_bits(
            handle, ST25R3916_REG_TX_DRIVER, ST25R3916_REG_TX_DRIVER_d_res_mask, 0x00);
        // Use minimum non-overlap
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_RES_AM_MOD,
            ST25R3916_REG_RES_AM_MOD_fa3_f,
            ST25R3916_REG_RES_AM_MOD_fa3_f);

        // Set activation threashold
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_FIELD_THRESHOLD_ACTV,
            ST25R3916_REG_FIELD_THRESHOLD_ACTV_trg_mask,
            ST25R3916_REG_FIELD_THRESHOLD_ACTV_trg_105mV);
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_FIELD_THRESHOLD_ACTV,
            ST25R3916_REG_FIELD_THRESHOLD_ACTV_rfe_mask,
            ST25R3916_REG_FIELD_THRESHOLD_ACTV_rfe_105mV);
        // Set deactivation threashold
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_FIELD_THRESHOLD_DEACTV,
            ST25R3916_REG_FIELD_THRESHOLD_DEACTV_trg_mask,
            ST25R3916_REG_FIELD_THRESHOLD_DEACTV_trg_75mV);
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_FIELD_THRESHOLD_DEACTV,
            ST25R3916_REG_FIELD_THRESHOLD_DEACTV_rfe_mask,
            ST25R3916_REG_FIELD_THRESHOLD_DEACTV_rfe_75mV);
        // Enable external load modulation
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_AUX_MOD,
            ST25R3916_REG_AUX_MOD_lm_ext,
            ST25R3916_REG_AUX_MOD_lm_ext);
        // Enable internal load modulation
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_AUX_MOD,
            ST25R3916_REG_AUX_MOD_lm_dri,
            ST25R3916_REG_AUX_MOD_lm_dri);
        // Adjust FDT
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_PASSIVE_TARGET,
            ST25R3916_REG_PASSIVE_TARGET_fdel_mask,
            (5U << ST25R3916_REG_PASSIVE_TARGET_fdel_shift));
        // Reduce RFO resistance in Modulated state
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_PT_MOD,
            ST25R3916_REG_PT_MOD_ptm_res_mask | ST25R3916_REG_PT_MOD_pt_res_mask,
            0x0f);
        // Enable RX start on first 4 bits
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_EMD_SUP_CONF,
            ST25R3916_REG_EMD_SUP_CONF_rx_start_emv,
            ST25R3916_REG_EMD_SUP_CONF_rx_start_emv_on);
        // Set antena tunning
        st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_A, 0xff, 0x82);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_B, 0xff, 0x82);
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_OP_CONTROL,
            ST25R3916_REG_OP_CONTROL_en_fd_mask,
            ST25R3916_REG_OP_CONTROL_en_fd_auto_efd);

        // Perform calibration
        if(st25r3916_check_reg(
               handle,
               ST25R3916_REG_REGULATOR_CONTROL,
               ST25R3916_REG_REGULATOR_CONTROL_reg_s,
               0x00)) {
            FURI_LOG_I(TAG, "Adjusting regulators");
            // Reset logic
            st25r3916_set_reg_bits(
                handle, ST25R3916_REG_REGULATOR_CONTROL, ST25R3916_REG_REGULATOR_CONTROL_reg_s);
            st25r3916_clear_reg_bits(
                handle, ST25R3916_REG_REGULATOR_CONTROL, ST25R3916_REG_REGULATOR_CONTROL_reg_s);
            st25r3916_direct_cmd(handle, ST25R3916_CMD_ADJUST_REGULATORS);
            furi_delay_ms(6);
        }

        f_hal_nfc_low_power_mode_start();
        f_hal_nfc_release();
    } while(false);

    return error;
}

static bool f_hal_nfc_is_mine() {
    return (furi_mutex_get_owner(f_hal_nfc.mutex) == furi_thread_get_current_id());
}

FHalNfcError f_hal_nfc_acquire() {
    furi_check(f_hal_nfc.mutex);

    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_nfc);

    FHalNfcError error = FHalNfcErrorNone;
    if(furi_mutex_acquire(f_hal_nfc.mutex, 100) != FuriStatusOk) {
        furi_hal_spi_release(&furi_hal_spi_bus_handle_nfc);
        error = FHalNfcErrorBusy;
    }

    return error;
}

FHalNfcError f_hal_nfc_release() {
    furi_check(f_hal_nfc.mutex);
    furi_check(f_hal_nfc_is_mine());
    furi_check(furi_mutex_release(f_hal_nfc.mutex) == FuriStatusOk);

    furi_hal_spi_release(&furi_hal_spi_bus_handle_nfc);

    return FHalNfcErrorNone;
}

FHalNfcError f_hal_nfc_low_power_mode_start() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    st25r3916_clear_reg_bits(
        handle,
        ST25R3916_REG_OP_CONTROL,
        (ST25R3916_REG_OP_CONTROL_en | ST25R3916_REG_OP_CONTROL_rx_en |
         ST25R3916_REG_OP_CONTROL_wu | ST25R3916_REG_OP_CONTROL_tx_en |
         ST25R3916_REG_OP_CONTROL_en_fd_mask));
    f_hal_nfc_deinit_gpio_isr();
    f_hal_nfc_timers_deinit();

    return error;
}

FHalNfcError f_hal_nfc_low_power_mode_stop() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    do {
        f_hal_nfc_init_gpio_isr();
        f_hal_nfc_timers_init();
        error = f_hal_nfc_turn_on_osc(handle);
        if(error != FHalNfcErrorNone) break;
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_OP_CONTROL,
            ST25R3916_REG_OP_CONTROL_en_fd_mask,
            ST25R3916_REG_OP_CONTROL_en_fd_auto_efd);

    } while(false);

    return error;
}

static FHalNfcError f_hal_nfc_poller_init_common(FuriHalSpiBusHandle* handle) {
    // Disable wake up
    st25r3916_clear_reg_bits(handle, ST25R3916_REG_OP_CONTROL, ST25R3916_REG_OP_CONTROL_wu);
    // Enable correlator
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_AUX,
        ST25R3916_REG_AUX_dis_corr,
        ST25R3916_REG_AUX_dis_corr_correlator);

    st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_A, 0xff, 0x82);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_B, 0xFF, 0x82);

    st25r3916_write_reg(handle, ST25R3916_REG_OVERSHOOT_CONF1, 0x00);
    st25r3916_write_reg(handle, ST25R3916_REG_OVERSHOOT_CONF2, 0x00);
    st25r3916_write_reg(handle, ST25R3916_REG_UNDERSHOOT_CONF1, 0x00);
    st25r3916_write_reg(handle, ST25R3916_REG_UNDERSHOOT_CONF2, 0x00);

    return FHalNfcErrorNone;
}

static FHalNfcError f_hal_nfc_listener_init_common(FuriHalSpiBusHandle* handle) {
    UNUSED(handle);
    // TODO: Common listener configuration
    return FHalNfcErrorNone;
}

FHalNfcError f_hal_nfc_set_mode(FHalNfcMode mode, FHalNfcTech tech) {
    furi_assert(mode < FHalNfcModeNum);
    furi_assert(tech < FHalNfcTechNum);

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    FHalNfcError error = FHalNfcErrorNone;

    if(mode == FHalNfcModePoller) {
        do {
            error = f_hal_nfc_poller_init_common(handle);
            if(error != FHalNfcErrorNone) break;
            error = f_hal_nfc_tech[tech]->poller.init(handle);
        } while(false);

    } else if(mode == FHalNfcModeListener) {
        do {
            error = f_hal_nfc_listener_init_common(handle);
            if(error != FHalNfcErrorNone) break;
            error = f_hal_nfc_tech[tech]->listener.init(handle);
        } while(false);
    }

    f_hal_nfc.mode = mode;
    f_hal_nfc.tech = tech;
    return error;
}

FHalNfcError f_hal_nfc_reset_mode() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);

    const FHalNfcMode mode = f_hal_nfc.mode;
    const FHalNfcTech tech = f_hal_nfc.tech;
    if(mode == FHalNfcModePoller) {
        error = f_hal_nfc_tech[tech]->poller.deinit(handle);
    } else if(mode == FHalNfcModeListener) {
        error = f_hal_nfc_tech[tech]->listener.deinit(handle);
    }
    // Set default value in mode register
    st25r3916_write_reg(handle, ST25R3916_REG_MODE, ST25R3916_REG_MODE_om0);
    st25r3916_write_reg(handle, ST25R3916_REG_STREAM_MODE, 0);
    st25r3916_clear_reg_bits(handle, ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx);
    st25r3916_clear_reg_bits(
        handle,
        ST25R3916_REG_BIT_RATE,
        ST25R3916_REG_BIT_RATE_txrate_mask | ST25R3916_REG_BIT_RATE_rxrate_mask);

    // Write default values
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF1, 0);
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_RX_CONF2,
        ST25R3916_REG_RX_CONF2_sqm_dyn | ST25R3916_REG_RX_CONF2_agc_en |
            ST25R3916_REG_RX_CONF2_agc_m);

    st25r3916_write_reg(
        handle,
        ST25R3916_REG_CORR_CONF1,
        ST25R3916_REG_CORR_CONF1_corr_s7 | ST25R3916_REG_CORR_CONF1_corr_s4 |
            ST25R3916_REG_CORR_CONF1_corr_s1 | ST25R3916_REG_CORR_CONF1_corr_s0);
    st25r3916_write_reg(handle, ST25R3916_REG_CORR_CONF2, 0);

    return error;
}

FHalNfcError f_hal_nfc_poller_field_on() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    if(!st25r3916_check_reg(
           handle,
           ST25R3916_REG_OP_CONTROL,
           ST25R3916_REG_OP_CONTROL_tx_en,
           ST25R3916_REG_OP_CONTROL_tx_en)) {
        // Set min guard time
        st25r3916_write_reg(handle, ST25R3916_REG_FIELD_ON_GT, 0);
        // st25r3916_direct_cmd(handle, ST25R3916_CMD_INITIAL_RF_COLLISION);
        // Enable tx rx
        st25r3916_set_reg_bits(
            handle,
            ST25R3916_REG_OP_CONTROL,
            (ST25R3916_REG_OP_CONTROL_rx_en | ST25R3916_REG_OP_CONTROL_tx_en));
    }

    return error;
}

FHalNfcError f_hal_iso14443_3a_poller_tx_custom_parity(const uint8_t* tx_data, size_t tx_bits) {
    furi_assert(tx_data);

    // TODO common code for f_hal_nfc_poller_tx

    FHalNfcError err = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    // Prepare tx
    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_clear_reg_bits(
        handle, ST25R3916_REG_TIMER_EMV_CONTROL, ST25R3916_REG_TIMER_EMV_CONTROL_nrt_emv);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_ISO14443A_NFC,
        (ST25R3916_REG_ISO14443A_NFC_no_tx_par | ST25R3916_REG_ISO14443A_NFC_no_rx_par),
        (ST25R3916_REG_ISO14443A_NFC_no_tx_par | ST25R3916_REG_ISO14443A_NFC_no_rx_par));
    uint32_t interrupts =
        (ST25R3916_IRQ_MASK_FWL | ST25R3916_IRQ_MASK_TXE | ST25R3916_IRQ_MASK_RXS |
         ST25R3916_IRQ_MASK_RXE | ST25R3916_IRQ_MASK_PAR | ST25R3916_IRQ_MASK_CRC |
         ST25R3916_IRQ_MASK_ERR1 | ST25R3916_IRQ_MASK_ERR2 | ST25R3916_IRQ_MASK_NRE);
    // Clear interrupts
    st25r3916_get_irq(handle);
    // Enable interrupts
    st25r3916_mask_irq(handle, ~interrupts);

    st25r3916_write_fifo(handle, tx_data, tx_bits);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);
    return err;
}

FHalNfcError f_hal_nfc_poller_tx_common(
    FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    furi_assert(tx_data);

    FHalNfcError err = FHalNfcErrorNone;

    // Prepare tx
    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_clear_reg_bits(
        handle, ST25R3916_REG_TIMER_EMV_CONTROL, ST25R3916_REG_TIMER_EMV_CONTROL_nrt_emv);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_ISO14443A_NFC,
        (ST25R3916_REG_ISO14443A_NFC_no_tx_par | ST25R3916_REG_ISO14443A_NFC_no_rx_par),
        (ST25R3916_REG_ISO14443A_NFC_no_tx_par_off | ST25R3916_REG_ISO14443A_NFC_no_rx_par_off));
    uint32_t interrupts =
        (ST25R3916_IRQ_MASK_FWL | ST25R3916_IRQ_MASK_TXE | ST25R3916_IRQ_MASK_RXS |
         ST25R3916_IRQ_MASK_RXE | ST25R3916_IRQ_MASK_PAR | ST25R3916_IRQ_MASK_CRC |
         ST25R3916_IRQ_MASK_ERR1 | ST25R3916_IRQ_MASK_ERR2 | ST25R3916_IRQ_MASK_NRE);
    // Clear interrupts
    st25r3916_get_irq(handle);
    // Enable interrupts
    st25r3916_mask_irq(handle, ~interrupts);

    st25r3916_write_fifo(handle, tx_data, tx_bits);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);

    return err;
}

FHalNfcError
    f_hal_nfc_common_fifo_tx(FuriHalSpiBusHandle* handle, const uint8_t* tx_data, size_t tx_bits) {
    FHalNfcError err = FHalNfcErrorNone;

    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_write_fifo(handle, tx_data, tx_bits);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);

    return err;
}

FHalNfcError f_hal_nfc_poller_tx(const uint8_t* tx_data, size_t tx_bits) {
    furi_assert(f_hal_nfc.mode == FHalNfcModePoller);
    furi_assert(f_hal_nfc.tech < FHalNfcTechNum);
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    return f_hal_nfc_tech[f_hal_nfc.tech]->poller.tx(handle, tx_data, tx_bits);
}

FHalNfcError f_hal_nfc_poller_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits) {
    furi_assert(f_hal_nfc.mode == FHalNfcModePoller);
    furi_assert(f_hal_nfc.tech < FHalNfcTechNum);
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    return f_hal_nfc_tech[f_hal_nfc.tech]->poller.rx(handle, rx_data, rx_data_size, rx_bits);
}

FHalNfcEvent f_hal_nfc_poller_wait_event(uint32_t timeout_ms) {
    furi_assert(f_hal_nfc.mode == FHalNfcModePoller);
    furi_assert(f_hal_nfc.tech < FHalNfcTechNum);

    return f_hal_nfc_tech[f_hal_nfc.tech]->poller.wait_event(timeout_ms);
}

FHalNfcEvent f_hal_nfc_listener_wait_event(uint32_t timeout_ms) {
    furi_assert(f_hal_nfc.mode == FHalNfcModeListener);
    furi_assert(f_hal_nfc.tech < FHalNfcTechNum);

    return f_hal_nfc_tech[f_hal_nfc.tech]->listener.wait_event(timeout_ms);
}

FHalNfcError f_hal_nfc_listener_tx(const uint8_t* tx_data, size_t tx_bits) {
    furi_assert(tx_data);

    furi_assert(f_hal_nfc.mode == FHalNfcModeListener);
    furi_assert(f_hal_nfc.tech < FHalNfcTechNum);

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    return f_hal_nfc_tech[f_hal_nfc.tech]->listener.tx(handle, tx_data, tx_bits);
}

FHalNfcError f_hal_nfc_common_fifo_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    FHalNfcError error = FHalNfcErrorNone;

    if(!st25r3916_read_fifo(handle, rx_data, rx_data_size, rx_bits)) {
        error = FHalNfcErrorBufferOverflow;
    }

    return error;
}

FHalNfcError f_hal_nfc_listener_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits) {
    furi_assert(rx_data);
    furi_assert(rx_bits);

    furi_assert(f_hal_nfc.mode == FHalNfcModeListener);
    furi_assert(f_hal_nfc.tech < FHalNfcTechNum);

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    return f_hal_nfc_tech[f_hal_nfc.tech]->listener.rx(handle, rx_data, rx_data_size, rx_bits);
}

FHalNfcError f_hal_nfc_trx_reset() {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);

    return FHalNfcErrorNone;
}

// TODO make virtual
FHalNfcError f_hal_nfc_listener_sleep() {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    // Enable auto collision resolution
    st25r3916_clear_reg_bits(
        handle, ST25R3916_REG_PASSIVE_TARGET, ST25R3916_REG_PASSIVE_TARGET_d_106_ac_a);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_GOTO_SLEEP);

    return FHalNfcErrorNone;
}

FHalNfcError f_hal_nfc_listener_disable_auto_col_res() {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    st25r3916_set_reg_bits(
        handle, ST25R3916_REG_PASSIVE_TARGET, ST25R3916_REG_PASSIVE_TARGET_d_106_ac_a);

    return FHalNfcErrorNone;
}
