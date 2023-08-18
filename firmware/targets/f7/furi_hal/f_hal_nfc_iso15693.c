#include "f_hal_nfc_i.h"

#include <digital_signal/presets/nfc/iso15693_signal.h>
#include <signal_reader/signal_reader.h>
#include <nfc/helpers/bit_buffer.h>

#include <furi_hal_resources.h>

#define ISO15693_FC (13560000.0)
#define ISO15693_PULSE_DURATION_NS (128.0 * 1000000000.0 / ISO15693_FC)

#define FURI_HAL_ISO15693_BUFFER_SIZE (512U)

#define ISO15693_FRAME_PARSER_BITSTREAM_BUFF_SIZE (512)
#define ISO15693_FRAME_PARSER_BITRATE_F64MHZ (603U)

#define ISO15693_FRAME_PARSER_EVENT_FLAG_DATA_RECEIVED (1U << 16)
#define ISO15693_FRAME_PARSER_EVENT_FLAG_ALL (ISO15693_FRAME_PARSER_EVENT_FLAG_DATA_RECEIVED)

#define BITS_IN_BYTE (8U)

#define TAG "FuriHalIso15693"

typedef enum {
    Iso15693FrameParserStateWaitSOF,
    Iso15693FrameParserStateRx1OutOf4,
    Iso15693FrameParserStateRx1OutOf256,
} Iso15693FrameParserState;

typedef struct {
    bool sof_detected;
    uint8_t next_byte;
    uint8_t next_byte_part;
} Iso15693FrameParser1OutOf4Context;

typedef struct {
    uint8_t bit_offset;
    size_t bytes_processed;
    uint8_t zero_position;
} Iso15693FrameParser1OutOf256Context;

typedef struct {
    Iso15693FrameParserState state;

    SignalReader* signal_reader;
    FuriThreadId thread_id;

    uint8_t bitstream_buff[ISO15693_FRAME_PARSER_BITSTREAM_BUFF_SIZE / 2];
    size_t bitstream_idx;

    Iso15693FrameParser1OutOf4Context mode_1_out_of_4;
    Iso15693FrameParser1OutOf256Context mode_1_out_of_256;

    uint16_t next_pattern;
    size_t byte_idx;
    uint16_t last_byte;
    uint8_t bit_offset;

    BitBuffer* parsed_frame;
} Iso15693FrameParser;

typedef struct {
    Iso15693Signal* signal;
    Iso15693FrameParser* parser;
} FHalNfcIso15693Listener;

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener;

Iso15693FrameParser* iso15693_frame_parser_alloc(size_t max_frame_size) {
    Iso15693FrameParser* instance = malloc(sizeof(Iso15693FrameParser));
    instance->parsed_frame = bit_buffer_alloc(max_frame_size);

    instance->signal_reader =
        signal_reader_alloc(&gpio_spi_r_miso, ISO15693_FRAME_PARSER_BITSTREAM_BUFF_SIZE);
    signal_reader_set_sample_rate(
        instance->signal_reader, SignalReaderTimeUnit64Mhz, ISO15693_FRAME_PARSER_BITRATE_F64MHZ);
    signal_reader_set_pull(instance->signal_reader, GpioPullDown);

    return instance;
}

void iso15693_frame_parser_free(Iso15693FrameParser* instance) {
    furi_assert(instance);

    bit_buffer_free(instance->parsed_frame);
    signal_reader_free(instance->signal_reader);
    free(instance);
}

void iso15693_frame_parser_reset(Iso15693FrameParser* instance) {
    furi_assert(instance);

    instance->state = Iso15693FrameParserStateWaitSOF;
    memset(instance->bitstream_buff, 0, sizeof(instance->bitstream_buff));
    instance->bitstream_idx = 0;

    instance->mode_1_out_of_4.sof_detected = false;
    instance->mode_1_out_of_4.next_byte = 0;
    instance->mode_1_out_of_4.next_byte_part = 0;

    instance->mode_1_out_of_256.bytes_processed = 0;
    instance->mode_1_out_of_256.zero_position = 0;

    instance->bit_offset = 0;
    instance->last_byte = 0xFFFF;
    instance->next_pattern = 0x0000;
    instance->byte_idx = 0;

    bit_buffer_reset(instance->parsed_frame);
    // instance->thread_id = NULL;
}

static void signal_reader_callback(SignalReaderEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data->data);
    furi_assert(event.data->len == ISO15693_FRAME_PARSER_BITSTREAM_BUFF_SIZE / 2);

    Iso15693FrameParser* instance = context;
    memcpy(instance->bitstream_buff, event.data->data, event.data->len);
    furi_thread_flags_set(instance->thread_id, ISO15693_FRAME_PARSER_EVENT_FLAG_DATA_RECEIVED);
}

