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

UHFTag* send_polling_command(UHFWorker* uhf_worker) {
    // read epc bank
    UHFTag* uhf_tag = uhf_tag_alloc();
    while(true) {
        M100ResponseType status = m100_send_single_poll(uhf_worker->module, uhf_tag);
        furi_delay_ms(100);
        if(uhf_worker->state == UHFWorkerStateStop) {
            uhf_tag_free(uhf_tag);
            return NULL;
        }
        if(status == M100Success) break;
    }
    return uhf_tag;
}

static UHFWorkerEvent
    read_bank_till_max_length(UHFWorker* uhf_worker, UHFTag* uhf_tag, BankType bank) {
    unsigned int retry = 3, word_low = 5, word_high = 100;
    unsigned int word_size;
    M100ResponseType status;
    do {
        if(uhf_worker->state == UHFWorkerStateStop) return UHFWorkerEventAborted;
        if(word_low >= word_high) return UHFWorkerEventSuccess;
        word_size = (word_low + word_high) / 2;
        status = m100_read_label_data_storage(uhf_worker->module, uhf_tag, bank, 0, word_size);
        if(status == M100Success) {
            word_low = word_size + 1;
        } else if(status == M100MemoryOverrun) {
            word_high = word_size - 1;
        } else if(status == M100NoTagResponse) {
            retry--;
        }
    } while(retry);
    return UHFWorkerEventSuccess;
}

UHFWorkerEvent read_single_card(UHFWorker* uhf_worker) {
    UHFTag* uhf_tag = send_polling_command(uhf_worker);
    if(uhf_tag == NULL) return UHFWorkerEventAborted;
    uhf_tag_wrapper_set_tag(uhf_worker->uhf_tag_wrapper, uhf_tag);
    // Todo : set select here
    if(m100_set_select(uhf_worker->module, uhf_tag) != M100Success) return UHFWorkerEventFail;
    // read tid
    UHFWorkerEvent event;
    event = read_bank_till_max_length(uhf_worker, uhf_tag, TIDBank);
    if(event != UHFWorkerEventSuccess) return event;
    // read user
    event = read_bank_till_max_length(uhf_worker, uhf_tag, UserBank);
    if(event != UHFWorkerEventSuccess) return event;
    return UHFWorkerEventSuccess;
}

UHFWorkerEvent write_single_card(UHFWorker* uhf_worker) {
    UHFTag* uhf_tag_des = send_polling_command(uhf_worker);
    if(uhf_tag_des == NULL) return UHFWorkerEventAborted;
    UHFTag* uhf_tag_from = uhf_worker->uhf_tag_wrapper->uhf_tag;
    if(m100_set_select(uhf_worker->module, uhf_tag_des) != M100Success) return UHFWorkerEventFail;
    do {
        M100ResponseType rp_type = m100_write_label_data_storage(
            uhf_worker->module, uhf_tag_from, uhf_tag_des, UserBank, 0, 0);
        if(uhf_worker->state == UHFWorkerStateStop) return UHFWorkerEventAborted;
        if(rp_type == M100Success) break;
    } while(true);
    do {
        M100ResponseType rp_type = m100_write_label_data_storage(
            uhf_worker->module, uhf_tag_from, uhf_tag_des, EPCBank, 0, 0);
        if(uhf_worker->state == UHFWorkerStateStop) return UHFWorkerEventAborted;
        if(rp_type == M100Success) break;
    } while(true);
    return UHFWorkerEventSuccess;
}

int32_t uhf_worker_task(void* ctx) {
    UHFWorker* uhf_worker = ctx;
    if(uhf_worker->state == UHFWorkerStateVerify) {
        UHFWorkerEvent event = verify_module_connected(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    } else if(uhf_worker->state == UHFWorkerStateDetectSingle) {
        UHFWorkerEvent event = read_single_card(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    } else if(uhf_worker->state == UHFWorkerStateWriteSingle) {
        UHFWorkerEvent event = write_single_card(uhf_worker);
        uhf_worker->callback(event, uhf_worker->ctx);
    }
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