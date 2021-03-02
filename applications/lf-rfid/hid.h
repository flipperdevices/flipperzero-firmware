#pragma once

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <stream_buffer.h>

typedef struct {
    uint32_t prev_t;
    uint32_t dt;
    uint8_t pulse_counter;
    uint8_t last_pulse;
    uint8_t preamble_buffer[8];
    uint8_t symbol_counter;
    bool is_packet;
    uint8_t last_symbol;
    uint8_t payload_counter;
    uint8_t* payload;
    StreamBufferHandle_t stream_buffer;
    osMessageQueueId_t event_queue;
} HidCtx;

#define HID_SIZE 44

void hid_emulation(uint8_t* data, GpioPin* pin);
void hid_prepare_data(uint8_t facility_code, uint16_t card_no, uint8_t* data);
void hid_fsm(HidCtx* ctx, uint32_t t);
void hid_init(HidCtx* ctx);
