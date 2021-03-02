#include <furi.h>
#include <api-hal.h>

#include "lf-rfid.h"
#include "hid.h"

void hid_prepare_data(uint8_t facility_code, uint16_t card_no, uint8_t* data) {
    for(size_t i = 0; i < 96; i++) {
        data[i] = 0;
    }


    uint64_t card_format = 1;
    uint8_t oem_code = 1;

    uint16_t op_bits = card_no & 0b111111111111;
    uint16_t ep_bits = (card_no >> 12 & 0b1111) | (uint16_t)facility_code << 4;

    uint8_t op = 1;
    for(uint8_t i = 0; i < 12; i++) {
        op ^= (op_bits >> i) & 1;
    }

    uint8_t ep = 0;
    for(uint8_t i = 0; i < 12; i++) {
        ep ^= (ep_bits >> i) & 1;
    }

    uint64_t result = 0;
    result |= ((uint64_t)op & 1) << 0;
    result |= ((uint64_t)card_no & 0xFFFF) << 1;
    result |= ((uint64_t)facility_code & 0xFF) << 17;
    result |= ((uint64_t)ep & 1) << 25;
    result |= ((uint64_t)card_format & 0b11111111111) << 26;
    result |= ((uint64_t)oem_code & 0b1111111) << 37;

    // write a preamble
    data[3] = 1;
    data[4] = 1;
    data[5] = 1;
    data[7] = 1;

    // oem code
    // data[37] = 1;

    size_t out_idx = 8;

    printf("HID data: [");
    for(uint8_t i = 0; i < 44; i++) {
        if(result & ((uint64_t)1 << (43 - i))) {
            printf("1");
            data[out_idx] = 1;
            data[out_idx + 1] = 0;
            out_idx += 2;
        } else {
            printf("0");
            data[out_idx] = 0;
            data[out_idx + 1] = 1;
            out_idx += 2;
        }
    }
    printf("]\n\r");
}

void hid_emulation(uint8_t* data, GpioPin* pin) {
    taskENTER_CRITICAL();
    gpio_write(pin, false);

    for(uint8_t i = 0; i < 8; i++) {
        for(uint8_t j = 0; j < 96; j++) {
            uint8_t time = data[j] ? 40 : 32;

            for(uint8_t k = 0; k < 6; k++) {
                delay_us(time + i/2);
                gpio_write(pin, true);
                delay_us(time - i/2);
                gpio_write(pin, false);
            }
        }
    }

    gpio_write(pin, false);
    taskEXIT_CRITICAL();
}

uint8_t HID_PREAMBLE[] = {false, false, false, true, true, true, false, true};

static GpioPin debug_0 = {.pin = GPIO_PIN_2, .port = GPIOB};
// static GpioPin debug_1 = {.pin = GPIO_PIN_3, .port = GPIOC};

void hid_push_bit(HidCtx* ctx, bool bit) {
    if(!ctx->is_preamble) {
        // finding preamble

        ctx->preamble_buffer[ctx->symbol_counter % 8] = bit;

        ctx->is_preamble = true;
        if(ctx->symbol_counter > 8) {
            for(uint8_t i = 0; i < 8; i++) {
                if(ctx->preamble_buffer[(i + ctx->symbol_counter - 7) % 8] != HID_PREAMBLE[i]) {
                    ctx->is_preamble = false;
                    break;
                }
            }
        } else {
            ctx->is_preamble = false;
        }

        if(ctx->is_preamble) {
            gpio_write(&debug_0, true);
            delay_us(5);
            gpio_write(&debug_0, false);

            ctx->symbol_counter = 0;
        }
    } else {
        if(ctx->symbol_counter == 88) {
            ctx->symbol_counter = 0;
            ctx->is_preamble = false;
        }
    }

    ctx->symbol_counter++;
}

void hid_fsm(HidCtx* ctx, uint32_t dwt) {
    uint32_t dt = (dwt - ctx->prev_dwt) / (SystemCoreClock / 1000000.0f);
    ctx->prev_dwt = dwt;

    if(abs(ctx->prev_dt - dt) > 5) {
        gpio_write(&debug_0, true);
        delay_us(3);
        gpio_write(&debug_0, false);
    }
    ctx->prev_dt = dt;

    /*
    if(hid_symbol_counter > 500) {
        // try to move sampling frame
        pulse_cnt = (pulse_cnt + 2) % 6;
        hid_symbol_counter = 0;
    } else {
        pulse_cnt++;
    }
    
    if(pulse_cnt == 6) {
        if(dt < 72 && dt > 40) {
            gpio_write(&debug_0, true);
            delay_us(3);
            gpio_write(&debug_0, false);

            hid_push_bit(false);
        } else if(dt >= 72 && dt < 100) {
            gpio_write(&debug_0, true);
            delay_us(15);
            gpio_write(&debug_0, false);

            hid_push_bit(true);
        }

        pulse_cnt = 0;
    }
    */
}

void hid_init(HidCtx* ctx) {
    // TOOD: init preamble_buffer?

    ctx->symbol_counter = 0;
    ctx->is_preamble = true;

    ctx->prev_dwt = 0;
    ctx->prev_dt = 0;

    ctx->pulse_cnt = 0;
}