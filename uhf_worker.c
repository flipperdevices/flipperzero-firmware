#include "uhf_worker.h"
#include "uhf_cmd.h"

// uart callback functions
void module_rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    UHFData* uhf_data = ctx;
    uhf_data_append(uhf_data, data);
    // FURI_LOG_E("module_rx_callback", "%02x", data);
}

// yrm100 module commands
UHFWorkerEvent verify_module_connected(UHFWorker* uhf_worker) {
    UHFResponseData* uhf_response_data = uhf_worker->response_data;
    uhf_response_data_reset(uhf_response_data);
    FURI_LOG_E("log", "freeing done");
    UHFData* hardware_version = uhf_response_data->head;
    UHFData* software_version = uhf_response_data_add_new_uhf_data(uhf_response_data);
    UHFData* manufacturer = uhf_response_data_add_new_uhf_data(uhf_response_data);
    FURI_LOG_E("log", "alloc done");
    furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUD_RATE);
    // read hardware version
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, hardware_version);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_HARDWARE_VERSION.cmd, CMD_HARDWARE_VERSION.length);
    furi_delay_ms(150);
    // read software version
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, software_version);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SOFTWARE_VERSION.cmd, CMD_SOFTWARE_VERSION.length);
    furi_delay_ms(150);
    // read manufacturer
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, manufacturer);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_MANUFACTURERS.cmd, CMD_MANUFACTURERS.length);
    furi_delay_ms(150);
    FURI_LOG_E("log", "done sending tx");
    if(!hardware_version->end || !software_version->end || !manufacturer->end) {
        return UHFWorkerEventFail;
    }
    return UHFWorkerEventSuccess;
}

UHFWorkerEvent read_single_card(UHFWorker* uhf_worker) {
    UHFResponseData* uhf_response_data = uhf_worker->response_data;
    UHFData* uhf_data = uhf_response_data->head;
    furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUD_RATE);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, uhf_data);
    uhf_data_reset(uhf_data);
    while(true) {
        furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SINGLE_POLLING.cmd, CMD_SINGLE_POLLING.length);
        furi_delay_ms(100);
        if(uhf_worker->state == UHFWorkerStateStop) {
            return UHFWorkerEventAborted;
        }
        if(uhf_data->end) {
            // before breaking, check if the response is not an error
            // index 1 = response type, index 5 = parameter
            if(uhf_data->data[1] == 0x01 && uhf_data->data[5] == 0x15) {
                continue;
            } else if(uhf_data->data[1] == 0x02)
                break; // success read
        }
    }
    return UHFWorkerEventSuccess;
}

int32_t uhf_worker_task(void* ctx) {
    UHFWorker* uhf_worker = ctx;
    if(uhf_worker->state == UHFWorkerStateVerify) {
        UHFWorkerEvent event = verify_module_connected(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    }
    if(uhf_worker->state == UHFWorkerStateDetectSingle) {
        UHFWorkerEvent event = read_single_card(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    }
    return 0;
}

UHFWorker* uhf_worker_alloc() {
    UHFWorker* uhf_worker = (UHFWorker*)malloc(sizeof(UHFWorker));
    uhf_worker->thread = furi_thread_alloc_ex("UHFWorker", 8 * 1024, uhf_worker_task, uhf_worker);
    uhf_worker->response_data = uhf_response_data_alloc();
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
    uhf_response_data_free(uhf_worker->response_data);
    free(uhf_worker);
}