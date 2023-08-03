#include "uhf_worker.h"
#include "uhf_cmd.h"

// inner functions
uint8_t calculate_checksum(uint8_t* arr, size_t length) {
    // CheckSum8 Modulo 256
    // Sum of Bytes % 256
    uint8_t sum_val = 0x00;
    for(size_t i = 0; i < length; i++) {
        sum_val += arr[i];
    }
    return sum_val % 256;
}

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
    // FURI_LOG_E("log", "freeing done");
    UHFData* hardware_version = uhf_response_data->head;
    UHFData* software_version = uhf_response_data_add_new_uhf_data(uhf_response_data);
    UHFData* manufacturer = uhf_response_data_add_new_uhf_data(uhf_response_data);
    // FURI_LOG_E("log", "alloc done");
    furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUD_RATE);
    // read hardware version
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, hardware_version);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_HARDWARE_VERSION.cmd, CMD_HARDWARE_VERSION.length);
    furi_delay_ms(50);
    // read software version
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, software_version);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SOFTWARE_VERSION.cmd, CMD_SOFTWARE_VERSION.length);
    furi_delay_ms(50);
    // read manufacturer
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, manufacturer);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_MANUFACTURERS.cmd, CMD_MANUFACTURERS.length);
    furi_delay_ms(50);
    // FURI_LOG_E("log", "done sending tx");
    if(!hardware_version->end || !software_version->end || !manufacturer->end) {
        return UHFWorkerEventFail;
    }
    return UHFWorkerEventSuccess;
}

UHFWorkerEvent read_single_card(UHFWorker* uhf_worker) {
    UHFResponseData* uhf_response_data = uhf_worker->response_data;
    uhf_response_data_reset(uhf_response_data);
    UHFData* raw_read_data = uhf_response_data_get_uhf_data(uhf_response_data, 0);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUD_RATE);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, raw_read_data);
    uhf_data_reset(raw_read_data);
    // read epc bank
    while(true) {
        furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SINGLE_POLLING.cmd, CMD_SINGLE_POLLING.length);
        furi_delay_ms(100);
        if(uhf_worker->state == UHFWorkerStateStop) {
            return UHFWorkerEventAborted;
        }
        if(raw_read_data->end) {
            if(raw_read_data->data[1] == 0x01 && raw_read_data->data[5] == 0x15) {
                uhf_data_reset(raw_read_data);
                continue;
            } else if(raw_read_data->data[1] == 0x02)
                break; // read success
        }
    }
    // Set select
    uint8_t* select_cmd = (uint8_t*)malloc(sizeof(uint8_t) * CMD_SET_SELECT_PARAMETER.length);
    memcpy(select_cmd, (void*)&CMD_SET_SELECT_PARAMETER.cmd[0], CMD_SET_SELECT_PARAMETER.length);
    // set select param
    select_cmd[5] = 0x01; // 0x00=rfu, 0x01=epc, 0x10=tid, 0x11=user
    // set ptr
    select_cmd[9] = 0x20;
    // set mask
    for(int i = 0; i < 12; i++) // 96 bits, 12 bytes
    {
        select_cmd[12 + i] = raw_read_data->data[8 + i];
    }
    // checksum
    select_cmd[CMD_SET_SELECT_PARAMETER.length - 2] =
        calculate_checksum(select_cmd + 1, CMD_SET_SELECT_PARAMETER.length - 3);
    UHFData* select_response = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, select_response);
    furi_hal_uart_tx(FuriHalUartIdUSART1, select_cmd, CMD_SET_SELECT_PARAMETER.length);
    furi_delay_ms(50);

    if(select_response->data[5] != 0x00) return UHFWorkerEventFail;

    uint8_t* read_bank_cmd =
        (uint8_t*)malloc(sizeof(uint8_t) * CMD_READ_LABEL_DATA_STORAGE.length);
    memcpy(
        read_bank_cmd,
        (void*)&CMD_READ_LABEL_DATA_STORAGE.cmd[0],
        CMD_READ_LABEL_DATA_STORAGE.length);

    // read rfu bank
    UHFData* rfu_bank = select_response;
    uhf_data_reset(rfu_bank);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, rfu_bank);
    read_bank_cmd[9] = 0x00;
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd, CMD_READ_LABEL_DATA_STORAGE.length);
    furi_delay_ms(50);

    // for(int i = 0; i < (int)rfu_bank->length; i++) {
    //     FURI_LOG_E("rfu_bank", "data[%d]=%02x", i, rfu_bank->data[i]);
    // }

    // read epc bank
    UHFData* epc_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, epc_bank);
    read_bank_cmd[9] = 0x01;
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd, CMD_READ_LABEL_DATA_STORAGE.length);
    furi_delay_ms(50);

    // for(int i = 0; i < (int)epc_bank->length; i++) {
    //     FURI_LOG_E("epc_bank", "data[%d]=%02x", i, epc_bank->data[i]);
    // }

    // read tid bank
    UHFData* tid_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, tid_bank);
    read_bank_cmd[9] = 0x02;
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd, CMD_READ_LABEL_DATA_STORAGE.length);
    furi_delay_ms(50);

    // for(int i = 0; i < (int)tid_bank->length; i++) {
    //     FURI_LOG_E("tid_bank", "data[%d]=%02x", i, tid_bank->data[i]);
    // }

    // read user bank
    UHFData* user_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, user_bank);
    read_bank_cmd[9] = 0x03;
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd, CMD_READ_LABEL_DATA_STORAGE.length);
    furi_delay_ms(50);

    // for(int i = 0; i < (int)user_bank->length; i++) {
    //     FURI_LOG_E("user_bank", "data[%d]=%02x", i, user_bank->data[i]);
    // }

    free(select_cmd);
    free(read_bank_cmd);
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