void iso15693_frame_parser_start(Iso15693FrameParser* instance) {
    furi_assert(instance);

    instance->thread_id = furi_thread_get_current_id();
    signal_reader_start(instance->signal_reader, signal_reader_callback, instance);
}

void iso15693_frame_parser_stop(Iso15693FrameParser* instance) {
    furi_assert(instance);

    signal_reader_stop(instance->signal_reader);
}

static bool iso15693_frame_parser_get_next_pattern(Iso15693FrameParser* instance) {
    bool next_pattern_extracted = false;

    if(instance->byte_idx >= COUNT_OF(instance->bitstream_buff)) {
        instance->byte_idx = 0;
    } else {
        instance->next_pattern = instance->last_byte |
                                 ((uint16_t)instance->bitstream_buff[instance->byte_idx] << 8);
        instance->last_byte = instance->bitstream_buff[instance->byte_idx];
        instance->byte_idx++;
        next_pattern_extracted = true;
    }

    return next_pattern_extracted;
}

static bool iso15693_frame_parser_check_pattern(
    Iso15693FrameParser* instance,
    bool invert_pattern,
    uint16_t mask) {
    bool ret = false;
    uint16_t mask_with_offset = mask << instance->bit_offset;

    if(invert_pattern) {
        ret = ((~instance->next_pattern) & mask_with_offset) == mask_with_offset;
    } else {
        ret = (instance->next_pattern & mask_with_offset) == mask_with_offset;
    }

    return ret;
}

bool iso15693_frame_parser_run(Iso15693FrameParser* instance) {
    bool frame_parsed = false;

    do {
        // Detect data
        if(instance->state == Iso15693FrameParserStateWaitSOF) {
            size_t i = 0;
            if(!instance->mode_1_out_of_4.sof_detected) {
                for(i = 0; i < COUNT_OF(instance->bitstream_buff); i++) {
                    if(instance->bitstream_buff[i] != 0xFF) {
                        break;
                    }
                }
                if(i == COUNT_OF(instance->bitstream_buff)) break;

                for(size_t j = 0; j < 8; j++) {
                    if(FURI_BIT(instance->bitstream_buff[i], j) == 0) {
                        instance->bit_offset = j;
                        break;
                    }
                }

                if((i == COUNT_OF(instance->bitstream_buff) - 1) && (instance->bit_offset != 0)) {
                    instance->mode_1_out_of_4.sof_detected = true;
                    instance->last_byte = instance->bitstream_buff[i];
                    printf("SOF starts at last byte\r\n");
                    break;
                }
            }

            uint16_t sof_candidate = 0;
            if(instance->mode_1_out_of_4.sof_detected) {
                sof_candidate = ((uint16_t)instance->bitstream_buff[i] << 8) | instance->last_byte;
                instance->last_byte = instance->bitstream_buff[i];
                instance->byte_idx = i + 1;
            } else {
                sof_candidate = ((uint16_t)instance->bitstream_buff[i + 1] << 8) |
                                instance->bitstream_buff[i];
                instance->last_byte = instance->bitstream_buff[i + 1];
                instance->byte_idx = i + 2;
            }
            const uint16_t sof_1_out_of_4 = 0x21;
            const uint16_t sof_1_out_of_256 = 0x81;

            if(((~sof_candidate) & (sof_1_out_of_4 << instance->bit_offset)) ==
               (sof_1_out_of_4 << instance->bit_offset)) {
                instance->state = Iso15693FrameParserStateRx1OutOf4;
                FURI_LOG_D(TAG, "SOF: %04X", (~sof_candidate) >> instance->bit_offset);
            } else if(
                ((~sof_candidate) & (sof_1_out_of_256 << instance->bit_offset)) ==
                (sof_1_out_of_256 << instance->bit_offset)) {
                instance->state = Iso15693FrameParserStateRx1OutOf256;
            } else {
                printf("SoF detection error\r\n");
                iso15693_frame_parser_reset(instance);
                break;
            }
        }

        if(instance->state == Iso15693FrameParserStateRx1OutOf4) {
            const uint16_t bit_patterns_1_out_of_4[] = {0x02, 0x08, 0x20, 0x80};
            const uint16_t eof = 0x04;
            while(true) {
                if(!iso15693_frame_parser_get_next_pattern(instance)) break;

                if(instance->mode_1_out_of_4.next_byte_part == 0) {
                    if(iso15693_frame_parser_check_pattern(instance, true, eof)) {
                        FURI_LOG_D(TAG, "EOF DETECTED");
                        frame_parsed = true;
                        break;
                    }
                }

                size_t j = 0;
                for(j = 0; j < COUNT_OF(bit_patterns_1_out_of_4); j++) {
                    if(iso15693_frame_parser_check_pattern(
                           instance, true, bit_patterns_1_out_of_4[j])) {
                        instance->mode_1_out_of_4.next_byte |=
                            j << (instance->mode_1_out_of_4.next_byte_part * 2);
                        instance->mode_1_out_of_4.next_byte_part++;
                        if(instance->mode_1_out_of_4.next_byte_part == 4) {
                            instance->mode_1_out_of_4.next_byte_part = 0;
                            FURI_LOG_D(TAG, "Byte %02X parsed", instance->mode_1_out_of_4.next_byte);
                            bit_buffer_append_byte(
                                instance->parsed_frame, instance->mode_1_out_of_4.next_byte);
                            instance->mode_1_out_of_4.next_byte = 0;
                        }
                        break;
                    }
                }
                if(j == COUNT_OF(bit_patterns_1_out_of_4)) {
                    FURI_LOG_W(TAG, "Rx error: unknown pattern: %d", instance->mode_1_out_of_4.next_byte_part);
                    iso15693_frame_parser_reset(instance);
                    break;
                }
            }
        }

        if(instance->state == Iso15693FrameParserStateRx1OutOf256) {
            const uint16_t eof = 0x04;
            while(true) {
                if(!iso15693_frame_parser_get_next_pattern(instance)) break;

                // Check eof
                if(instance->mode_1_out_of_256.bytes_processed == 0) {
                    if(iso15693_frame_parser_check_pattern(instance, true, eof)) {
                        frame_parsed = true;
                        break;
                    }
                }

                if(!iso15693_frame_parser_check_pattern(instance, false, 0xFF)) {
                    for(size_t j = 0; j < 8; j++) {
                        if(FURI_BIT(instance->next_pattern, instance->bit_offset + j) == 0) {
                            bit_buffer_append_byte(
                                instance->parsed_frame,
                                instance->mode_1_out_of_256.bytes_processed * 4 + j / 2);
                        }
                    }
                }
                instance->mode_1_out_of_256.bytes_processed =
                    (instance->mode_1_out_of_256.bytes_processed + 1) % 64;
            }
        }

    } while(false);

    return frame_parsed;
}

