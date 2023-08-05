#include "uhf_worker.h"
#include "uhf_cmd.h"

#define CB_DELAY 50
// inner functions
uint8_t calculate_checksum(UHFData* uhf_data) {
    // CheckSum8 Modulo 256
    // Sum of Bytes % 256
    uint8_t sum_val = 0x00;
    size_t length = uhf_data->length - 2;
    for(size_t i = 1; i < length; i++) {
        sum_val += uhf_data->data[i];
    }
    return sum_val % 256;
}
bool validate_checksum(UHFData* uhf_data) {
    uint8_t data_checksum = uhf_data->data[uhf_data->length - 2];
    uint8_t actual_checksum = calculate_checksum(uhf_data);
    return data_checksum == actual_checksum;
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
    furi_delay_ms(CB_DELAY);
    // read software version
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, software_version);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_SOFTWARE_VERSION.cmd, CMD_SOFTWARE_VERSION.length);
    furi_delay_ms(CB_DELAY);
    // read manufacturer
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, manufacturer);
    furi_hal_uart_tx(FuriHalUartIdUSART1, CMD_MANUFACTURERS.cmd, CMD_MANUFACTURERS.length);
    furi_delay_ms(CB_DELAY);
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
    UHFData* select_cmd = uhf_data_alloc();
    select_cmd->length = CMD_SET_SELECT_PARAMETER.length;
    memcpy((void*)&select_cmd->data, (void*)&CMD_SET_SELECT_PARAMETER.cmd[0], select_cmd->length);
    // set select param
    select_cmd->data[5] = 0x01; // 0x00=rfu, 0x01=epc, 0x10=tid, 0x11=user
    // set ptr
    select_cmd->data[9] = 0x20;
    // set mask
    for(int i = 0; i < 12; i++) // 96 bits, 12 bytes
    {
        select_cmd->data[12 + i] = raw_read_data->data[8 + i];
    }
    // checksum
    select_cmd->data[select_cmd->length - 2] = calculate_checksum(select_cmd);
    UHFData* select_response = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, select_response);
    furi_hal_uart_tx(FuriHalUartIdUSART1, select_cmd->data, select_cmd->length);
    furi_delay_ms(CB_DELAY);

    if(select_response->data[5] != 0x00) return UHFWorkerEventFail;

    UHFData* read_bank_cmd = uhf_data_alloc();
    read_bank_cmd->length = CMD_READ_LABEL_DATA_STORAGE.length;
    memcpy(
        (void*)&read_bank_cmd->data[0],
        (void*)&CMD_READ_LABEL_DATA_STORAGE.cmd[0],
        read_bank_cmd->length);

    // int retry = 10;
    // read rfu bank
    UHFData* rfu_bank = select_response;
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, rfu_bank);
    read_bank_cmd->data[9] = 0x00;
    read_bank_cmd->data[read_bank_cmd->length - 2] = calculate_checksum(read_bank_cmd);
    uhf_data_reset(rfu_bank);
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd->data, read_bank_cmd->length);
    furi_delay_ms(CB_DELAY);
    if(rfu_bank->data[2] != read_bank_cmd->data[2]) {
        uhf_data_reset(rfu_bank);
    }

    // read epc bank
    UHFData* epc_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, epc_bank);
    read_bank_cmd->data[9] = 0x01;
    read_bank_cmd->data[read_bank_cmd->length - 2] = calculate_checksum(read_bank_cmd);
    uhf_data_reset(epc_bank);
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd->data, read_bank_cmd->length);
    furi_delay_ms(CB_DELAY);
    if(epc_bank->data[2] != read_bank_cmd->data[2]) {
        uhf_data_reset(epc_bank);
    }

    // read tid bank
    UHFData* tid_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, tid_bank);
    read_bank_cmd->data[9] = 0x02;
    read_bank_cmd->data[read_bank_cmd->length - 2] = calculate_checksum(read_bank_cmd);
    uhf_data_reset(tid_bank);
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd->data, read_bank_cmd->length);
    furi_delay_ms(CB_DELAY);
    if(tid_bank->data[2] != read_bank_cmd->data[2]) {
        uhf_data_reset(tid_bank);
    }

    // read user bank
    UHFData* user_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, user_bank);
    read_bank_cmd->data[9] = 0x03;
    read_bank_cmd->data[read_bank_cmd->length - 2] = calculate_checksum(read_bank_cmd);
    uhf_data_reset(user_bank);
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd->data, read_bank_cmd->length);
    furi_delay_ms(CB_DELAY);
    if(user_bank->data[2] != read_bank_cmd->data[2]) {
        uhf_data_reset(user_bank);
    }

    // for(int i = 0; i < (int)user_bank->length; i++) {
    //     FURI_LOG_E("user_bank", "data[%d]=%02x", i, user_bank->data[i]);
    // }
    // for(int i = 0; i < (int)rfu_bank->length; i++) {
    //     FURI_LOG_E("rfu_bank", "data[%d]=%02x", i, rfu_bank->data[i]);
    // }
    // for(int i = 0; i < (int)epc_bank->length; i++) {
    //     FURI_LOG_E("epc_bank", "data[%d]=%02x", i, epc_bank->data[i]);
    // }
    // for(int i = 0; i < (int)tid_bank->length; i++) {
    //     FURI_LOG_E("tid_bank", "data[%d]=%02x", i, tid_bank->data[i]);
    // }
    uhf_data_free(select_cmd);
    uhf_data_free(read_bank_cmd);
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