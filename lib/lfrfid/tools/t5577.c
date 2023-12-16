#include "t5577.h"

#include <furi.h>
#include <furi_hal_rfid.h>

/****************************************/

// Required clocks to load configurations, time must be >= 3ms
#define T5577_INITIAL_WAIT_CLOCKS 400

// Required clocks to write block, typical time is around 5.6ms
#define T5577_PROGRAM_CLOCKS 700

#define T5577_START_GAP_CLOCKS 30
#define T5577_WRITE_GAP_CLOCKS 18
#define T5577_BIT_0_CLOCKS 24
#define T5577_BIT_1_CLOCKS 56

/****************************************/

#define T5577_OPCODE_PAGE_0 0b10
#define T5577_OPCODE_PAGE_1 0b11
#define T5577_OPCODE_RESET 0b00

/****************************************/

static inline void t5577_furi_delay_clocks(uint32_t clocks) {
    // T = 1/f => 1 / 125khz == 8 microseconds;
    furi_delay_us(clocks * 8);
}

static void t5577_start() {
    furi_hal_rfid_tim_read_start(125000, 0.5);
    // do not ground the antenna
    furi_hal_rfid_pin_pull_release();
}

static void t5577_stop() {
    furi_hal_rfid_tim_read_stop();
    furi_hal_rfid_pins_reset();
}

static void t5577_write_gap(uint32_t clocks) {
    furi_hal_rfid_tim_read_pause();
    t5577_furi_delay_clocks(clocks);
    furi_hal_rfid_tim_read_continue();
}

static void t5577_write_bit(bool value) {
    uint32_t clocks = (value) ? T5577_BIT_1_CLOCKS : T5577_BIT_0_CLOCKS;
    t5577_furi_delay_clocks(clocks);
    t5577_write_gap(T5577_WRITE_GAP_CLOCKS);
}

static void t5577_write_opcode(uint8_t value) {
    t5577_write_bit((value >> 1) & 1);
    t5577_write_bit((value >> 0) & 1);
}

static void t5577_write_reset() {
    t5577_write_gap(T5577_START_GAP_CLOCKS);
    t5577_write_opcode(T5577_OPCODE_RESET);
    t5577_furi_delay_clocks(T5577_INITIAL_WAIT_CLOCKS);
}

static void t5577_write_block(uint8_t page, uint8_t block, bool lock_bit, uint32_t data) {
    // Then we send a write gap to set write mode
    t5577_write_gap(T5577_START_GAP_CLOCKS);

    uint8_t selected_page = (page == 0) ? T5577_OPCODE_PAGE_0 : T5577_OPCODE_PAGE_1;
    t5577_write_opcode(selected_page);
    t5577_write_bit(lock_bit);

    // Send Block Data
    for(uint8_t i = 0; i < 32; i++) {
        t5577_write_bit(data >> (31 - i));
    }

    // Block address -> All blocks in T5577 are 8: 0 to 7
    t5577_write_bit(block >> 2);
    t5577_write_bit(block >> 1);
    t5577_write_bit(block >> 0);

    t5577_furi_delay_clocks(T5577_PROGRAM_CLOCKS);

    // After programmed, the fob returns in read mode
    // If we wrote block 0 we need to reload configurations
    if(block == 0) {
        t5577_write_reset();
    }
}

void t5577_write(LFRFIDT5577* data, uint8_t page) {
    furi_assert(data);
    furi_assert(data->blocks_to_write <= LFRFID_T5577_BLOCK_COUNT);
    furi_assert(page < LFRFID_T5577_PAGE_COUNT);

    t5577_start();
    {
        FURI_CRITICAL_ENTER();

        // After the fob is entered in the field, we wait for loading configuration
        t5577_furi_delay_clocks(T5577_INITIAL_WAIT_CLOCKS);

        for(uint8_t i = 0; i < data->blocks_to_write; i++) {
            t5577_write_block(page, i, false, data->block[i]);
        }

        FURI_CRITICAL_EXIT();
    }
    t5577_stop();
}