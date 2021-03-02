#pragma once

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <stream_buffer.h>

typedef struct {
    bool preamble_buffer[8];
    uint16_t symbol_counter;
    bool is_preamble;
    uint32_t prev_dwt;
    uint8_t prev_dt;
    uint8_t pulse_cnt;
} HidCtx;

void hid_emulation(uint8_t* data, GpioPin* pin);
void hid_prepare_data(uint8_t facility_code, uint16_t card_no, uint8_t* data);
void hid_fsm(HidCtx* ctx, uint32_t dwt);
void hid_init(HidCtx* ctx);
