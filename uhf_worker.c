#include "uhf_worker.h"
#include "uhf_cmd.h"

void single_poll_rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    UHFResponseData* response_data = ctx;
    UHFData* first_data = response_data->data;
    uhf_data_append(first_data, data);
}

UHFWorkerEvent read_single_card(UHFWorker* uhf_worker) {
    UHFResponseData* uhf_response_data = uhf_worker->data;
    furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUD_RATE);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, single_poll_rx_callback, uhf_response_data);
    UHFData* uhf_data = uhf_response_data->data;
    uhf_data_reset(uhf_data);
    while(!uhf_data->end) {
        furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SINGLE_POLLING.cmd, CMD_SINGLE_POLLING.length);
        furi_delay_ms(100);
        if(uhf_worker->state == UHFWorkerStateStop) {
            return UHFWorkerEventAborted;
        }
    }
    return UHFWorkerEventSuccess;
}

int32_t uhf_worker_task(void* ctx) {
    UHFWorker* uhf_worker = ctx;
    if(uhf_worker->state == UHFWorkerStateDetect) {
        UHFWorkerEvent event = read_single_card(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    }
    return 0;
}

UHFWorker* uhf_worker_alloc() {
    UHFWorker* uhf_worker = (UHFWorker*)malloc(sizeof(UHFWorker));
    uhf_worker->thread = furi_thread_alloc_ex("UHFWorker", 8 * 1024, uhf_worker_task, uhf_worker);
    uhf_worker->data = uhf_response_data_alloc();
    uhf_worker->callback = NULL;
    uhf_worker->ctx = NULL;
    return uhf_worker;
}

void uhf_worker_change_state(UHFWorker* worker, UHFWorkerState state) {
    worker->state = state;
}

void uhf_worker_start(
    UHFWorker* uhf_worker,
    UHFWorkerState state,
    UHFWorkerCallback callback,
    void* ctx) {
    uhf_worker->state = state;
    uhf_worker->callback = callback;
    uhf_worker->ctx = ctx;
    furi_thread_start(uhf_worker->thread);
}

void uhf_worker_stop(UHFWorker* uhf_worker) {
    furi_assert(uhf_worker);
    furi_assert(uhf_worker->thread);

    if(furi_thread_get_state(uhf_worker->thread) != FuriThreadStateStopped) {
        uhf_worker_change_state(uhf_worker, UHFWorkerStateStop);
        furi_thread_join(uhf_worker->thread);
    }
}

void uhf_worker_free(UHFWorker* uhf_worker) {
    furi_assert(uhf_worker);
    furi_thread_free(uhf_worker->thread);
    uhf_response_data_free(uhf_worker->data);
    free(uhf_worker);
}