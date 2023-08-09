#include "f_hal_nfc_i.h"

#include <pulse_reader/pulse_reader.h>
// #include <digital_signal/digital_signal.h>

#include <furi_hal_resources.h>

#define ISO15693_LOAD_MODULATION_POLARITY (false)

#define ISO15693_FC (13560000.0f)
#define ISO15693_RESP_SUBC1_PULSE_32 (1.0f / (ISO15693_FC / 32) / 2.0f)
#define ISO15693_RESP_SUBC1_UNMOD_256 (256.0f / ISO15693_FC)
#define ISO15693_PULSE_DURATION_NS (128.0f * 1000000000.0f / ISO15693_FC)

#define FURI_HAL_ISO15693_BUFFER_SIZE (512U)

#define BITS_IN_BYTE (8U)

#define TAG "FuriHalIso15693"

typedef enum {
    FuriHalIso15693FrameStateSof1,
    FuriHalIso15693FrameStateSof2,
    FuriHalIso15693FrameStateCoding4,
    FuriHalIso15693FrameStateCoding256,
    FuriHalIso15693FrameStateEof,
    FuriHalIso15693FrameStateReset,
} FuriHalIso15693FrameState;

typedef struct {
    uint8_t rx_buf[FURI_HAL_ISO15693_BUFFER_SIZE];
    size_t rx_bytes;
    uint32_t timestamp;
    PulseReader* reader;
} FHalNfcIso15693Listener;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener_alloc() {
    FHalNfcIso15693Listener* instance = malloc(sizeof(FHalNfcIso15693Listener));

    instance->reader = pulse_reader_alloc(&gpio_nfc_irq_rfid_pull, FURI_HAL_ISO15693_BUFFER_SIZE);
    pulse_reader_set_timebase(instance->reader, PulseReaderUnitNanosecond);
    pulse_reader_set_bittime(instance->reader, ISO15693_PULSE_DURATION_NS);
    pulse_reader_set_pull(instance->reader, GpioPullDown);

    return instance;
}

static void f_hal_nfc_iso15693_listener_free(FHalNfcIso15693Listener* instance) {
    furi_assert(instance);

    pulse_reader_free(instance->reader);
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

    return f_hal_nfc_iso15693_common_init(handle);
}

static FHalNfcError f_hal_nfc_iso15693_listener_deinit(FuriHalSpiBusHandle* handle) {
    UNUSED(handle);

    f_hal_nfc_iso15693_listener_free(f_hal_nfc_iso15693_listener);

    return FHalNfcErrorNone;
}

static void f_hal_nfc_iso15693_listener_transparent_mode_enter(FuriHalSpiBusHandle* handle) {
    /* stop operation to configure for transparent and passive mode */
    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    /* set enable, rx_enable and field detector enable */
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_OP_CONTROL,
        ST25R3916_REG_OP_CONTROL_en | ST25R3916_REG_OP_CONTROL_rx_en |
            ST25R3916_REG_OP_CONTROL_en_fd_auto_efd);
    /* explicitly set the modulation resistor in case system config changes for some reason */
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_PT_MOD,
        (0 << ST25R3916_REG_PT_MOD_ptm_res_shift) | (15 << ST25R3916_REG_PT_MOD_pt_res_shift));
    /* target mode: target, other fields do not have any effect as we use transparent mode */
    st25r3916_write_reg(handle, ST25R3916_REG_MODE, ST25R3916_REG_MODE_targ);
    /* let us modulate the field using MOSI, read ASK modulation using IRQ */
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSPARENT_MODE);

    furi_hal_spi_bus_handle_deinit(handle);
    f_hal_nfc_deinit_gpio_isr();

    //
    furi_hal_gpio_init(&gpio_spi_r_mosi, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(&gpio_spi_r_mosi, false);
}

static void f_hal_nfc_iso15693_listener_transparent_mode_exit(FuriHalSpiBusHandle* handle) {
    // Configure gpio back to SPI and exit transparent mode
    furi_hal_spi_bus_handle_init(handle);
    f_hal_nfc_init_gpio_isr();
    // st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);
    st25r3916_write_reg(handle, ST25R3916_REG_OP_CONTROL, 0x00);
    st25r3916_write_reg(handle, ST25R3916_REG_MODE, 0x08);
}

