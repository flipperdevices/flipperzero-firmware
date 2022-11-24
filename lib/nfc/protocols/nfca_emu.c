#include <limits.h>
#include <furi.h>

#include <furi_hal.h>
#include <furi_hal_nfc.h>
#include <furi_hal_spi.h>
#include <furi_hal_gpio.h>
#include <furi_hal_cortex.h>
#include <furi_hal_resources.h>
#include <st25r3916.h>
#include <st25r3916_irq.h>

#include "nfca_emu.h"

#define TAG "NfcA-emu"




void nfca_emu_init(NfcaEmuState *state, FuriHalNfcDevData* nfc_data) {
    //nfca_emu_alloc();
    rfal_platform_spi_acquire();

    st25r3916ExecuteCommand(ST25R3916_CMD_STOP);
    st25r3916WriteRegister(ST25R3916_REG_OP_CONTROL, 0xC3);
    st25r3916WriteRegister(ST25R3916_REG_MODE, 0x88);
    st25r3916ExecuteCommand(ST25R3916_CMD_TRANSPARENT_MODE);

    furi_hal_spi_bus_handle_deinit(&furi_hal_spi_bus_handle_nfc);

    FURI_LOG_D(TAG, "Starting NfcA emulation");
    FURI_LOG_D(TAG, "  UID:          %02X %02X %02X %02X %02X %02X %02X %02X", 
        nfc_data->uid[0], nfc_data->uid[1], nfc_data->uid[2], nfc_data->uid[3], 
        nfc_data->uid[4], nfc_data->uid[5], nfc_data->uid[6], nfc_data->uid[7]);

    /* allocate a 512 edge buffer, more than enough */
    state->reader_signal = pulse_reader_alloc(&gpio_spi_r_miso, 512);
    /* timebase shall be 1 ns */
    pulse_reader_set_timebase(state->reader_signal, PulseReaderUnitNanosecond);
    
    pulse_reader_start(state->reader_signal);

    /* set start values */
    state->bits_received = 0;
    state->have_sof = false;
    state->valid_frame = false;
}

void nfca_emu_deinit(NfcaEmuState *state) {
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_nfc);
    rfal_platform_spi_release();
    pulse_reader_free(state->reader_signal);
}


void nfca_bit_received(NfcaEmuState *state, uint8_t bit) {
    
    /* According to ISO14443-3 short frames have 7 bits and standard 9 bits per byte,
       where the 9th bit is odd parity. Data is transmitted LSB first. */
    uint32_t byte_num = (state->bits_received / 9);
    uint32_t bit_num = (state->bits_received % 9);

    if(bit_num == 8) {
        uint32_t parity_value = 1 << (state->bits_received / 9);
        state->parity_bits &= ~parity_value;
        state->parity_bits |= bit ? parity_value : 0;
    } else {
        uint32_t bit_value = 1 << bit_num;
        state->frame_data[byte_num] &= ~bit_value;
        state->frame_data[byte_num] |= bit ? bit_value : 0;
    }

    state->bits_received++;
}


bool nfca_emu_loop(NfcaEmuState *state, FuriHalNfcDevData* nfc_data, uint32_t timeout_ms) {
    furi_assert(nfc_data);
    
    bool ret = false;

    while(!ret) {
        uint32_t timeout_ns = timeout_ms * 1000;
        uint32_t timeout = state->have_sof ? (4 * NFCA_TB) : timeout_ns;
        uint32_t nsec = pulse_reader_receive(state->reader_signal, timeout);


        bool eof = state->have_sof && nsec > 2*NFCA_TB;

        if(nsec != PULSE_READER_NO_EDGE) {
            FURI_LOG_T(TAG, "pulse: %lu ns, frametime now: %ld", nsec, state->frame_time);
        }

        if(IS_T1(nsec) || eof) {
            FURI_LOG_T(TAG, "  T1 pulse");

            if(!state->have_sof) {
                FURI_LOG_T(TAG, "    SOF");
                state->frame_time = -(NFCA_TB - nsec);
                state->have_sof = true;
                state->valid_frame = false;
                state->bits_received = 0;
                continue;
            }

            if(state->frame_time > NFCA_TB_MIN) {
                FURI_LOG_T(TAG, "    Y");
                state->frame_time -= NFCA_TB;
                nfca_bit_received(state, 0);
            }

            if(IS_ZERO(state->frame_time)) {
                FURI_LOG_T(TAG, "    Z");
                state->frame_time = -(NFCA_TB - nsec);
                nfca_bit_received(state, 0);
            } else if(IS_TX(state->frame_time)) {
                FURI_LOG_T(TAG, "    X");
                state->frame_time = -(NFCA_TX - nsec);
                nfca_bit_received(state, 1);
            } else {
                if(eof) {
                    FURI_LOG_T(TAG, "    EOF");
                    state->have_sof = false;
                    state->valid_frame = true;
                    ret = true;
                } else {
                    FURI_LOG_D(TAG, "    unexpected: %ld", state->frame_time);
                }
            }
            
            
        } else {
            if(!state->have_sof) {
                state->frame_time = 0;
            } else {
                state->frame_time += nsec;
                FURI_LOG_T(TAG, "  non-T1 pulse");
            }
        }
    }

    if(ret) {

        if(state->bits_received > 7) {
            /* a last 0-bit will look like a missing bit */
            if((state->bits_received % 9) == 8) {
                nfca_bit_received(state, 0);
                state->bits_received++;
            }
            FURI_LOG_D(TAG, "Received standard frame: %d bits (%d byte)", state->bits_received, state->bits_received/9);

            for(size_t pos = 0; pos < state->bits_received / 9; pos++) {
                FURI_LOG_D(TAG, " 0x%02X", state->frame_data[pos]);
            }
        } else {
            FURI_LOG_D(TAG, "Received short frame: %d bits", state->bits_received);
            FURI_LOG_D(TAG, " 0x%02X", state->frame_data[0] & 0x7F);
        }

        /* we know that this code uses TIM2, so stop pulse reader */
        //pulse_reader_stop(state->reader_signal);
        //nfcv_emu_handle_packet(nfc_data, nfcv_data, frame_payload, frame_pos);
        //pulse_reader_start(state->reader_signal);
    }

    return ret;
}
