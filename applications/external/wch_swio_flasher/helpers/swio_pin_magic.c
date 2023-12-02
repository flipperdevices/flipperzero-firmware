/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "swio.h"
#include "swio_inner.h"
#include "../utils.h"
#include "../config.h"

// Ugly code with some ASM timing magic

#define TAG "WCH_SWIO_FL"

// not my code, thx john-bollinger@stackoverflow ...
static uint8_t countBit1Fast(uint32_t n) {
    n = (n & 0x55555555u) + ((n >> 1) & 0x55555555u);
    n = (n & 0x33333333u) + ((n >> 2) & 0x33333333u);
    n = (n & 0x0f0f0f0fu) + ((n >> 4) & 0x0f0f0f0fu);
    n = (n & 0x00ff00ffu) + ((n >> 8) & 0x00ff00ffu);
    n = (n & 0x0000ffffu) + ((n >> 16) & 0x0000ffffu);
    return n;
}

uint8_t calcParity(uint8_t address, WchSwioFlasher_SWIO_RW rw, uint32_t data) {
    uint8_t cnt = 1; // header

    cnt += countBit1Fast(address & 0x7F);
    cnt += rw & 0x01;
    cnt += countBit1Fast(data);

    return cnt & 0x01;
}
#pragma GCC push_options
#pragma GCC optimize("O0")
WchSwioFlasher_Error
    WchSwioFlasher_SWIO_rxtx(uint8_t address, WchSwioFlasher_SWIO_RW rw, uint32_t* data) {
    //asm volatile("bkpt  0x00");

    uint32_t header = (address << 1) | (rw & 0x01);
    uint32_t result = (uint32_t)-1;
    uint32_t txrx_data = *data;

    if(rw == WchSwioFlasher_SWIO_Write) {
        uint8_t parity = calcParity(address, rw, txrx_data);
#ifdef SWIO_TXRX_DEBUG_MSG_ENABLE
        FURI_LOG_D(TAG, "TX to %02X (parity %d): " FMT_4HEX, address, parity, _UI(*data));
#endif

        if(parity) {
            header |= 0x80000000;
        }
    }

    FURI_CRITICAL_ENTER();

#ifdef SWIO_TRIGGER_OUT_ENABLE
    furi_hal_gpio_write(&SWI_TRIG_GPIO, false);
#endif
    // -----------------------------------
    // r2 - data buffer (store header, tx and rx data)
    // r3 - timeout counter (rx only)
    // r4 - tmp register (eg.: load & store to IO)
    // r5 - io port base
    // r6 - store MODER state (input/output settings)
    // r8 - bit counter
    asm volatile(
        "nop \n" // for better visibility of this code in dissasembly :)

        "MOV r2, %[header]\n" // prepare header
        "AND %[result],%[result], #0x80000000 \n" // prepare result (without obstructing tx parity - gcc share register for header and result)

        "movw     r5, :lower16:%[port_base] \n" // store port base
        "movt     r5, :upper16:%[port_base] \n"

        "LDR      r6, [r5,%[moder_offset]] \n" // load current MODER
        "ORR      r6, r6, %[mode_out_mask] \n" // prepare MODER to IO pin as output
        "STR      r6, [r5,%[moder_offset]] \n" // set IO pin as output

        // ===========================================================
        // --- start bit ---

        "write_sb: \n" // write start bit procedure

        // drive IO pin to 'log0'
        "MOV      r4, %[reset_mask] \n"
        "STR      r4, [r5, %[bssr_offset]] \n" // set pin to low

        "BL delay_sb_l \n"
        "MOV      r4, %[set_mask] \n"
        "STR      r4, [r5, %[bssr_offset]] \n" // set pin to high
        "BL delay_sb_h \n"

        // --- start bit completed ---
        // ===========================================================
        // --- write address and r/w bit start

        "MOV      r8, #7\n" // setup counter

        "tx_hdr_loop: \n"

        "MOV      r4, %[reset_mask] \n"
        "STR      r4, [r5, %[bssr_offset]] \n" // set pin to low
        //"BL delay_ad_l \n" // wait in line low level
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"

        "TST      r2, #0x80 \n" // Test bit0 for value
        "BNE      write_hdr_high\n"

        "BL delay \n" // wait log0
        "BL delay \n"

        "write_hdr_high: \n"
        "MOV      r4, %[set_mask] \n"
        "STR      r4, [r5, %[bssr_offset]] \n" // set pin to high

        "NOP \n" // delay high

        "NOP \n"
        "NOP \n"

        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"

        //"BL delay_ad_h \n" // delay high

        "LSL      r2, r2, #1\n" // shift data
        "SUBS     r8, r8, #1 \n" // decrement counter
        "BPL      tx_hdr_loop \n" // loop if counter != 0

        // --- write address and r/w bit completed
        // ===========================================================
        // --- determine read/write

        "TST      r2, #0x100 \n" // Test if transfer is write
        "BNE      tx_data \n"
        "B        rx_data \n"

        // --- determine read/write completed
        // ===========================================================
        // --- write data

        "tx_data: \n"
        "MOV      r2, %[data] \n" // load data
        "MOV      r8, #32 \n" // setup counter

        "tx_loop: \n"

        "MOV      r4, %[reset_mask] \n"
        "STR      r4, [r5,%[bssr_offset]] \n" // set pin to low

        // magic delay
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"

        "TST      r2, #0x80000000 \n" // Test bit0 for value
        "BNE      write_high\n"

        // magic delay
        "BL delay \n" // wait log0
        "BL delay \n"

        "write_high: \n"
        "MOV      r4, %[set_mask] \n"
        "STR      r4, [r5,%[bssr_offset]] \n" // set pin to high

        // magic delay
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"

        // move to next tx bit
        "LSL      r2, r2, #1\n" // shift data

        // handle parity bit
        "CMP      r8, #0x01 \n" // test if next bit will be parity
        "BNE      tx_loop_dec_cnt\n" // branch if next bit != parity bit
        "AND      r2, %[header], 0x80000000 \n" // load parity bit

        "tx_loop_dec_cnt: \n"
        "SUBS     r8, r8, #1 \n" // decrement counter
        "BPL      tx_loop\n" // loop if counter != 0

        "B exit \n" // End transder

        // --- write data completed
        // ===========================================================
        // --- read data

        "rx_data: \n"

        // prepare counter and data regiter
        "MOV      r8, #32\n" // setup bit counter
        "MOV      r2, #0 \n" // Set empty value to data register

        // bit rx main loop
        "rx_loop: \n"
        "LSL      r2, r2, #1\n" // shift data

        // drive IO output with log0 for start rx
        "ORR      r6, r6, %[mode_out_mask] \n" // pin output mask
        "STR      r6, [r5,%[moder_offset]] \n" // set IO as output
        "MOV      r4, %[reset_mask] \n" // set to log0 IO mask
        "STR      r4, [r5,%[bssr_offset]] \n" // set IO to log0

        // magic log0 delay
        "NOP \n"
        "NOP \n"
        "NOP \n"
        //"NOP \n"

        // set-up delay counter
        "MOV      r3, #0xffff \n" // Reset timeout  TODO: proper value

        // start listening
        "AND      r6, r6, %[mode_in_mask] \n" // pin input mask
        "STR      r6, [r5,%[moder_offset]] \n" // set IO as input
        "MOV      r4, %[set_mask] \n" // set to log1 IO mask
        "STR      r4, [r5,%[bssr_offset]] \n" // set IO to log1

        // magic delay because of capacity of wires ...
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"
        "NOP \n"

        // Test RX pin if is log0 or log1 there
        "LDR      r4, [r5,%[idr_offset]] \n" // read pin
        "TST      r4, %[set_mask] \n" // Test pin state
        "BEQ      rx_wait_for_edge\n" // pin is in log0, skip store 'one'

        // Store bit
        "ORR       r2,r2, #1 \n" // set 'one' to data reg
        "B rx_loop_continue \n" // in case of log1, skip 'rx_wait_for_edge' (we have't time for it)

        // 'zero' bit detected, we need to wait for rising edge of signal for continue
        "rx_wait_for_edge: \n"
        "SUBS     r3, r3, #1 \n" // decrement timeout counter
        "BEQ      exit_err_reset\n" // error when timeout counter == 0
        "LDR      r4, [r5,%[idr_offset]] \n" // read IO pin
        "TST      r4, %[set_mask] \n" // Test IO pin state
        "BEQ      rx_wait_for_edge\n" // loop if IO pin != log1

        // after rising edge, continue with rx
        "rx_loop_continue: \n"

        "CMP      r8, #0x01 \n" // test if next bit will be parity
        "BNE      rx_loop_dec_cnt\n" // branch if next bit != parity bit
        "MOV      %[data], r2 \n" // store data

        "rx_loop_dec_cnt: \n"
        "SUBS     r8, r8, #1 \n" // decrement counter
        "BPL      rx_loop \n" // loop if counter != 0

        // rx done, store parity
        "AND      %[result], r2, #0x01 \n" // store parity to result (bit0)

        // --- read data completed
        // ===========================================================
        // --- read parity

        "B exit \n"

        // --- read data completed
        // ===========================================================
        // --- misc utils

        "delay: \n"
        "nop \n"
        "nop \n"

        "nop \n"
        "nop \n"
        "nop \n"
        "nop \n"
        "nop \n"
        "nop \n"
        "nop \n"

        "nop \n"
        "nop \n"
        "nop \n"

        "nop \n"
        "nop \n"
        "delay_sb_h: \n"
        "nop \n"
        "nop \n"

        "delay_sb_l: \n"
        "delay_rx_l: \n"
        "nop \n"
        "delay_ad_l: \n"
        "delay_tx_l: \n"
        "BX LR \n"

        // --- misc utils end
        // ===========================================================
        // --- error handlers

        "exit_err_reset: \n"
        "MOV %[result],0x1000 \n"

        // --- error handlers end
        // ===========================================================
        // --- exit ...

        "exit: \n"
        : [result] "=r"(result), [data] "=r"(txrx_data)
        : [header] "r"(header),
          "1"(txrx_data),
          [port_base] "i"(SWI_PORT_BASE),
          [bssr_offset] "i"(SWI_PORT_BSSR_OFFSET),
          [idr_offset] "i"(SWI_PORT_IDR_OFFSET),
          [moder_offset] "i"(SWI_PORT_MODER_OFFSET),
          [set_mask] "i"(SWI_SET_MASK),
          [reset_mask] "i"(SWI_RESET_MASK),
          [mode_out_mask] "i"(SWI_SET_MODE_OUTPUT_MASK),
          [mode_in_mask] "i"(SWI_SET_MODE_INPUT_MASK)

        : "r2", "r3", "r4", "r5", "r6", "r8");

#ifdef SWIO_TRIGGER_OUT_ENABLE
    furi_hal_gpio_write(&SWI_TRIG_GPIO, true);
#endif

    FURI_CRITICAL_EXIT();

    if(rw == WchSwioFlasher_SWIO_Read) {
        *data = txrx_data;
        uint8_t parity = result & 0x01;

#ifdef SWIO_TXRX_DEBUG_MSG_ENABLE
        FURI_LOG_D(TAG, "RX from %02X (parity %d): " FMT_4HEX, address, parity, _UI(*data));
#endif
        if(parity != calcParity(address, rw, txrx_data)) {
            return WchSwioFlasher_Error_SwdParityCheckError;
        }
    }

    if((result & 0x1000) != 0) {
        FURI_LOG_E(TAG, "RX Error");
        return WchSwioFlasher_Error_SwdResetDetected;
    }

    return WchSwioFlasher_Ok;
}
#pragma GCC pop_options
