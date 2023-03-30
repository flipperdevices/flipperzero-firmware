#include "f_hal_nfc_i.h"

#include <lib/drivers/st25r3916.h>

#include <furi.h>
#include <furi_hal_spi.h>

#define TAG "FHalNfc"

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

FHalNfcError f_hal_nfc_init() {
    FHalNfcError error = FHalNfcErrorNone;
    f_hal_nfc_event_init();

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    do {
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

        furi_hal_spi_release(&furi_hal_spi_bus_handle_nfc);
    } while(false);

    f_hal_nfc_low_power_mode_start();

    return error;
}

FHalNfcError f_hal_nfc_low_power_mode_start() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    st25r3916_clear_reg_bits(
        handle,
        ST25R3916_REG_OP_CONTROL,
        (ST25R3916_REG_OP_CONTROL_en | ST25R3916_REG_OP_CONTROL_rx_en |
         ST25R3916_REG_OP_CONTROL_wu | ST25R3916_REG_OP_CONTROL_tx_en |
         ST25R3916_REG_OP_CONTROL_en_fd_mask));
    f_hal_nfc_deinit_gpio_isr();
    f_hal_nfc_timers_deinit();

    furi_hal_spi_release(handle);

    return error;
}

FHalNfcError f_hal_nfc_low_power_mode_stop() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

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
    furi_hal_spi_release(handle);

    return error;
}

static void f_hal_nfc_configure_poller_common(FuriHalSpiBusHandle* handle) {
    st25r3916_change_reg_bits(
        handle, ST25R3916_REG_MODE, ST25R3916_REG_MODE_tr_am, ST25R3916_REG_MODE_tr_am_am);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_TX_DRIVER,
        ST25R3916_REG_TX_DRIVER_am_mod_mask,
        ST25R3916_REG_TX_DRIVER_am_mod_12percent);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_AUX_MOD,
        (ST25R3916_REG_AUX_MOD_dis_reg_am | ST25R3916_REG_AUX_MOD_res_am),
        0x00);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_A, 0xff, 0x82);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_ANT_TUNE_B, 0xFF, 0x82);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_OVERSHOOT_CONF1, 0xff, 0x00);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_OVERSHOOT_CONF2, 0xff, 0x00);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_UNDERSHOOT_CONF1, 0xff, 0x00);
    st25r3916_change_reg_bits(handle, ST25R3916_REG_UNDERSHOOT_CONF2, 0xff, 0x00);
}

FHalNfcError f_hal_nfc_set_mode(FHalNfcMode mode, FHalNfcBitrate bitrate) {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    if(mode == FHalNfcModeNfcaPoller) {
        f_hal_nfc_configure_poller_common(handle);
        // Disable wake up
        st25r3916_clear_reg_bits(handle, ST25R3916_REG_OP_CONTROL, ST25R3916_REG_OP_CONTROL_wu);
        // Enable ISO14443A mode
        st25r3916_write_reg(handle, ST25R3916_REG_MODE, ST25R3916_REG_MODE_om_iso14443a);
        st25r3916_change_reg_bits(
            handle,
            ST25R3916_REG_AUX,
            ST25R3916_REG_AUX_dis_corr,
            ST25R3916_REG_AUX_dis_corr_correlator);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_OVERSHOOT_CONF1, 0xff, 0x40);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_OVERSHOOT_CONF2, 0xff, 0x03);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_UNDERSHOOT_CONF1, 0xff, 0x40);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_UNDERSHOOT_CONF2, 0xff, 0x03);
    } else if(mode == FHalNfcModeNfcaListener) {
        st25r3916_write_reg(
            handle,
            ST25R3916_REG_OP_CONTROL,
            ST25R3916_REG_OP_CONTROL_en | ST25R3916_REG_OP_CONTROL_rx_en |
                ST25R3916_REG_OP_CONTROL_en_fd_auto_efd);
        st25r3916_write_reg(
            handle,
            ST25R3916_REG_MODE,
            ST25R3916_REG_MODE_targ_targ | ST25R3916_REG_MODE_om3 | ST25R3916_REG_MODE_om0);
        st25r3916_write_reg(
            handle,
            ST25R3916_REG_PASSIVE_TARGET,
            ST25R3916_REG_PASSIVE_TARGET_fdel_2 | ST25R3916_REG_PASSIVE_TARGET_fdel_0 |
                ST25R3916_REG_PASSIVE_TARGET_d_ac_ap2p |
                ST25R3916_REG_PASSIVE_TARGET_d_212_424_1r);

        st25r3916_write_reg(handle, ST25R3916_REG_MASK_RX_TIMER, 0x02);
    }

    if(bitrate == FHalNfcBitrate106) {
        st25r3916_change_reg_bits(handle, ST25R3916_REG_RX_CONF1, 0xff, 0x08);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_RX_CONF2, 0xff, 0x2d);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_RX_CONF3, 0xff, 0x00);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_RX_CONF4, 0xff, 0x00);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_CORR_CONF1, 0xff, 0x51);
        st25r3916_change_reg_bits(handle, ST25R3916_REG_CORR_CONF2, 0xff, 0x00);
    }

    furi_hal_spi_release(handle);

    return error;
}

