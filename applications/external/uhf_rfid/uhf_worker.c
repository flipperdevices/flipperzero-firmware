#include "uhf_worker.h"
#include "uhf_cmd.h"

#define CB_DELAY 50

// uart callback functions
void module_rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    UHFData* uhf_data = ctx;
    uhf_data_append(uhf_data, data);
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

static uint8_t get_epc_length_in_bits(uint8_t pc) {
    uint8_t epc_length = pc;
    epc_length >>= 3;
    return (uint8_t)epc_length * 16; // x-words * 16 bits
}

static bool send_set_select_command(UHFData* selected_tag, UHFBank bank) {
    bool success = false;
    // Set select
    UHFData* select_cmd = uhf_data_alloc();
    select_cmd->start = true;
    select_cmd->length = CMD_SET_SELECT_PARAMETER.length;
    memcpy((void*)&select_cmd->data, (void*)&CMD_SET_SELECT_PARAMETER.cmd[0], select_cmd->length);
    // set select param
    size_t mask_length_bits = (size_t)get_epc_length_in_bits(selected_tag->data[6]);
    size_t mask_length_bytes = (size_t)mask_length_bits / 8;
    select_cmd->data[5] = bank; // 0x00=rfu, 0x01=epc, 0x10=tid, 0x11=user
    // set ptr
    select_cmd->data[9] = 0x20; // epc data begins after 0x20
    // set mask length
    select_cmd->data[10] = mask_length_bits;
    // set mask starting position
    select_cmd->length = 12;
    // set mask
    for(size_t i = 0; i < mask_length_bytes; i++) {
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

    success = select_response->data[5] == 0x00;

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
    // debug
    // FuriString* temp_str;
    // temp_str = furi_string_alloc();
    // e-debug
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

    // todo : rfu ?
    UHFTag* uhf_tag = uhf_worker->uhf_tag;
    uhf_tag_reset(uhf_tag);

    // add to tag object
    UHFData* raw_bank_data = uhf_data_alloc();
    size_t epc_length = (size_t)get_epc_length_in_bits(raw_read_data->data[6]) / 8;
    size_t offset = (size_t)(8 + epc_length);

    UHFData* read_bank_cmd = uhf_data_alloc();
    read_bank_cmd->length = CMD_READ_LABEL_DATA_STORAGE.length;
    memcpy(
        (void*)&read_bank_cmd->data[0],
        (void*)&CMD_READ_LABEL_DATA_STORAGE.cmd[0],
        read_bank_cmd->length);

    if(!send_set_select_command(raw_read_data, EPC_BANK)) return UHFWorkerEventFail;

    int retry = 3;
    do {
        if(read_bank(read_bank_cmd, raw_bank_data, EPC_BANK)) {
            uhf_tag_set_epc(uhf_tag, raw_bank_data->data + offset, epc_length + 2);
            FURI_LOG_E("TAG", "epc read");
            break;
        }

    } while(retry--);
    // // debug
    // furi_string_reset(temp_str);
    // for(size_t i = 0; i < raw_bank_data->length; i++) {
    //     furi_string_cat_printf(temp_str, "%02x ", raw_bank_data->data[i]);
    // }
    // FURI_LOG_E("TAG", "data = %s", furi_string_get_cstr(temp_str));
    // // e-debug
    uhf_data_reset(raw_bank_data);
    retry = 3;
    do {
        if(read_bank(read_bank_cmd, raw_bank_data, TID_BANK)) {
            uhf_tag_set_tid(uhf_tag, raw_bank_data->data + offset, 16);
            break;
        }
    } while(retry--);
    // // debug
    // furi_string_reset(temp_str);
    // for(size_t i = 0; i < raw_bank_data->length; i++) {
    //     furi_string_cat_printf(temp_str, "%02x ", raw_bank_data->data[i]);
    // }
    // FURI_LOG_E("TAG", "data = %s", furi_string_get_cstr(temp_str));
    // // e-debug
    uhf_data_reset(raw_bank_data);
    retry = 3;
    if(raw_read_data->data[6] & 0x04) {
        do {
            if(read_bank(read_bank_cmd, raw_bank_data, USER_BANK)) {
                uhf_tag_set_user(uhf_tag, raw_bank_data->data + offset, 16);
                break;
            }
        } while(retry--);
    }
    // // debug
    // furi_string_reset(temp_str);
    // for(size_t i = 0; i < raw_bank_data->length; i++) {
    //     furi_string_cat_printf(temp_str, "%02x ", raw_bank_data->data[i]);
    // }
    // FURI_LOG_E("TAG", "data = %s", furi_string_get_cstr(temp_str));
    // // e-debug
    uhf_data_reset(raw_bank_data);
    uhf_data_free(raw_bank_data);
    uhf_data_free(read_bank_cmd);
    // debug
    // furi_string_free(temp_str);
    // e-debug

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