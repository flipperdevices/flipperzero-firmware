#include "f_hal_nfc_i.h"

#include <digital_signal/presets/nfc/iso15693_signal.h>
#include <signal_reader/parsers/iso15693/iso15693_parser.h>

#include <furi_hal_resources.h>

#define F_HAL_NFC_ISO15693_MAX_FRAME_SIZE (1024U)
#define F_HAL_NFC_ISO15693_POLLER_MAX_BUFFER_SIZE (64)

#define F_HAL_NFC_ISO15693_RESP_SOF_SIZE (5)
#define F_HAL_NFC_ISO15693_RESP_EOF_SIZE (5)
#define F_HAL_NFC_ISO15693_RESP_SOF_MASK (0x1FU)
#define F_HAL_NFC_ISO15693_RESP_SOF_PATTERN (0x17U)
#define F_HAL_NFC_ISO15693_RESP_EOF_PATTERN (0x1DU)

#define F_HAL_NFC_ISO15693_RESP_PATTERN_MASK (0x03U)
#define F_HAL_NFC_ISO15693_RESP_PATTERN_0 (0x01U)
#define F_HAL_NFC_ISO15693_RESP_PATTERN_1 (0x02U)

#define BITS_IN_BYTE (8U)

#define TAG "FuriHalIso15693"

typedef struct {
    Iso15693Signal* signal;
    Iso15693Parser* parser;
} FHalNfcIso15693Listener;

typedef struct {
    // 4 bits per data bit on transmit
    uint8_t fifo_buf[F_HAL_NFC_ISO15693_POLLER_MAX_BUFFER_SIZE * 4];
    size_t fifo_buf_bits;
    uint8_t frame_buf[F_HAL_NFC_ISO15693_POLLER_MAX_BUFFER_SIZE * 2];
    size_t frame_buf_bits;
} FHalNfcIso15693Poller;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener = NULL;
static FHalNfcIso15693Poller* f_hal_nfc_iso15693_poller = NULL;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener_alloc() {
    FHalNfcIso15693Listener* instance = malloc(sizeof(FHalNfcIso15693Listener));

    instance->signal = iso15693_signal_alloc(&gpio_spi_r_mosi);
    instance->parser = iso15693_parser_alloc(&gpio_spi_r_miso, F_HAL_NFC_ISO15693_MAX_FRAME_SIZE);

    return instance;
}

static void f_hal_nfc_iso15693_listener_free(FHalNfcIso15693Listener* instance) {
    furi_assert(instance);

    iso15693_signal_free(instance->signal);
    iso15693_parser_free(instance->parser);

    free(instance);
}

static FHalNfcIso15693Poller* f_hal_nfc_iso15693_poller_alloc() {
    FHalNfcIso15693Poller* instance = malloc(sizeof(FHalNfcIso15693Poller));

    return instance;
}

static void f_hal_nfc_iso15693_poller_free(FHalNfcIso15693Poller* instance) {
    furi_assert(instance);

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
    furi_assert(f_hal_nfc_iso15693_poller == NULL);

    f_hal_nfc_iso15693_poller = f_hal_nfc_iso15693_poller_alloc();

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
    furi_assert(f_hal_nfc_iso15693_poller);

    f_hal_nfc_iso15693_poller_free(f_hal_nfc_iso15693_poller);
    f_hal_nfc_iso15693_poller = NULL;

    return FHalNfcErrorNone;
}

static void iso15693_3_poller_encode_frame(
    const uint8_t* tx_data,
    size_t tx_bits,
    uint8_t* frame_buf,
    size_t frame_buf_size,
    size_t* frame_buf_bits) {
    static const uint8_t bit_patterns_1_out_of_4[] = {0x02, 0x08, 0x20, 0x80};
    size_t frame_buf_size_calc = (tx_bits / 2) + 2;
    furi_assert(frame_buf_size >= frame_buf_size_calc);

    // Add SOF 1 out of 4
    frame_buf[0] = 0x21;

    size_t byte_pos = 1;
    for(size_t i = 0; i < tx_bits / BITS_IN_BYTE; ++i) {
        for(size_t j = 0; j < BITS_IN_BYTE; j += (BITS_IN_BYTE) / 4) {
            const uint8_t bit_pair = (tx_data[i] >> j) & 0x03;
            frame_buf[byte_pos++] = bit_patterns_1_out_of_4[bit_pair];
        }
    }
    // Add EOF
    frame_buf[byte_pos++] = 0x04;
    *frame_buf_bits = byte_pos * BITS_IN_BYTE;
}