FHalNfcError f_hal_nfc_poller_field_on() {
    FHalNfcError error = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

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

    furi_hal_spi_release(handle);

    return error;
}

FHalNfcError f_hal_nfc_poller_tx(uint8_t* tx_data, uint16_t tx_bits) {
    furi_assert(tx_data);

    FHalNfcError err = FHalNfcErrorNone;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    // Prepare tx
    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_clear_reg_bits(
        handle, ST25R3916_REG_TIMER_EMV_CONTROL, ST25R3916_REG_TIMER_EMV_CONTROL_nrt_emv);
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
    furi_hal_spi_release(handle);
    return err;
}

FHalNfcError f_hal_nfc_poller_rx(uint8_t* rx_data, uint16_t rx_data_size, uint16_t* rx_bits) {
    furi_assert(rx_data);
    furi_assert(rx_bits);

    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    FHalNfcError error = FHalNfcErrorNone;

    if(!st25r3916_read_fifo(handle, rx_data, rx_data_size, rx_bits)) {
        error = FHalNfcErrorBufferOverflow;
    }

    furi_hal_spi_release(handle);

    return error;
}

FHalNfcError f_hal_nfc_trx_reset() {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);

    furi_hal_spi_release(handle);
    return FHalNfcErrorNone;
}

FHalNfcError f_hal_nfc_listen_start() {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    // f_hal_nfc_trx_reset();
    uint32_t interrupts =
        (/*ST25R3916_IRQ_MASK_FWL | ST25R3916_IRQ_MASK_TXE |*/ ST25R3916_IRQ_MASK_RXS /*|
         ST25R3916_IRQ_MASK_RXE | ST25R3916_IRQ_MASK_PAR | ST25R3916_IRQ_MASK_CRC |
         ST25R3916_IRQ_MASK_ERR1 | ST25R3916_IRQ_MASK_ERR2 | ST25R3916_IRQ_MASK_EON |
         ST25R3916_IRQ_MASK_EOF | ST25R3916_IRQ_MASK_WU_A_X | ST25R3916_IRQ_MASK_WU_A*/);
    // Clear interrupts
    // FURI_LOG_I("LISTEN START", "%lX", interrupts);
    st25r3916_get_irq(handle);
    // Enable interrupts
    st25r3916_mask_irq(handle, interrupts);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_GOTO_SENSE);
    // st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);

    furi_hal_spi_release(handle);
    return FHalNfcErrorNone;
}

FHalNfcError f_hal_nfc_listener_sleep() {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    st25r3916_direct_cmd(handle, ST25R3916_CMD_GOTO_SLEEP);

    furi_hal_spi_release(handle);
    return FHalNfcErrorNone;
}

void f_hal_nfc_set_mask_receive_timer(uint32_t time_fc) {
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    furi_hal_spi_acquire(handle);

    st25r3916_write_reg(handle, ST25R3916_REG_MASK_RX_TIMER, time_fc);

    furi_hal_spi_release(handle);
}
