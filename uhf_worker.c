#include "uhf_worker.h"
#include "uhf_tag.h"

// yrm100 module commands
UHFWorkerEvent verify_module_connected(UHFWorker* uhf_worker) {
    char* hw_version = m100_get_hardware_version(uhf_worker->module);
    char* sw_version = m100_get_software_version(uhf_worker->module);
    char* manufacturer = m100_get_manufacturers(uhf_worker->module);
    // verify all data exists
    if(hw_version == NULL || sw_version == NULL || manufacturer == NULL) return UHFWorkerEventFail;
    return UHFWorkerEventSuccess;
}

uint8_t get_epc_length_in_bits(uint8_t pc) {
    uint8_t epc_length = pc;
    epc_length >>= 3;
    return (uint8_t)epc_length * 16; // x-words * 16 bits
}

// bool read_bank(UHFData* read_bank_cmd, UHFData* response_bank, UHFBank bank) {
//     furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, response_bank);
//     read_bank_cmd->data[9] = bank;
//     read_bank_cmd->data[read_bank_cmd->length - 2] = uhf_data_calculate_checksum(read_bank_cmd);
//     uhf_data_reset(response_bank);
//     furi_hal_uart_tx(FuriHalUartIdUSART1, read_bank_cmd->data, read_bank_cmd->length);
//     furi_delay_ms(CB_DELAY);
//     return response_bank->data[2] == read_bank_cmd->data[2];
// }

// bool write_bank(UHFData* write_bank_cmd, UHFBank bank, uint8_t* bank_data, size_t bank_len) {
//     UHFData* rp_data = uhf_data_alloc();
//     write_bank_cmd->end = false;
//     for(size_t i = 0; i < write_bank_cmd->length; i++) {
//         continue;
//     }
//     furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, module_rx_callback, rp_data);
//     for(int i = 5; i < 9; i++) { // no access password for now
//         write_bank_cmd->data[i] = 0;
//     }
//     write_bank_cmd->data[9] = bank;
//     size_t word_len = bank_len / 2;
//     write_bank_cmd->data[13] = word_len;
//     write_bank_cmd->length = 14;
//     write_bank_cmd->start = true;
//     for(size_t i = 0; i < bank_len; i++) {
//         uhf_data_append(write_bank_cmd, bank_data[i]);
//     }
//     uhf_data_append(write_bank_cmd, 00);
//     uhf_data_append(write_bank_cmd, FRAME_END);
//     write_bank_cmd->data[4] = write_bank_cmd->length - 7;
//     write_bank_cmd->data[write_bank_cmd->length - 2] = uhf_data_calculate_checksum(write_bank_cmd);
//     furi_hal_uart_tx(FuriHalUartIdUSART1, write_bank_cmd->data, write_bank_cmd->length);
//     furi_delay_ms(CB_DELAY);
//     bool success = rp_data->data[2] == write_bank_cmd->data[2];
//     uhf_data_free(rp_data);
//     return success;
// }

UHFTag* send_polling_command(UHFWorker* uhf_worker) {
    // read epc bank
    UHFTag* uhf_tag;
    while(true) {
        uhf_tag = m100_send_single_poll(uhf_worker->module);
        furi_delay_ms(100);
        if(uhf_worker->state == UHFWorkerStateStop) {
            return NULL;
        }
        if(uhf_tag != NULL) break;
        FURI_LOG_E("WKR", "null still");
    }
    return uhf_tag;
}

