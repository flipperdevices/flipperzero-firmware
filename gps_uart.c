#include <string.h>

#include "minmea.h"
#include "gps_uart.h"

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

#define WORKER_ALL_RX_EVENTS                                                      \
    (WorkerEvtStop | WorkerEvtRxDone)

static void gps_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    GpsUart* gps_uart = (GpsUart*)context;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(ev == UartIrqEventRXNE) {
        xStreamBufferSendFromISR(gps_uart->rx_stream, &data, 1, &xHigherPriorityTaskWoken);
        furi_thread_flags_set(furi_thread_get_id(gps_uart->thread), WorkerEvtRxDone);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void gps_uart_serial_init(GpsUart* gps_uart) {
    furi_hal_console_disable();
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, gps_uart_on_irq_cb, gps_uart);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, GPS_BAUDRATE);
}

static void gps_uart_serial_deinit(GpsUart* gps_uart) {
    UNUSED(gps_uart);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    furi_hal_console_enable();
}

static void gps_uart_parse_nmea(GpsUart* gps_uart, char* line)
{
    switch(minmea_sentence_id(line, false)) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line)) {
                gps_uart->status.latitude = minmea_tocoord(&frame.latitude);
                gps_uart->status.longitude = minmea_tocoord(&frame.longitude);
            }
        } break;

        default: break;
    }
}

static int32_t gps_uart_worker(void* context) {
    GpsUart* gps_uart = (GpsUart*)context;

    gps_uart->rx_stream = xStreamBufferCreate(RX_BUF_SIZE * 5, 1);
    size_t rx_offset = 0;

    gps_uart_serial_init(gps_uart);

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            size_t len = 0;
            do {
                len = xStreamBufferReceive(gps_uart->rx_stream, gps_uart->rx_buf + rx_offset,
                                           RX_BUF_SIZE - 1 - rx_offset, 0);
                if(len > 0) {
                    rx_offset += len;
                    gps_uart->rx_buf[rx_offset] = '\0';

                    char * line_current = (char *)gps_uart->rx_buf;
                    while(1) {
                        while (*line_current == '\0' && line_current < (char *)gps_uart->rx_buf + rx_offset - 1)
                            line_current++;
                        char * newline = strchr(line_current, '\n');
                        if(newline) {
                            *newline = '\0';
                            gps_uart_parse_nmea(gps_uart, line_current);
                            line_current = newline + 1;
                        } else {
                            if(line_current > (char *)gps_uart->rx_buf) {
                                rx_offset = 0;
                                while(*line_current) {
                                    gps_uart->rx_buf[rx_offset++] = *(line_current++);
                                }
                            }
                            break;
                        }
                    }
                }
            } while(len > 0);
        }
    }

    gps_uart_serial_deinit(gps_uart);

    vStreamBufferDelete(gps_uart->rx_stream);

    return 0;
}

GpsUart* gps_uart_enable() {
    GpsUart* gps_uart = malloc(sizeof(GpsUart));

    gps_uart->status.latitude = 0.0;
    gps_uart->status.longitude = 0.0;

    gps_uart->thread = furi_thread_alloc();
    furi_thread_set_name(gps_uart->thread, "GpsUartWorker");
    furi_thread_set_stack_size(gps_uart->thread, 1024);
    furi_thread_set_context(gps_uart->thread, gps_uart);
    furi_thread_set_callback(gps_uart->thread, gps_uart_worker);

    furi_thread_start(gps_uart->thread);
    return gps_uart;
}

void gps_uart_disable(GpsUart* gps_uart) {
    furi_assert(gps_uart);
    furi_thread_flags_set(furi_thread_get_id(gps_uart->thread), WorkerEvtStop);
    furi_thread_join(gps_uart->thread);
    furi_thread_free(gps_uart->thread);
    free(gps_uart);
}