static FHalNfcError f_hal_nfc_iso15693_listener_rx_transparent() {
    uint32_t periods_previous = 0;
    uint32_t frame_pos = 0;
    uint32_t byte_value = 0;
    uint32_t bits_received = 0;
    bool wait_for_pulse = false;

    const uint32_t timeout = 1000000;

    FuriHalIso15693FrameState frame_state = FuriHalIso15693FrameStateSof1;
    FHalNfcError ret = FHalNfcErrorNone;

    pulse_reader_start(f_hal_nfc_iso15693_listener->reader);

    for(;;) {
        uint32_t periods = pulse_reader_receive(f_hal_nfc_iso15693_listener->reader, timeout);
        const uint32_t timestamp = DWT->CYCCNT;

        /* when timed out, reset to SOF state */
        if(periods == PULSE_READER_NO_EDGE || periods == PULSE_READER_LOST_EDGE) {
            ret = FHalNfcErrorCommunicationTimeout;
            break;
        }

        /* short helper for detecting a pulse position */
        if(wait_for_pulse) {
            wait_for_pulse = false;
            if(periods != 1) {
                frame_state = FuriHalIso15693FrameStateReset;
            }
            continue;
        }

        switch(frame_state) {
        case FuriHalIso15693FrameStateSof1:
            frame_state = (periods == 1) ? FuriHalIso15693FrameStateSof2 :
                                           FuriHalIso15693FrameStateSof1;
            break;

        case FuriHalIso15693FrameStateSof2:
            /* waiting for the second low period, telling us about coding */
            if(periods == 6) {
                frame_state = FuriHalIso15693FrameStateCoding256;
                periods_previous = 0;
                wait_for_pulse = true;
            } else if(periods == 4) {
                frame_state = FuriHalIso15693FrameStateCoding4;
                periods_previous = 2;
                wait_for_pulse = true;
            } else {
                frame_state = FuriHalIso15693FrameStateReset;
            }
            break;

        case FuriHalIso15693FrameStateCoding256:
            if(periods_previous > periods) {
                frame_state = FuriHalIso15693FrameStateReset;
                break;
            }

            /* previous symbol left us with some pulse periods */
            periods -= periods_previous;

            if(periods > 512) {
                frame_state = FuriHalIso15693FrameStateReset;
                break;
            } else if(periods == 2) {
                frame_state = FuriHalIso15693FrameStateEof;
                break;
            }

            periods_previous = 512 - (periods + 1);
            byte_value = (periods - 1) / 2;
            if(frame_pos < FURI_HAL_ISO15693_BUFFER_SIZE) {
                f_hal_nfc_iso15693_listener->rx_buf[frame_pos++] = (uint8_t)byte_value;
            }

            wait_for_pulse = true;
            break;

        case FuriHalIso15693FrameStateCoding4:
            if(periods_previous > periods) {
                frame_state = FuriHalIso15693FrameStateReset;
                break;
            }

            /* previous symbol left us with some pulse periods */
            periods -= periods_previous;
            periods_previous = 0;

            byte_value >>= 2;
            bits_received += 2;

            if(periods == 1) {
                byte_value |= 0x00 << 6; // -V684
                periods_previous = 6;
            } else if(periods == 3) {
                byte_value |= 0x01 << 6;
                periods_previous = 4;
            } else if(periods == 5) {
                byte_value |= 0x02 << 6;
                periods_previous = 2;
            } else if(periods == 7) {
                byte_value |= 0x03 << 6;
                periods_previous = 0;
            } else if(periods == 2) {
                frame_state = FuriHalIso15693FrameStateEof;
                break;
            } else {
                frame_state = FuriHalIso15693FrameStateReset;
                break;
            }

            if(bits_received >= BITS_IN_BYTE) {
                if(frame_pos < FURI_HAL_ISO15693_BUFFER_SIZE) {
                    f_hal_nfc_iso15693_listener->rx_buf[frame_pos++] = (uint8_t)byte_value;
                }
                bits_received = 0;
            }
            wait_for_pulse = true;
            break;

        default:
            break;
        }

        if(frame_state == FuriHalIso15693FrameStateReset) {
            frame_state = FuriHalIso15693FrameStateSof1;
        } else if(frame_state == FuriHalIso15693FrameStateEof) {
            f_hal_nfc_iso15693_listener->rx_bytes = frame_pos;
            f_hal_nfc_iso15693_listener->timestamp = timestamp;
            break;
        }
    }

    pulse_reader_stop(f_hal_nfc_iso15693_listener->reader);

    if(frame_state != FuriHalIso15693FrameStateEof) {
        ret = FHalNfcErrorCommunication;
    }

    return ret;
}

static FHalNfcError f_hal_nfc_iso15693_listener_rx_start(FuriHalSpiBusHandle* handle) {
    FHalNfcError error = FHalNfcErrorNone;

    f_hal_nfc_iso15693_listener_transparent_mode_enter(handle);

    error = f_hal_nfc_iso15693_listener_rx_transparent();

    f_hal_nfc_iso15693_listener_transparent_mode_exit(handle);

    if(error == FHalNfcErrorNone) {
        f_hal_nfc_event_set(FHalNfcEventInternalTypeTransparentRxEnd);
    } else {
        FURI_LOG_D(TAG, "Pulse Reader Error: %d", error);
    }

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

    FURI_LOG_D(TAG, "RX");

    // f_hal_nfc_event_set(FHalNfcEventInternalTypeTransparentFieldOn);

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
            .rx_start = f_hal_nfc_iso15693_listener_rx_start,
            .rx = f_hal_nfc_iso15693_listener_rx,
        },
};
