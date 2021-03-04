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

    for(uint8_t i = 0; i < 32; i++) {
        for(uint8_t j = 0; j < 96; j++) {
            uint8_t time = data[j] ? 40 : 32;

            for(uint8_t k = 0; k < 6; k++) {
                delay_us(time + i / 8);
                gpio_write(pin, true);
                delay_us(time - i / 8);
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

void hid_push_symbol(HidCtx* ctx, bool symbol) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(!ctx->is_packet) {
        // finding preamble

        ctx->preamble_buffer[ctx->symbol_counter % 8] = symbol;

        ctx->is_packet = true;

        if(ctx->symbol_counter > 8) {
            for(uint8_t i = 0; i < 8; i++) {
                if(ctx->preamble_buffer[(i + ctx->symbol_counter - 7) % 8] != HID_PREAMBLE[i]) {
                    ctx->is_packet = false;
                    break;
                }
            }
        } else {
            ctx->is_packet = false;
        }

        if(ctx->is_packet) {
            ctx->payload_counter = 0;
            ctx->symbol_counter = 0;
            ctx->last_symbol = symbol;
        }
    } else {
        // collecting bits
        if(ctx->symbol_counter % 2 == 0) {
            if(ctx->last_symbol == 0 && symbol == 1) {
                ctx->payload[ctx->payload_counter] = 0;
            } else if(ctx->last_symbol == 1 && symbol == 0) {
                ctx->payload[ctx->payload_counter] = 1;
            } else {
                // manchester fails
                ctx->is_packet = false;
            }

            ctx->payload_counter++;
        } else {
            ctx->last_symbol = symbol;
        }

        if(ctx->symbol_counter == HID_SIZE * 2) {
            if(xStreamBufferSendFromISR(
                   ctx->stream_buffer, ctx->payload, HID_SIZE, &xHigherPriorityTaskWoken) ==
               HID_SIZE) {
                AppEvent event;
                event.type = EventTypeRx;
                event.value.rx.protocol = ProtocolHid;
                osMessageQueuePut(ctx->event_queue, &event, 0, 0);
            }

            ctx->symbol_counter = 8;
            ctx->is_packet = false;
        }
    }

    ctx->symbol_counter++;
    if(ctx->symbol_counter > 250) {
        ctx->symbol_counter = 0;
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
gpio_write(&debug_0, true);
delay_us(15);
gpio_write(&debug_0, false);
*/

void hid_fsm(HidCtx* ctx, uint32_t t) {
    // wait message will be consumed
    if(xStreamBufferBytesAvailable(ctx->stream_buffer) == HID_SIZE) return;

    ctx->dt = (t - ctx->prev_t) / (SystemCoreClock / 1000000.0f);
    ctx->prev_t = t;

    uint8_t pulse = ctx->dt > 72 ? 1 : 0;

    /*
    if(pulse == 1) {
        gpio_write(&debug_0, true);
        delay_us(5);
        gpio_write(&debug_0, false);
    }
    */

    if(ctx->last_pulse == pulse) {
        if(ctx->pulse_counter == 1) {
            // sample bit
            hid_push_symbol(ctx, pulse);
        }

        ctx->pulse_counter++;
        if(ctx->pulse_counter == 6) {
            ctx->pulse_counter = 0;
        }
    } else {
        ctx->pulse_counter = 0;
    }

    ctx->last_pulse = pulse;
    gpio_write(&debug_0, ctx->is_packet);
}

void hid_init(HidCtx* ctx) {
    ctx->dt = 0;
    ctx->prev_t = 0;
    ctx->pulse_counter = 0;
    ctx->last_pulse = 0;
    ctx->symbol_counter = 0;
    ctx->is_packet = false;
    ctx->last_symbol = 0;
    ctx->payload_counter = 0;
    xStreamBufferReset(ctx->stream_buffer);
}