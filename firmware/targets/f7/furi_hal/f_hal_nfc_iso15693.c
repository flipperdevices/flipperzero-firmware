#include "f_hal_nfc_i.h"

#include <pulse_reader/pulse_reader.h>
#include <digital_signal/presets/nfc/iso15693_signal.h>

#include <furi_hal_resources.h>

#define ISO15693_FC (13560000.0)
#define ISO15693_PULSE_DURATION_NS (128.0 * 1000000000.0 / ISO15693_FC)

#define FURI_HAL_ISO15693_BUFFER_SIZE (512U)

#define BITS_IN_BYTE (8U)

#define TAG "FuriHalIso15693"

typedef struct {
    uint8_t rx_buf[FURI_HAL_ISO15693_BUFFER_SIZE];
    size_t rx_bytes;
    uint32_t timestamp;
    PulseReader* reader;
    Iso15693Signal* signal;
} FHalNfcIso15693Listener;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener_alloc() {
    FHalNfcIso15693Listener* instance = malloc(sizeof(FHalNfcIso15693Listener));

    instance->reader = pulse_reader_alloc(&gpio_spi_r_miso, FURI_HAL_ISO15693_BUFFER_SIZE);
    pulse_reader_set_timebase(instance->reader, PulseReaderUnitNanosecond);
    pulse_reader_set_bittime(instance->reader, ISO15693_PULSE_DURATION_NS);
    pulse_reader_set_pull(instance->reader, GpioPullDown);

    instance->signal = iso15693_signal_alloc(&gpio_spi_r_mosi);

    return instance;
}

static void f_hal_nfc_iso15693_listener_free(FHalNfcIso15693Listener* instance) {
    furi_assert(instance);

    pulse_reader_free(instance->reader);
    iso15693_signal_free(instance->signal);

    free(instance);
}

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
    furi_assert(f_hal_nfc_iso15693_listener == NULL);

    f_hal_nfc_iso15693_listener = f_hal_nfc_iso15693_listener_alloc();

    // Set default operation mode
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_MODE,
        ST25R3916_REG_MODE_om_mask | ST25R3916_REG_MODE_tr_am,
        ST25R3916_REG_MODE_om_targ_nfca | ST25R3916_REG_MODE_tr_am_ook);

    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_OP_CONTROL,
        ST25R3916_REG_OP_CONTROL_rx_en,
        ST25R3916_REG_OP_CONTROL_rx_en);

    // Enable passive target mode
    st25r3916_change_reg_bits(
        handle, ST25R3916_REG_MODE, ST25R3916_REG_MODE_targ, ST25R3916_REG_MODE_targ_targ);

    return f_hal_nfc_iso15693_common_init(handle);
}

static FHalNfcError f_hal_nfc_iso15693_listener_deinit(FuriHalSpiBusHandle* handle) {
    UNUSED(handle);

    f_hal_nfc_iso15693_listener_free(f_hal_nfc_iso15693_listener);

    return FHalNfcErrorNone;
}

static void f_hal_nfc_iso15693_listener_transparent_mode_enter(FuriHalSpiBusHandle* handle) {
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSPARENT_MODE);

    furi_hal_spi_bus_handle_deinit(handle);
    f_hal_nfc_deinit_gpio_isr();

    furi_hal_gpio_init(&gpio_spi_r_miso, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
}

static void f_hal_nfc_iso15693_listener_transparent_mode_exit(FuriHalSpiBusHandle* handle) {
    // Configure gpio back to SPI and exit transparent mode
    f_hal_nfc_init_gpio_isr();
    furi_hal_spi_bus_handle_init(handle);

    st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);
}

static FHalNfcError
    f_hal_nfc_iso15693_listener_tx_transparent(const uint8_t* data, size_t data_size) {
    iso15693_signal_tx(
        f_hal_nfc_iso15693_listener->signal, Iso15693SignalDataRateHi, data, data_size);

    return FHalNfcErrorNone;
}

static FHalNfcError f_hal_nfc_iso15693_listener_rx_start(FuriHalSpiBusHandle* handle) {
    FHalNfcError error = FHalNfcErrorNone;

    f_hal_nfc_iso15693_listener_transparent_mode_enter(handle);

    error = f_hal_nfc_iso15693_listener_rx_transparent();

    f_hal_nfc_iso15693_listener_transparent_mode_exit(handle);

    if(error == FHalNfcErrorNone) {
        f_hal_nfc_event_set(FHalNfcEventInternalTypeTransparentRxEnd);
    }

    return error;
}

static FHalNfcError f_hal_nfc_iso15693_listener_tx(
    FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    furi_assert(f_hal_nfc_iso15693_listener);

    FHalNfcError error = FHalNfcErrorNone;
    f_hal_nfc_iso15693_listener_transparent_mode_enter(handle);

    error = f_hal_nfc_iso15693_listener_tx_transparent(tx_data, tx_bits / BITS_IN_BYTE);

    f_hal_nfc_iso15693_listener_transparent_mode_exit(handle);

    return error;
}

static FHalNfcError f_hal_nfc_iso15693_listener_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    furi_assert(f_hal_nfc_iso15693_listener);
    UNUSED(handle);

    const size_t rx_bytes_ready = f_hal_nfc_iso15693_listener->rx_bytes;
    const size_t rx_bits_ready = rx_bytes_ready * BITS_IN_BYTE;

    if(rx_bytes_ready > rx_data_size) {
        return FHalNfcErrorBufferOverflow;
    }

    memcpy(rx_data, f_hal_nfc_iso15693_listener->rx_buf, rx_bytes_ready);
    *rx_bits = rx_bits_ready;

    f_hal_nfc_iso15693_listener->rx_bytes = 0;
    // f_hal_nfc_event_set(FHalNfcEventInternalTypeTransparentFieldOn);

    return FHalNfcErrorNone;
}

const FHalNfcTechBase f_hal_nfc_iso15693 = {
    .poller =
        {
            .init = f_hal_nfc_iso15693_poller_init,
            .deinit = f_hal_nfc_iso15693_poller_deinit,
            .wait_event = f_hal_nfc_wait_event_common,
        },

    .listener =
        {
            .init = f_hal_nfc_iso15693_listener_init,
            .deinit = f_hal_nfc_iso15693_listener_deinit,
            .wait_event = f_hal_nfc_wait_event_common,
            .rx_start = f_hal_nfc_iso15693_listener_rx_start,
            .tx = f_hal_nfc_iso15693_listener_tx,
            .rx = f_hal_nfc_iso15693_listener_rx,
        },
};
