#include "uart_worker.h"
#include <furi_hal_console.h>
#include <furi_hal_uart.h>

#define TAG "UartWorker"

void uart_worker_change_state(UartWorker* uart_worker, UartWorkerState state) {
    furi_assert(uart_worker);

    uart_worker->state = state;
}

void uart_worker_change_rx_sub_state(UartWorker* uart_worker, UartWorkerStateRxSub rx_sub_state) {
    furi_assert(uart_worker);

    uart_worker->rxSubState = rx_sub_state;
}

UartWorker* uart_worker_alloc(NfcRelayConfigUart config) {
    UartWorker* uart_worker = malloc(sizeof(UartWorker));

    uart_worker->thread = furi_thread_alloc_ex("UartWorker", 2048, uart_worker_task, uart_worker);
    uart_worker->rx_stream = furi_stream_buffer_alloc(512, 1);
    uart_worker->packet = NULL;
    uart_worker->config = config;

    uart_worker_change_state(uart_worker, UartWorkerStateReady);
    uart_worker_change_rx_sub_state(uart_worker, UartWorkerStateRxSubReadTyp);

    return uart_worker;
}

void uart_worker_free(UartWorker* uart_worker) {
    furi_assert(uart_worker);

    furi_thread_free(uart_worker->thread);
    furi_stream_buffer_free(uart_worker->rx_stream);

    free(uart_worker);
}

void uart_worker_stop(UartWorker* uart_worker) {
    furi_assert(uart_worker);

    uart_worker_change_state(uart_worker, UartWorkerStateStop);

    furi_hal_uart_set_irq_cb(uart_worker->config.uartId, NULL, NULL);
    if(uart_worker->config.uartId == FuriHalUartIdUSART1) {
        furi_hal_console_enable();
    } else if(uart_worker->config.uartId == FuriHalUartIdLPUART1) {
        furi_hal_uart_deinit(FuriHalUartIdLPUART1);
    }

    furi_thread_join(uart_worker->thread);
}

static void uart_worker_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    furi_assert(context);
    UartWorker* uart_worker = context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(uart_worker->rx_stream, &data, 1, 0);
        //furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
        uart_worker_change_state(uart_worker, UartWorkerStateRx);
    }
}

void uart_worker_start(UartWorker* uart_worker) {
    furi_assert(uart_worker);
    if(uart_worker->config.uartId == FuriHalUartIdUSART1) {
        furi_hal_console_disable();
        furi_hal_uart_set_br(FuriHalUartIdUSART1, uart_worker->config.baudrate);
    } else if(uart_worker->config.uartId == FuriHalUartIdLPUART1) {
        furi_hal_uart_init(FuriHalUartIdLPUART1, uart_worker->config.baudrate);
    }
    furi_hal_uart_set_irq_cb(uart_worker->config.uartId, uart_worker_on_irq_cb, uart_worker);

    furi_thread_start(uart_worker->thread);
}

void uart_worker_tx(UartWorker* uart_worker, uint8_t* data, size_t len) {
    furi_assert(uart_worker);
    furi_assert(data);

    furi_hal_uart_tx(uart_worker->config.uartId, data, len);
}

void uart_worker_tx_packet(UartWorker* uart_worker, NfcRelayPacket* packet) {
    furi_assert(uart_worker);

    uart_worker_tx(uart_worker, (uint8_t*)packet, packet->len + sizeof(NfcRelayPacket));
}

NfcRelayPacket* uart_worker_pop_packet(UartWorker* uart_worker) {
    if(uart_worker->packet == NULL) {
        return NULL;
    }
    NfcRelayPacket* tmp = uart_worker->packet;
    uart_worker->packet = NULL;
    uart_worker_change_rx_sub_state(uart_worker, UartWorkerStateRxSubReadTyp);
    FURI_LOG_D(TAG, "pop packet success");
    return tmp;
}

int32_t uart_worker_task(void* context) {
    furi_assert(context);
    UartWorker* uart_worker = context;

    uint8_t typ = 0;
    uint8_t len = 0;
    uint8_t data[256];
    uint8_t tmpdata[256];
    uint8_t recv_len = 0;
    while(1) {
        if(uart_worker->state == UartWorkerStateStop) break;

        if(uart_worker->rxSubState == UartWorkerStateRxSubReadTyp && (!uart_worker->packet)) {
            size_t rlen = furi_stream_buffer_receive(uart_worker->rx_stream, &typ, 1, 0);
            if(rlen == 1) {
                FURI_LOG_D(TAG, "Rx Typ");
                uart_worker_change_rx_sub_state(uart_worker, UartWorkerStateRxSubReadLen);
            }
        } else if(uart_worker->rxSubState == UartWorkerStateRxSubReadLen) {
            FURI_LOG_D(TAG, "Rx Len");
            size_t rlen = furi_stream_buffer_receive(uart_worker->rx_stream, &len, 1, 0);
            if(rlen == 1) {
                if(len == 0) {
                    FURI_LOG_D(TAG, "New packet");
                    NfcRelayPacket* tmp = malloc(sizeof(NfcRelayPacket));
                    tmp->typ = typ;
                    tmp->len = len;
                    uart_worker->packet = tmp;
                    furi_stream_set_trigger_level(uart_worker->rx_stream, 1);
                    uart_worker_change_rx_sub_state(uart_worker, UartWorkerStateRxSubReadFinish);
                    FURI_LOG_D(TAG, "Read packet Finish (len==0)");
                } else {
                    FURI_LOG_D(TAG, "New packet with data");
                    recv_len = 0;
                    uart_worker_change_rx_sub_state(uart_worker, UartWorkerStateRxSubReadData);
                }
            }
        } else if(uart_worker->rxSubState == UartWorkerStateRxSubReadData) {
            size_t rlen =
                furi_stream_buffer_receive(uart_worker->rx_stream, &tmpdata, len - recv_len, 0);
            if(!rlen) continue;
            FURI_LOG_D(TAG, "Rx Data");
            FURI_LOG_D(TAG, "rlen: %d, recv_len: %d, len: %d", rlen, recv_len, len);
            memcpy(&data[recv_len], &tmpdata, rlen);
            recv_len += rlen;

            if(recv_len == len) {
                FURI_LOG_D(TAG, "New packet");
                NfcRelayPacket* tmp = malloc(sizeof(NfcRelayPacket) + len);
                tmp->typ = typ;
                tmp->len = len;
                memcpy(&tmp->buf, data, len);
                uart_worker->packet = tmp;
                uart_worker_change_rx_sub_state(uart_worker, UartWorkerStateRxSubReadFinish);
                furi_stream_set_trigger_level(uart_worker->rx_stream, 1);
                FURI_LOG_D(TAG, "Read packet Finish");
            }
        } else if(uart_worker->rxSubState == UartWorkerStateRxSubReadFinish) {
            continue;
        }
    }

    return 0;
}
