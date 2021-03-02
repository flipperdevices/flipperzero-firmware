#pragma once

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <stream_buffer.h>

typedef struct {
    uint32_t prev_dwt;
    int8_t symbol;
    bool center;
    size_t symbol_cnt;
    StreamBufferHandle_t stream_buffer;
    uint8_t* int_buffer;
    osMessageQueueId_t event_queue;
} Em4100Ctx;

void em4100_emulation(uint8_t* data, GpioPin* pin);
void em4100_prepare_data(uint32_t ID, uint32_t VENDOR, uint8_t* data);

bool em4100_even_check(uint8_t* buf);
void em4100_extract_data(uint8_t* buf, uint8_t* customer, uint32_t* em_data);

void em4100_init(Em4100Ctx* ctx);
void em4100_fsm(Em4100Ctx* ctx, bool state, uint32_t dt);