static bool iso15693_3_poller_decode_frame(
    const uint8_t* buf,
    size_t buf_bits,
    uint8_t* buf_decoded,
    size_t buf_decoded_size,
    size_t* buf_decoded_bits) {
    bool decoded = false;
    size_t bit_pos = 0;
    memset(buf_decoded, 0, buf_decoded_size);

    do {
        if(buf_bits == 0) break;
        // Check SOF
        if((buf[0] & F_HAL_NFC_ISO15693_RESP_SOF_MASK) != F_HAL_NFC_ISO15693_RESP_SOF_PATTERN)
            break;

        // 2 response bits = 1 data bit
        for(uint32_t i = F_HAL_NFC_ISO15693_RESP_SOF_SIZE;
            i < buf_bits - F_HAL_NFC_ISO15693_RESP_SOF_SIZE;
            i += BITS_IN_BYTE / 4) {
            const size_t byte_index = i / BITS_IN_BYTE;
            const size_t bit_offset = i % BITS_IN_BYTE;
            const uint8_t resp_byte = (buf[byte_index] >> bit_offset) |
                                      (buf[byte_index + 1] << (BITS_IN_BYTE - bit_offset));

            // Check EOF
            if(resp_byte == F_HAL_NFC_ISO15693_RESP_EOF_PATTERN) {
                decoded = true;
                break;
            }

            const uint8_t bit_pattern = resp_byte & F_HAL_NFC_ISO15693_RESP_PATTERN_MASK;

            if(bit_pattern == F_HAL_NFC_ISO15693_RESP_PATTERN_0) {
                bit_pos++;
            } else if(bit_pattern == F_HAL_NFC_ISO15693_RESP_PATTERN_1) {
                buf_decoded[bit_pos / BITS_IN_BYTE] |= 1 << (bit_pos % BITS_IN_BYTE);
                bit_pos++;
            } else {
                break;
            }
            if(bit_pos / BITS_IN_BYTE > buf_decoded_size) {
                break;
            }
        }

    } while(false);

    if(decoded) {
        *buf_decoded_bits = bit_pos;
    }

    return decoded;
}

static FHalNfcError f_hal_nfc_iso15693_poller_tx(
    FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    FHalNfcIso15693Poller* instance = f_hal_nfc_iso15693_poller;
    iso15693_3_poller_encode_frame(
        tx_data,
        tx_bits,
        instance->frame_buf,
        sizeof(instance->frame_buf),
        &instance->frame_buf_bits);
    return f_hal_nfc_poller_tx_common(handle, instance->frame_buf, instance->frame_buf_bits);
}

static FHalNfcError f_hal_nfc_iso15693_poller_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    FHalNfcError error = FHalNfcErrorNone;
    FHalNfcIso15693Poller* instance = f_hal_nfc_iso15693_poller;

    do {
        error = f_hal_nfc_common_fifo_rx(
            handle, instance->fifo_buf, sizeof(instance->fifo_buf), &instance->fifo_buf_bits);
        if(error != FHalNfcErrorNone) break;

        if(!iso15693_3_poller_decode_frame(
               instance->fifo_buf,
               instance->fifo_buf_bits,
               instance->frame_buf,
               sizeof(instance->frame_buf),
               &instance->frame_buf_bits)) {
            error = FHalNfcErrorDataFormat;
            break;
        }
        if(rx_data_size < instance->frame_buf_bits / BITS_IN_BYTE) {
            error = FHalNfcErrorBufferOverflow;
            break;
        }

        memcpy(rx_data, instance->frame_buf, instance->frame_buf_bits / BITS_IN_BYTE);
        *rx_bits = instance->frame_buf_bits;
    } while(false);

    return error;
}

static void f_hal_nfc_iso15693_listener_transparent_mode_enter(FuriHalSpiBusHandle* handle) {
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSPARENT_MODE);

    furi_hal_spi_bus_handle_deinit(handle);
    f_hal_nfc_deinit_gpio_isr();
}

static void f_hal_nfc_iso15693_listener_transparent_mode_exit(FuriHalSpiBusHandle* handle) {
    // Configure gpio back to SPI and exit transparent mode
    f_hal_nfc_init_gpio_isr();
    furi_hal_spi_bus_handle_init(handle);

    st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);
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

    FHalNfcError error = f_hal_nfc_iso15693_common_init(handle);

    f_hal_nfc_iso15693_listener_transparent_mode_enter(handle);

    return error;
}

