#include "mhz19.h"
#include <furi.h>
#include <furi_hal.h>

#define MHZ19_UART_EXCHANGE_SIZE (9u)
#define MHZ19_UART_COMMAND_AUTOCALIBRATION (0x79)
#define MHZ19_UART_COMMAND_GAS_CONCENTRATION (0x86)
#define MHZ19_UART_COMMAND_CALIBRATE_ZERO (0x87)
#define MHZ19_UART_COMMAND_CALIBRATE_SPAN (0x88)
#define MHZ19_UART_COMMAND_SET_RANGE (0x89)

struct Mhz19 {
    FuriStreamBuffer* stream;
};

static uint8_t mhz19_checksum(uint8_t* packet) {
    uint8_t checksum = 0;
    for(size_t i = 1; i < 8; i++) {
        checksum += packet[i];
    }
    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}

static void mhz19_uart_worker_uart_cb(UartIrqEvent ev, uint8_t data, void* context) {
    FuriStreamBuffer* stream = context;
    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(stream, &data, 1, 0);
    }
}

Mhz19* mhz19_alloc() {
    Mhz19* instance = malloc(sizeof(Mhz19));
    instance->stream = furi_stream_buffer_alloc(32, MHZ19_UART_EXCHANGE_SIZE);
    return instance;
}

void mhz19_free(Mhz19* instance) {
    furi_assert(instance);
    furi_stream_buffer_free(instance->stream);
    free(instance);
}

void mhz19_open(Mhz19* instance) {
    furi_assert(instance);
    furi_hal_uart_init(FuriHalUartIdLPUART1, 9600);
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, mhz19_uart_worker_uart_cb, instance->stream);
    furi_hal_power_enable_otg();
}

void mhz19_close(Mhz19* instance) {
    furi_assert(instance);
    furi_hal_power_disable_otg();
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, NULL, NULL);
    furi_hal_uart_deinit(FuriHalUartIdLPUART1);
}

bool mhz19_read_gas_concentration(Mhz19* instance, uint32_t* value) {
    furi_assert(instance);

    uint8_t buffer[MHZ19_UART_EXCHANGE_SIZE] = {0};
    furi_stream_buffer_reset(instance->stream);

    // Send Request
    buffer[0] = 0xff;
    buffer[1] = 0x01;
    buffer[2] = MHZ19_UART_COMMAND_GAS_CONCENTRATION;
    buffer[8] = mhz19_checksum(buffer);
    furi_hal_uart_tx(FuriHalUartIdLPUART1, (uint8_t*)buffer, sizeof(buffer));

    // Get response
    bool ret = false;
    do {
        size_t read_size =
            furi_stream_buffer_receive(instance->stream, buffer, sizeof(buffer), 50);
        if(read_size != MHZ19_UART_EXCHANGE_SIZE) {
            FURI_LOG_E("Worker", "RX failed %zu", read_size);
            break;
        }

        if(buffer[8] != mhz19_checksum(buffer)) {
            FURI_LOG_E("Worker", "Incorrect checksum %x!=%x", buffer[8], mhz19_checksum(buffer));
            break;
        }

        *value = (uint32_t)buffer[2] * 256 + buffer[3];

        ret = true;
    } while(false);

    return ret;
}