static FHalNfcIso15693Listener* f_hal_nfc_iso15693_listener_alloc() {
    FHalNfcIso15693Listener* instance = malloc(sizeof(FHalNfcIso15693Listener));

    instance->signal = iso15693_signal_alloc(&gpio_spi_r_mosi);
    instance->parser = iso15693_frame_parser_alloc(1024);

    return instance;
}

static void f_hal_nfc_iso15693_listener_free(FHalNfcIso15693Listener* instance) {
    furi_assert(instance);

    iso15693_signal_free(instance->signal);
    iso15693_frame_parser_free(instance->parser);

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
    UNUSED(handle);
    return FHalNfcErrorNone;
}

static FHalNfcEvent f_hal_nfc_iso15693_wait_event(uint32_t timeout_ms) {
    FHalNfcEvent event = 0;
    FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;

    f_hal_nfc_iso15693_listener_transparent_mode_enter(handle);
    iso15693_frame_parser_start(f_hal_nfc_iso15693_listener->parser);

    while(true) {
        uint32_t flag = furi_thread_flags_wait(
            ISO15693_FRAME_PARSER_EVENT_FLAG_ALL, FuriFlagWaitAny, timeout_ms);
        furi_thread_flags_clear(flag);

        if(flag & FHalNfcEventAbortRequest) {
            event = FHalNfcEventAbortRequest;
            break;
        }
        if(flag & ISO15693_FRAME_PARSER_EVENT_FLAG_DATA_RECEIVED) {
            if(iso15693_frame_parser_run(f_hal_nfc_iso15693_listener->parser)) {
                event = FHalNfcEventRxEnd;
                break;
            }
        }
    }

    iso15693_frame_parser_stop(f_hal_nfc_iso15693_listener->parser);
    f_hal_nfc_iso15693_listener_transparent_mode_exit(handle);

    return event;
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

    if(rx_data_size <
       bit_buffer_get_size_bytes(f_hal_nfc_iso15693_listener->parser->parsed_frame)) {
        return FHalNfcErrorBufferOverflow;
    }

    bit_buffer_write_bytes(
        f_hal_nfc_iso15693_listener->parser->parsed_frame, rx_data, rx_data_size);
    *rx_bits = bit_buffer_get_size(f_hal_nfc_iso15693_listener->parser->parsed_frame);
    iso15693_frame_parser_reset(f_hal_nfc_iso15693_listener->parser);

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
            .wait_event = f_hal_nfc_iso15693_wait_event,
            .rx_start = f_hal_nfc_iso15693_listener_rx_start,
            .tx = f_hal_nfc_iso15693_listener_tx,
            .rx = f_hal_nfc_iso15693_listener_rx,
        },
};