static FHalNfcError f_hal_nfc_iso15693_listener_deinit(FuriHalSpiBusHandle* handle) {
    furi_assert(f_hal_nfc_iso15693_listener);

    f_hal_nfc_iso15693_listener_transparent_mode_exit(handle);

    f_hal_nfc_iso15693_listener_free(f_hal_nfc_iso15693_listener);
    f_hal_nfc_iso15693_listener = NULL;

    return FHalNfcErrorNone;
}

static FHalNfcError
    f_hal_nfc_iso15693_listener_tx_transparent(const uint8_t* data, size_t data_size) {
    iso15693_signal_tx(
        f_hal_nfc_iso15693_listener->signal, Iso15693SignalDataRateHi, data, data_size);

    return FHalNfcErrorNone;
}

static void f_hal_nfc_iso15693_parser_callback(Iso15693ParserEvent event, void* context) {
    furi_assert(context);

    if(event == Iso15693ParserEventDataReceived) {
        FuriThreadId thread_id = context;
        furi_thread_flags_set(thread_id, FHalNfcEventInternalTypeTransparentDataReceived);
    }
}

static FHalNfcEvent f_hal_nfc_iso15693_wait_event(uint32_t timeout_ms) {
    FHalNfcEvent event = 0;

    FuriThreadId thread_id = furi_thread_get_current_id();
    iso15693_parser_start(
        f_hal_nfc_iso15693_listener->parser, f_hal_nfc_iso15693_parser_callback, thread_id);

    while(true) {
        uint32_t flag = furi_thread_flags_wait(
            FHalNfcEventInternalTypeAbort | FHalNfcEventInternalTypeTransparentDataReceived,
            FuriFlagWaitAny,
            timeout_ms);
        furi_thread_flags_clear(flag);

        if(flag & FHalNfcEventInternalTypeAbort) {
            event = FHalNfcEventAbortRequest;
            break;
        }
        if(flag & FHalNfcEventInternalTypeTransparentDataReceived) {
            if(iso15693_parser_run(f_hal_nfc_iso15693_listener->parser)) {
                event = FHalNfcEventRxEnd;
                break;
            }
        }
    }
    iso15693_parser_stop(f_hal_nfc_iso15693_listener->parser);

    return event;
}

static FHalNfcError f_hal_nfc_iso15693_listener_tx(
    FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    UNUSED(handle);
    furi_assert(f_hal_nfc_iso15693_listener);

    FHalNfcError error = FHalNfcErrorNone;

    error = f_hal_nfc_iso15693_listener_tx_transparent(tx_data, tx_bits / BITS_IN_BYTE);

    return error;
}

static FHalNfcError f_hal_nfc_iso15693_listener_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    furi_assert(f_hal_nfc_iso15693_listener);
    UNUSED(handle);

    if(rx_data_size < iso15693_parser_get_data_size_bytes(f_hal_nfc_iso15693_listener->parser)) {
        return FHalNfcErrorBufferOverflow;
    }

    iso15693_parser_get_data(f_hal_nfc_iso15693_listener->parser, rx_data, rx_data_size, rx_bits);

    return FHalNfcErrorNone;
}

FHalNfcError f_hal_iso15693_listener_sleep(FuriHalSpiBusHandle* handle) {
    UNUSED(handle);

    return FHalNfcErrorNone;
}

const FHalNfcTechBase f_hal_nfc_iso15693 = {
    .poller =
        {
            .init = f_hal_nfc_iso15693_poller_init,
            .deinit = f_hal_nfc_iso15693_poller_deinit,
            .wait_event = f_hal_nfc_wait_event_common,
            .tx = f_hal_nfc_iso15693_poller_tx,
            .rx = f_hal_nfc_iso15693_poller_rx,
        },

    .listener =
        {
            .init = f_hal_nfc_iso15693_listener_init,
            .deinit = f_hal_nfc_iso15693_listener_deinit,
            .wait_event = f_hal_nfc_iso15693_wait_event,
            .tx = f_hal_nfc_iso15693_listener_tx,
            .rx = f_hal_nfc_iso15693_listener_rx,
            .sleep = f_hal_iso15693_listener_sleep,
            .idle = f_hal_iso15693_listener_sleep,
        },
};
