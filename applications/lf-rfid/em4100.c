#include <furi.h>
#include <api-hal.h>
#include <stream_buffer.h>

#include "lf-rfid.h"
#include "em4100.h"

void em4100_prepare_data(uint32_t ID, uint32_t VENDOR, uint8_t* data) {
    uint8_t value[10];

    // vendor rows (4 bit in a row)
    value[0] = (VENDOR >> 4) & 0xF;
    value[1] = VENDOR & 0xF;

    const uint8_t ROW_SIZE = 4;
    const uint8_t HEADER_SIZE = 9;

    // ID rows (4 bit in a row)
    for(int i = 0; i < 8; i++) {
        value[i + 2] = (ID >> (28 - i * ROW_SIZE)) & 0xF;
    }

    for(uint8_t i = 0; i < HEADER_SIZE; i++) {
        data[i] = 1; // header
    }

    for(uint8_t i = 0; i < 10; i++) { // data
        for(uint8_t j = 0; j < ROW_SIZE; j++) {
            data[HEADER_SIZE + i * (ROW_SIZE + 1) + j] = (value[i] >> ((ROW_SIZE - 1) - j)) & 1;
        }

        // row parity
        data[HEADER_SIZE + i * (ROW_SIZE + 1) + ROW_SIZE] =
            (data[HEADER_SIZE + i * (ROW_SIZE + 1) + 0] +
             data[HEADER_SIZE + i * (ROW_SIZE + 1) + 1] +
             data[HEADER_SIZE + i * (ROW_SIZE + 1) + 2] +
             data[HEADER_SIZE + i * (ROW_SIZE + 1) + 3]) %
            2;
    }

    for(uint8_t i = 0; i < ROW_SIZE; i++) { //checksum
        uint8_t checksum = 0;
        for(uint8_t j = 0; j < 10; j++) {
            checksum += data[HEADER_SIZE + i + j * (ROW_SIZE + 1)];
        }
        data[i + 59] = checksum % 2;
    }

    data[63] = 0; // stop bit

    /*
    printf("em data: ");
    for(uint8_t i = 0; i < 64; i++) {
        printf("%d ", data[i]);
    }
    printf("\r\n");
    */
}

void em4100_emulation(uint8_t* data, GpioPin* pin) {
    taskENTER_CRITICAL();
    gpio_write(pin, true);

    for(uint8_t i = 0; i < 8; i++) {
        for(uint8_t j = 0; j < 64; j++) {
            delay_us(260);
            gpio_write(pin, data[j]);
            delay_us(260);
            gpio_write(pin, !data[j]);
        }
    }

    gpio_write(pin, false);
    taskEXIT_CRITICAL();
}

const uint8_t ROW_SIZE = 4;
const uint8_t LINE_SIZE = 10;

bool em4100_even_check(uint8_t* buf) {
    uint8_t col_parity_sum[ROW_SIZE];
    for(uint8_t col = 0; col < ROW_SIZE; col++) {
        col_parity_sum[col] = 0;
    }

    // line parity
    for(uint8_t line = 0; line < LINE_SIZE; line++) {
        printf("%d: ", line);
        uint8_t parity_sum = 0;
        for(uint8_t col = 0; col < ROW_SIZE; col++) {
            parity_sum += buf[line * (ROW_SIZE + 1) + col];
            col_parity_sum[col] += buf[line * (ROW_SIZE + 1) + col];
            printf("%d ", buf[line * (ROW_SIZE + 1) + col]);
        }
        if((1 & parity_sum) != buf[line * (ROW_SIZE + 1) + ROW_SIZE]) {
            printf(
                "line parity fail at %d (%d : %d)\n",
                line,
                parity_sum,
                buf[line * (ROW_SIZE + 1) + ROW_SIZE]);
            return false;
        }
        printf("\r\n");
    }

    for(uint8_t col = 0; col < ROW_SIZE; col++) {
        if((1 & col_parity_sum[col]) != buf[LINE_SIZE * (ROW_SIZE + 1) + col]) {
            printf(
                "col parity fail at %d (%d : %d)\n",
                col,
                col_parity_sum[col],
                buf[LINE_SIZE * (ROW_SIZE + 1) + col]);
            return false;
        }
    }

    return true;
}

void em4100_extract_data(uint8_t* buf, uint8_t* customer, uint32_t* em_data) {
    uint32_t data = 0;
    uint8_t offset = 0;

    printf("customer: ");
    for(uint8_t line = 0; line < 2; line++) {
        for(uint8_t col = 0; col < ROW_SIZE; col++) {
            uint32_t bit = buf[line * (ROW_SIZE + 1) + col];

            data |= bit << (7 - offset);
            printf("%ld ", bit);

            offset++;
        }
    }
    printf("\r\n");

    *customer = data;

    data = 0;
    offset = 0;
    printf("data: ");
    for(uint8_t line = 2; line < LINE_SIZE; line++) {
        for(uint8_t col = 0; col < ROW_SIZE; col++) {
            uint32_t bit = buf[line * (ROW_SIZE + 1) + col];

            data |= bit << (31 - offset);
            printf("%ld ", bit);

            offset++;
        }
    }
    printf("\r\n");

    *em_data = data;
}

void em4100_init(Em4100Ctx* ctx) {
    ctx->prev_dwt = 0;
    ctx->symbol = -1; // init state
    ctx->center = false;
    ctx->symbol_cnt = 0;
    xStreamBufferReset(ctx->stream_buffer);

    for(size_t i = 0; i < 64; i++) {
        ctx->int_buffer[i] = 0;
    }
}

void em4100_fsm(Em4100Ctx* ctx, bool rx_value, uint32_t dt) {
    // wait message will be consumed
    if(xStreamBufferBytesAvailable(ctx->stream_buffer) == 64) return;

    // gpio_write(&debug_0, true);

    if(dt > 384) {
        // change symbol 0->1 or 1->0
        ctx->symbol = rx_value;
        ctx->center = true;
    } else {
        // same symbol as prev or center
        ctx->center = !ctx->center;
    }

    /*
    gpio_write(&debug_1, true);
    delay_us(center ? 10 : 30);
    gpio_write(&debug_1, false);
    */

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(ctx->center && ctx->symbol != -1) {
        /*
        gpio_write(&debug_0, true);
        delay_us(symbol ? 10 : 30);
        gpio_write(&debug_0, false);
        */

        ctx->int_buffer[ctx->symbol_cnt] = ctx->symbol;
        ctx->symbol_cnt++;
    }

    // check preamble
    if(ctx->symbol_cnt <= 9 && ctx->symbol == 0) {
        ctx->symbol_cnt = 0;
        ctx->symbol = -1;
    }

    // check stop bit
    if(ctx->symbol_cnt == 64 && ctx->symbol == 1) {
        ctx->symbol_cnt = 0;
        ctx->symbol = -1;
    }

    // TODO
    // write only 9..64 symbols directly to streambuffer

    if(ctx->symbol_cnt == 64) {
        if(xStreamBufferSendFromISR(
               ctx->stream_buffer, ctx->int_buffer, 64, &xHigherPriorityTaskWoken) == 64) {
            AppEvent event;
            event.type = EventTypeRx;
            event.value.rx.protocol = ProtocolEm4100;
            osMessageQueuePut(ctx->event_queue, &event, 0, 0);
        }

        ctx->symbol_cnt = 0;
    }

    // gpio_write(&debug_0, false);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
