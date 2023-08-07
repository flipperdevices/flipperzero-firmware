#include "uhf_worker.h"
#include "uhf_cmd.h"

#define CB_DELAY 50

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
    // verify that we received all data
    if(!hardware_version->end || !software_version->end || !manufacturer->end) {
        return UHFWorkerEventFail;
    }
    // verify all data was received correctly
    if(!uhf_data_verfiy_checksum(hardware_version) ||
       !uhf_data_verfiy_checksum(software_version) || !uhf_data_verfiy_checksum(manufacturer))
        return UHFWorkerEventFail;

    return UHFWorkerEventSuccess;
}

static bool send_set_select_command(UHFData* selected_tag) {
    bool success = false;
    uint16_t pc = 0;
    uint16_t mask_length_bits = 0xF800;
    size_t mask_length_word = 2;
    // Set select
    UHFData* select_cmd = uhf_data_alloc();
    select_cmd->length = CMD_SET_SELECT_PARAMETER.length;
    memcpy((void*)&select_cmd->data, (void*)&CMD_SET_SELECT_PARAMETER.cmd[0], select_cmd->length);
    pc += selected_tag->data[6];
    pc <<= 8;
    pc |= selected_tag->data[7];
    mask_length_bits &= pc;
    mask_length_bits >>= 11; // shift right 11 bits to only get 10h-14h
    // mask length word
    mask_length_word *= (size_t)(mask_length_bits);
    // mask length in bits
    mask_length_bits *= 16;
    // set select param
    select_cmd->data[5] = 0x01; // 0x00=rfu, 0x01=epc, 0x10=tid, 0x11=user
    // set ptr
    select_cmd->data[9] = 0x20;
    // set mask length
    select_cmd->data[10] = mask_length_bits;
    // set mask starting position
    select_cmd->length = 11;
    // set mask
    FURI_LOG_E("TAG", "Mask length (bits=%d, words=%d)", mask_length_bits, mask_length_word);
    for(size_t i = 0; i < mask_length_word; i++) {
        uhf_data_append(select_cmd, selected_tag->data[8 + i]);
    }
    uhf_data_append(select_cmd, 0x00); // add checksum section
    uhf_data_append(select_cmd, FRAME_END); // command end
    // add checksum
    select_cmd->data[select_cmd->length - 2] = uhf_data_calculate_checksum(select_cmd);

    UHFData* select_response = uhf_data_alloc();
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, select_response);
    furi_hal_uart_tx(FuriHalUartIdUSART1, select_cmd->data, select_cmd->length);
    furi_delay_ms(CB_DELAY);

    success = select_response->data[5] != 0x00;

    uhf_data_free(select_cmd);
    uhf_data_free(select_response);

    return success;
}

static bool read_bank(UHFData* read_bank_cmd, UHFData* response_bank, UHFBank bank) {
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, response_bank);
    read_bank_cmd->data[9] = bank;
    read_bank_cmd->data[read_bank_cmd->length - 2] = uhf_data_calculate_checksum(read_bank_cmd);
    uhf_data_reset(response_bank);
    furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd->data, read_bank_cmd->length);
    furi_delay_ms(CB_DELAY);
    return response_bank->data[2] == read_bank_cmd->data[2];
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

    if(send_set_select_command(raw_read_data)) return UHFWorkerEventFail;

    UHFData* read_bank_cmd = uhf_data_alloc();
    read_bank_cmd->length = CMD_READ_LABEL_DATA_STORAGE.length;
    memcpy(
        (void*)&read_bank_cmd->data[0],
        (void*)&CMD_READ_LABEL_DATA_STORAGE.cmd[0],
        read_bank_cmd->length);

    // int retry = 10;
    // read rfu bank
    UHFData* rfu_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    if(!read_bank(read_bank_cmd, rfu_bank, RFU_BANK)) {
        uhf_data_reset(rfu_bank);
    }

    // read epc bank
    UHFData* epc_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    if(!read_bank(read_bank_cmd, epc_bank, EPC_BANK)) {
        uhf_data_reset(epc_bank);
    }

    // read tid bank
    UHFData* tid_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    if(!read_bank(read_bank_cmd, tid_bank, TID_BANK)) {
        uhf_data_reset(tid_bank);
    }

    // read user bank
    UHFData* user_bank = uhf_response_data_add_new_uhf_data(uhf_response_data);
    if(!read_bank(read_bank_cmd, user_bank, USER_BANK)) {
        uhf_data_reset(user_bank);
    }
    FuriString* str;
    str = furi_string_alloc();
    furi_string_cat(str, "RFU : ");
    for(int i = 0; i < (int)rfu_bank->length; i++) {
        furi_string_cat_printf(str, "%02x ", rfu_bank->data[i]);
    }
    FURI_LOG_E("TAG", furi_string_get_cstr(str));
    furi_string_reset(str);
    furi_string_cat(str, "EPC : ");
    for(int i = 0; i < (int)epc_bank->length; i++) {
        furi_string_cat_printf(str, "%02x ", epc_bank->data[i]);
    }
    FURI_LOG_E("TAG", furi_string_get_cstr(str));
    furi_string_reset(str);
    furi_string_cat(str, "TID : ");
    for(int i = 0; i < (int)tid_bank->length; i++) {
        furi_string_cat_printf(str, "%02x ", tid_bank->data[i]);
    }
    FURI_LOG_E("TAG", furi_string_get_cstr(str));
    furi_string_reset(str);
    furi_string_cat(str, "USER : ");
    for(int i = 0; i < (int)user_bank->length; i++) {
        furi_string_cat_printf(str, "%02x ", user_bank->data[i]);
    }
    FURI_LOG_E("TAG", furi_string_get_cstr(str));
    furi_string_reset(str);
    furi_string_free(str);
    // uhf_data_free(select);
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