UHFWorkerEvent read_single_card(UHFWorker* uhf_worker) {
    UHFTag* uhf_tag = send_polling_command(uhf_worker);
    if(uhf_tag == NULL) return UHFWorkerEventAborted;
    uhf_tag_wrapper_set_tag(uhf_worker->uhf_tag_wrapper, uhf_tag);
    // Todo : set select here
    bool select_success = m100_set_select(uhf_worker->module, uhf_tag);
    FURI_LOG_E("TAG", "select success = %d", select_success);

    // Todo : read rfu
    m100_read_label_data_storage(uhf_worker->module, uhf_tag, ReservedBank, 0);
    // Todo : read epc
    m100_read_label_data_storage(uhf_worker->module, uhf_tag, EPCBank, 0);
    // Todo : read tid
    m100_read_label_data_storage(uhf_worker->module, uhf_tag, TIDBank, 0);
    // Todo : read user
    m100_read_label_data_storage(uhf_worker->module, uhf_tag, UserBank, 0);
    // add to tag object
    // UHFData* raw_bank_data = uhf_data_alloc();
    // size_t epc_length = (size_t)get_epc_length_in_bits(raw_read_data->data[6]) / 8;
    // size_t offset = (size_t)(8 + epc_length);

    // UHFData* read_bank_cmd = uhf_data_alloc();
    // read_bank_cmd->length = CMD_READ_LABEL_DATA_STORAGE.length;
    // memcpy(
    //     (void*)&read_bank_cmd->data[0],
    //     (void*)&CMD_READ_LABEL_DATA_STORAGE.cmd[0],
    //     read_bank_cmd->length);

    // if(!send_set_select_command(raw_read_data, EPC_BANK)) return UHFWorkerEventFail;

    // int retry = 3;
    // do {
    //     if(read_bank(read_bank_cmd, raw_bank_data, EPC_BANK)) {
    //         uhf_tag_set_epc(uhf_tag, raw_bank_data->data + offset, epc_length + 2);
    //         break;
    //     }

    // } while(retry--);
    // // // debug
    // // furi_string_reset(temp_str);
    // // for(size_t i = 0; i < raw_bank_data->length; i++) {
    // //     furi_string_cat_printf(temp_str, "%02x ", raw_bank_data->data[i]);
    // // }
    // // FURI_LOG_E("TAG", "data = %s", furi_string_get_cstr(temp_str));
    // // // e-debug
    // uhf_data_reset(raw_bank_data);
    // retry = 3;
    // do {
    //     if(read_bank(read_bank_cmd, raw_bank_data, TID_BANK)) {
    //         uhf_tag_set_tid(uhf_tag, raw_bank_data->data + offset, 16);
    //         break;
    //     }
    // } while(retry--);
    // // // debug
    // // furi_string_reset(temp_str);
    // // for(size_t i = 0; i < raw_bank_data->length; i++) {
    // //     furi_string_cat_printf(temp_str, "%02x ", raw_bank_data->data[i]);
    // // }
    // // FURI_LOG_E("TAG", "data = %s", furi_string_get_cstr(temp_str));
    // // // e-debug
    // uhf_data_reset(raw_bank_data);
    // retry = 3;
    // if(raw_read_data->data[6] & 0x04) {
    //     do {
    //         if(read_bank(read_bank_cmd, raw_bank_data, USER_BANK)) {
    //             uhf_tag_set_user(uhf_tag, raw_bank_data->data + offset, 16);
    //             break;
    //         }
    //     } while(retry--);
    // }
    // // // debug
    // // furi_string_reset(temp_str);
    // // for(size_t i = 0; i < raw_bank_data->length; i++) {
    // //     furi_string_cat_printf(temp_str, "%02x ", raw_bank_data->data[i]);
    // // }
    // // FURI_LOG_E("TAG", "data = %s", furi_string_get_cstr(temp_str));
    // // // e-debug
    // uhf_data_reset(raw_bank_data);
    // uhf_data_free(raw_bank_data);
    // uhf_data_free(read_bank_cmd);
    // // debug
    // // furi_string_free(temp_str);
    // // e-debug

    return UHFWorkerEventSuccess;
}

// UHFWorkerEvent write_single_card(UHFWorker* uhf_worker) {
//     UHFResponseData* uhf_response_data = uhf_worker->response_data;
//     uhf_response_data_reset(uhf_response_data);
//     UHFData* raw_read_data = uhf_response_data_get_uhf_data(uhf_response_data, 0);
//     furi_hal_uart_set_br(FuriHalUartIdUSART1, DEFAULT_BAUD_RATE);

//     send_polling_command(uhf_worker, raw_read_data);
//     // todo : rfu ?
//     UHFTag* uhf_tag = uhf_worker->uhf_tag;

//     UHFData* write_bank_cmd = uhf_data_alloc();
//     write_bank_cmd->length = CMD_WRITE_LABEL_DATA_STORAGE.length;

//     memcpy(
//         (void*)&write_bank_cmd->data[0],
//         (void*)&CMD_WRITE_LABEL_DATA_STORAGE.cmd[0],
//         write_bank_cmd->length);
//     if(!send_set_select_command(raw_read_data, EPC_BANK)) return UHFWorkerEventFail;

//     if(raw_read_data->data[6] & 0x04) {
//         if(!write_bank(write_bank_cmd, USER_BANK, uhf_tag->user, uhf_tag->user_length))
//             return UHFWorkerEventFail;
//     }
//     uint8_t write_data[uhf_tag->epc_length + 2];
//     memcpy(&write_data, &raw_read_data->data[raw_read_data->length - 4], 2);
//     memcpy(&write_data[2], &uhf_tag->epc, uhf_tag->epc_length);
//     write_data[10] = 0xF1;
//     if(!write_bank(write_bank_cmd, EPC_BANK, write_data, uhf_tag->epc_length + 2)) {
//         return UHFWorkerEventFail;
//     }
//     return UHFWorkerEventSuccess;
// }

int32_t uhf_worker_task(void* ctx) {
    UHFWorker* uhf_worker = ctx;
    if(uhf_worker->state == UHFWorkerStateVerify) {
        UHFWorkerEvent event = verify_module_connected(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    } else if(uhf_worker->state == UHFWorkerStateDetectSingle) {
        UHFWorkerEvent event = read_single_card(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    }
    // else if(uhf_worker->state == UHFWorkerStateWriteSingle) {
    //     UHFWorkerEvent event = write_single_card(uhf_worker);
    //     uhf_worker->callback(event, uhf_worker->ctx);
    // }
    return 0;
}

UHFWorker* uhf_worker_alloc() {
    UHFWorker* uhf_worker = (UHFWorker*)malloc(sizeof(UHFWorker));
    uhf_worker->thread = furi_thread_alloc_ex("UHFWorker", 8 * 1024, uhf_worker_task, uhf_worker);
    uhf_worker->module = m100_module_alloc();
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
    m100_module_free(uhf_worker->module);
    free(uhf_worker);
}